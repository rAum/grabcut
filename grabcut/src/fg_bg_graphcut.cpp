#include <vector>
#include <memory>

#include <grabcut/segmentation_data.h>
#include <quantization/quantization_model.hpp>

#include <grabcut/fg_bg_graphcut.hpp>

#include <gcgraph.hpp>

namespace grabcut {

namespace {

constexpr float color_distance_euclid(const std::uint8_t* x, const std::uint8_t* y) noexcept {
    int res[3] = { y[0] - x[0], y[1] - x[1], y[2] - x[2]};
    return static_cast<float>(res[0]*res[0] + res[1]*res[1]+ res[2]*res[2]);
}

} // namespace

struct FgBgGraphCut::Impl {
    std::unique_ptr<cv::detail::GCGraph<double>> graph;
    std::vector<int> nodes;
    std::vector<float> edge_weights_up;
    std::vector<float> edge_weights_horizontal;
    std::vector<std::pair<float, float>> edge_weights_diagonal;
    float beta = 1.f;

    static constexpr float lambda = 50 ;
    static constexpr float maximum = 10 * lambda + 1;
};

FgBgGraphCut::FgBgGraphCut() {
    impl_ = std::make_unique<Impl>();
}

FgBgGraphCut::~FgBgGraphCut() = default;

void FgBgGraphCut::allocate(Shape shape) {
    impl_->graph = std::make_unique<cv::detail::GCGraph<double>>();
    impl_->nodes.clear();
    const size_t total = shape.size();
    impl_->nodes.reserve(total);

    for (size_t i = 0; i < total; ++i) {
        impl_->nodes.push_back(impl_->graph->addVtx());
    }
}

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

void FgBgGraphCut::precompute_edge_weights(const Shape shape, const std::uint8_t* imgdata) {
    const size_t total = shape.size();

    constexpr float diag_distance = 1.f / M_SQRT2;
    const float beta = impl_->beta;
    auto diag_weight = [&](float color_distance) -> float {
        return (Impl::lambda * diag_distance) * expf(-beta * color_distance);
    };
    auto border_weight = [&](float color_distance) -> float {
        return Impl::lambda * expf(-beta * color_distance);
    };

    // setup horizontal connections
    impl_->edge_weights_horizontal.assign(total, 0);
    float* start = impl_->edge_weights_horizontal.data();
    float* ptr = impl_->edge_weights_horizontal.data();
    for (int i = 0; i < shape.height; ++i) {
        for (int j = 0; j < shape.width - 1; ++j) {
            auto offset = std::distance(start, ptr) * 3;
            float edge_weight = color_distance_euclid(imgdata + offset, imgdata + offset + 3);
            *ptr = border_weight(edge_weight);
            ++ptr;
        }
        ++ptr;
    }

    // vertical connections
    impl_->edge_weights_up.assign(total, 0);
    start = impl_->edge_weights_up.data();
    float* last_line = start;
    float* this_line = start + shape.width;
    const float* end_line_el =  start + shape.size();
    while (this_line != end_line_el) {
        auto offset = std::distance(start, this_line) * 3;
        auto offset_last = std::distance(start, last_line) * 3;
        float edge_weight = color_distance_euclid(imgdata + offset, imgdata + offset_last);
        edge_weight = border_weight(edge_weight);
        *this_line = edge_weight;
        ++last_line;
        ++this_line;
    }

    constexpr bool with_diagonals = true;
    if (!with_diagonals)
        return;

    impl_->edge_weights_diagonal.assign(total, {0, 0});
    auto *out = impl_->edge_weights_diagonal.data();
    /// the first and last is column is missing a link, but it should be mostly fine ;)
    for (int i = 1; i < shape.height; ++i) {
        for (int j = 1; j < shape.width - 1; ++j) {
            const auto idx = i * shape.width + j;
            auto prev_i = (i - 1) * shape.width;

            auto weight = color_distance_euclid(imgdata + idx * 3, imgdata + prev_i * 3 + 3 * j - 3);
            weight = diag_weight(weight);

            auto weight2 = color_distance_euclid(imgdata + idx * 3, imgdata + prev_i * 3 + 3 * j + 3);
            weight2 = diag_weight(weight2);

            out[idx].first = weight;
            out[idx].second = weight2;
        }
    }
}


void FgBgGraphCut::build_graph(const Shape shape, const std::uint8_t* imgdata) {
    const int total = shape.size();
    auto& graph = impl_->graph;
    auto& nodes = impl_->nodes;

    int edge_count = 2*(4*total - 3*(shape.width + shape.height) + 2);
    graph->create(total, edge_count);

    // setup horizontal connections
    auto* ptr = nodes.data();
    const auto* hedge = impl_->edge_weights_horizontal.data();
    for (int i = 0; i < shape.height; ++i) {
        for (int j = 0; j < shape.width - 1; ++j) {
            auto offset = std::distance(nodes.data(), ptr);
            float edge_weight = hedge[offset];
            graph->addEdges(*ptr, *(ptr+1), edge_weight, edge_weight);
            ++ptr;
        }
        ++ptr;
    }
    // vertical connections
    auto last_line = nodes.data();
    auto this_line = nodes.data() + shape.width;
    const auto end_line_el = nodes.data() + shape.size();
    const auto* vedge = impl_->edge_weights_up.data();
    while (this_line != end_line_el) {
        auto offset = std::distance(nodes.data(), this_line);
        float edge_weight = vedge[offset];
        graph->addEdges(*last_line, *this_line, edge_weight, edge_weight);
        ++last_line;
        ++this_line;
    }

    constexpr bool with_diagonals = true;
    if (!with_diagonals)
        return;

    /// the first and last is column is missing a link, but it should be mostly fine ;)
    const auto* diag = impl_->edge_weights_diagonal.data();
    for (int i = 1; i < shape.height; ++i) {
        for (int j = 1; j < shape.width - 1; ++j) {
            const auto idx = i * shape.width + j;
            auto prev_i = (i - 1) * shape.width;
            auto w = diag[idx];
            graph->addEdges(nodes[idx], nodes[prev_i + j - 1], w.first, w.first);
            graph->addEdges(nodes[idx], nodes[prev_i + j + 1], w.second, w.second);
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
        double fg_sink_weight(0);
        double bg_source_weight(0);
        switch (*trimap) {
            case Trimap::Foreground:
                fg_sink_weight = maximum_value;
                break;
            case Trimap::Background:
                bg_source_weight = maximum_value;
                break;
            case Trimap::Unknown:
            default:
                auto offset = std::distance(segdata.trimap.data(), trimap) * 3;
                Eigen::Vector3d color(imgdata[offset], imgdata[offset+1], imgdata[offset+2]);
                // note: the switch between foreground and background weights is correct
                bg_source_weight= -log(foreground.probability(color));
                fg_sink_weight = -log(background.probability(color));
        };
        graph->addTermWeights(node, bg_source_weight, fg_sink_weight);
        ++trimap;
    }
}

bool FgBgGraphCut::run(SegmentationData &segdata) {
    auto graph = impl_->graph.get();

    graph->maxFlow();

    int changed_pixels(0);

    auto out = segdata.segmap.data();
    auto node_id = impl_->nodes.data();
    for (auto trimap : segdata.trimap) {
        if (trimap == Trimap::Foreground) *out = Trimap::Foreground;
        else if (trimap == Trimap::Background) *out = Trimap::Background;
        else {
            auto seg_id = !graph->inSourceSegment(*node_id);
            auto new_value = seg_id? Trimap::Foreground : Trimap::Background;
            if (new_value != *out) {
                ++changed_pixels;
            }
            *out = new_value;
        }
        ++out;
        ++node_id;
    }
    return changed_pixels < 5;
}

}  // namespace grabcut