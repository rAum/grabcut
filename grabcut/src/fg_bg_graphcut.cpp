#include <vector>
#include <memory>
#include <maxflow.h>

#include <grabcut/segmentation_data.h>
#include <quantization/quantization_model.hpp>

#include <grabcut/fg_bg_graphcut.hpp>

using Graph = maxflow::Graph_FFF;

namespace grabcut {

namespace {

constexpr float color_distance_euclid(const std::uint8_t* x, const std::uint8_t* y) noexcept {
    int res[3] = { y[0] - x[0], y[1] - x[1], y[2] - x[2]};
    float r(res[0]/255.f), g(res[1]/255.f), b(res[2]/255.f);
    return r*r + g*g + b*b;
}

} // namespace

struct FgBgGraphCut::Impl {
    std::unique_ptr<Graph> graph;
    std::vector<Graph::node_id> nodes;
    float beta = 1.f;

    static constexpr float lambda = 50;
    static constexpr float maximum = 10 * lambda + 1;
};

FgBgGraphCut::FgBgGraphCut() {
    impl_ = std::make_unique<Impl>();
}

FgBgGraphCut::~FgBgGraphCut() = default;

void FgBgGraphCut::estimate_beta(const Shape shape, const std::uint8_t* image) noexcept {
    const int w = shape.width;
    const int h = shape.height;
    double beta(0);
    int edges(0);
    for (int i = 1; i < h; ++i) {
        for (int j = 1; j < w-1; ++j) {
            auto curr_indx = image + 3*(i*w+j);

            auto left = color_distance_euclid(curr_indx, image + 3*(i*w + j-1));
            auto up = color_distance_euclid(curr_indx, image + 3*((i-1)*w +j));
            auto up_left = color_distance_euclid(curr_indx, image + 3*((i-1)*w + j-1));
            auto up_right = color_distance_euclid(curr_indx, image + 3*((i-1)*w + j+1));
            beta += left + up + up_left + up_right;
            edges += 4;
        }
    }

    if (beta <= std::numeric_limits<decltype(beta)>::epsilon()) {
        beta = 1.f;
    }

    const auto final_beta = static_cast<float>(edges / (2.f * beta));
    impl_->beta =final_beta;
}


void FgBgGraphCut::build_graph(const Shape shape, const std::uint8_t* imgdata) {
    const int total = shape.size();
    auto& graph = impl_->graph;
    auto& nodes = impl_->nodes;

    graph = std::make_unique<Graph>(total, total*8/2);
    nodes.clear();
    nodes.reserve(total);

    for (int i = 0; i < total; ++i) {
        nodes.push_back(graph->add_node());
    }

    constexpr float diag_distance = 1.f / M_SQRT2;
    const float beta = impl_->beta;

    auto diag_weight = [&](float color_distance) -> float {
        return (Impl::lambda * diag_distance) * expf(-beta * color_distance);
    };
    auto border_weight = [&](float color_distance) -> float {
        return Impl::lambda * expf(-beta * color_distance);
    };

    // setup horizontal connections
    Graph::node_id* ptr = nodes.data();
    for (int i = 0; i < shape.height; ++i) {
        for (int j = 0; j < shape.width - 1; ++j) {
            auto offset = std::distance(nodes.data(), ptr) * 3;
            float edge_weight = color_distance_euclid(imgdata + offset, imgdata + offset + 3);
            edge_weight = border_weight(edge_weight);
            graph->add_edge(*ptr, *(ptr+1), edge_weight, edge_weight);
            ++ptr;
        }
        ++ptr;
    }
    // vertical connections
    Graph::node_id* last_line = nodes.data();
    Graph::node_id* this_line = nodes.data() + shape.width;
    const Graph::node_id* end_line_el = nodes.data() + shape.size();
    while (this_line != end_line_el) {
        auto offset = std::distance(nodes.data(), this_line) * 3;
        auto offset_last = std::distance(nodes.data(), last_line) * 3;
        float edge_weight = color_distance_euclid(imgdata + offset, imgdata + offset_last);
        edge_weight = border_weight(edge_weight);
        graph->add_edge(*last_line, *this_line, edge_weight, edge_weight);
        ++last_line;
        ++this_line;
    }

    constexpr bool with_diagonals = true;
    if (!with_diagonals)
        return;

    /// the first and last is column is missing a link, but it should be mostly fine ;)
    for (int i = 1; i < shape.height; ++i) {
        for (int j = 1; j < shape.width - 1; ++j) {
            const auto idx = i * shape.width + j;
            auto prev_i = (i - 1) * shape.width;

            auto weight = color_distance_euclid(imgdata + idx * 3, imgdata + prev_i * 3 + 3 * j - 3);
            weight = diag_weight(weight);
            graph->add_edge(nodes[idx], nodes[prev_i + j - 1], weight, weight);

            auto weight2 = color_distance_euclid(imgdata + idx * 3, imgdata + prev_i * 3 + 3 * j + 3);
            weight2 = diag_weight(weight2);
            graph->add_edge(nodes[idx], nodes[prev_i + j + 1], weight2, weight2);
        }
    }
}

void FgBgGraphCut::update_sink_source(const QuantizationModel &color_model, const std::uint8_t *imgdata,
                                      const SegmentationData &segdata) {
    auto& graph = impl_->graph;
    auto& nodes = impl_->nodes;

    constexpr float maximum_value = Impl::maximum;
    const auto& foreground = color_model.gmm[0];
    const auto& background = color_model.gmm[1];
    auto trimap = segdata.trimap.data();
    for (auto& node : nodes) {
        float fg_src_weight = 0;
        float bg_sink_weight = 0;
        switch (*trimap) {
            case Trimap::Foreground:
                fg_src_weight = maximum_value;
                break;
            case Trimap::Background:
                bg_sink_weight = maximum_value;
                break;
            case Trimap::Unknown:
            default:
                auto offset = std::distance(segdata.trimap.data(), trimap) * 3;
                Eigen::Vector3d color(imgdata[offset], imgdata[offset+1], imgdata[offset+2]);
                color *= 1.f/255.f;
                // note: the switch between foreground and background weights is correct
                bg_sink_weight= -log(foreground.probability(color));
                fg_src_weight = -log(background.probability(color));
        };
        graph->add_tweights(node, fg_src_weight, bg_sink_weight);
        ++trimap;
    }
}

bool FgBgGraphCut::run(SegmentationData &segdata) {
    auto graph = impl_->graph.get();

    graph->maxflow();

    int changed_pixels(0);

    auto out = segdata.segmap.data();
    auto node_id = impl_->nodes.data();
    for (const auto& trimap : segdata.trimap) {
        auto seg_id = graph->what_segment(*node_id);
        auto new_value = seg_id == Graph::SOURCE? Trimap::Foreground : Trimap::Background;
        if (new_value != *out) {
            ++changed_pixels;
        }
        *out = new_value;
        ++out;
        ++node_id;
    }
    return changed_pixels < 5;
}

}  // namespace grabcut