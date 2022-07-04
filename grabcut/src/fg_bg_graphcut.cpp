#include <vector>
#include <memory>
#include <graph.h>

#include <grabcut/segmentation_data.h>
#include <quantization/quantization_model.hpp>

#include <grabcut/fg_bg_graphcut.hpp>

namespace grabcut {

namespace {

constexpr float color_distance_euclid(const std::uint8_t* a, const std::uint8_t* b) noexcept {
    int res[3] = { b[0] - a[0], b[1] - a[1], b[2] - a[2]};
    res[0] *= res[0]; res[1] *= res[1]; res[2] *= res[2];
    return sqrtf(float(res[0] + res[1] + res[2]) / 255.f);
}

} // namespace

struct FgBgGraphCut::Impl {
    std::unique_ptr<Graph> graph;
    std::vector<Graph::node_id> nodes;

    static constexpr float lambda = 50;
    static constexpr float maximum = 10 * lambda + 1;
};

FgBgGraphCut::FgBgGraphCut() {
    impl_ = std::make_unique<Impl>();
}

FgBgGraphCut::~FgBgGraphCut() = default;

void FgBgGraphCut::build_graph(const Shape shape, const std::uint8_t* imgdata) {
    const int total = shape.size();
    auto& graph = impl_->graph;
    auto& nodes = impl_->nodes;

    graph = std::make_unique<Graph>();
    nodes.clear();
    nodes.reserve(total);

    for (int i = 0; i < total; ++i) {
        nodes.push_back(graph->add_node());
    }

    constexpr float diag_distance = 1.f / M_SQRT2;
    constexpr float full_distance = 1.f;
    constexpr float beta = 0.13f;

    auto diag_weight = [&](float color_distance) -> float {
        return Impl::lambda * diag_distance * expf(-beta * color_distance);
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
    for (int i = 1; i < shape.height; ++i) {
        for (int j = 0; j < shape.width; ++j) {
            auto offset = std::distance(nodes.data(), this_line) * 3;
            auto offset_last = std::distance(nodes.data(), last_line) * 3;
            float edge_weight = color_distance_euclid(imgdata + offset, imgdata + offset_last);
            edge_weight = border_weight(edge_weight);
            graph->add_edge(*last_line, *this_line, edge_weight, edge_weight);
            ++last_line;
            ++this_line;
        }
    }

    // TODO: add diagnoal connectivity
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
                Eigen::Vector3f color(imgdata[offset], imgdata[offset+1], imgdata[offset+2]);
                color *= 1.f/255.f;
                // note: the switch between foreground and background weights is correct
                bg_sink_weight= -logf(foreground.probability(color));
                fg_src_weight = -logf(background.probability(color));
        };
        graph->set_tweights(node, fg_src_weight, bg_sink_weight);
        ++trimap;
    }
}

void FgBgGraphCut::run(SegmentationData &segdata) {
    auto graph = impl_->graph.get();

    graph->maxflow();

    auto out = segdata.segmap.data();
    auto node_id = impl_->nodes.data();
    for (const auto& trimap : segdata.trimap) {
        switch (trimap) {
            case Trimap::Background:
            case Trimap::Foreground:
                //*out = trimap;
                //break;
            default:
                //*out = Trimap::Foreground;
                auto seg_id = graph->what_segment(*node_id);
                *out = seg_id == Graph::SOURCE? Trimap::Foreground : Trimap::Background;
        };
        ++out;
        ++node_id;
    }
}

}  // namespace grabcut