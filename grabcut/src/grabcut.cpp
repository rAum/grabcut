#include <grabcut/grabcut.h>

#include <grabcut/segmentation_data.h>
#include <quantization/orchard-bouman.h>
#include <quantization/quantization_model.hpp>
#include <grabcut/fg_bg_graphcut.hpp>
#include <gmm/learn_gmm.hpp>

namespace grabcut {

struct Grabcut::GbData {
    const std::uint8_t* image;
    Shape shape;
    SegmentationData segmentation;
    QuantizationModel color_model;
    FgBgGraphCut graphcut;
    bool converged = false;

    void init(const std::uint8_t* image, const std::uint8_t* mask, int width, int height, int channels) {
        shape.width = width;
        shape.height = height;
        shape.channels = channels;
        this->image = image;
        segmentation.init_from(shape, mask);
        converged = false;
    }

    void run() {
        if (converged)
            return;

        if (color_model.gmm[0].empty()) {
            quantization::quantize(image, shape, segmentation.segmap.data(), color_model);
            graphcut.estimate_beta(shape, image);
            graphcut.precompute_edge_weights(shape, image);
            graphcut.allocate(shape);
        }
        else {
            gmm::learn(color_model, segmentation, image);
        }
        graphcut.build_graph(shape, image);
        graphcut.update_sink_source(color_model, image, segmentation);
        converged = graphcut.run(segmentation);
    }
};

Grabcut::Grabcut() {
    impl_ = std::make_unique<Grabcut::GbData>();
}

Grabcut::~Grabcut() = default;

Grabcut& Grabcut::init(const std::uint8_t* image, const std::uint8_t* mask, int width, int height, int channels) {
    impl_->init(image, mask, width, height, channels);
    return *this;
}

Grabcut& Grabcut::run(int steps) {
    for (int i = 0; i < steps; ++i) {
        impl_->run();
    }
    return *this;
}

const std::vector<std::uint8_t>& Grabcut::get_mask() const {
    return impl_->segmentation.segmap;
};

std::vector<std::uint8_t> Grabcut::get_result() const {
    return impl_->segmentation.make_rgba(impl_->image, 0, 255);
}

}  // namespace grabcut
