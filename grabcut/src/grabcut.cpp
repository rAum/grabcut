#include <grabcut/grabcut.h>

#include <grabcut/segmentation_data.h>
#include <quantization/orchard-bouman.h>
#include <quantization/quantization_model.hpp>

namespace grabcut {

struct Grabcut::GbData {
    const std::uint8_t* image;
    Shape shape;
    SegmentationData segmentation;
    QuantizationModel color_model;
    static constexpr int lambda = 50;
    static constexpr int change_fg_bg_cost = 10 * lambda + 1;


    void init(const std::uint8_t* image, const std::uint8_t* mask, int width, int height, int channels) {
        shape.width = width;
        shape.height = height;
        shape.channels = channels;
        this->image = image;
        segmentation.init_from(shape, mask);
        quantization::quantize(image, shape, mask, color_model);
    }

    void run() {
        // for now just run slassification based on foreground & background GMM
        auto out = segmentation.segmap.data();
        constexpr float to_zero_one = 1.f/255.f;
        for(auto rgb=image; rgb != image + shape.chsize(); rgb += 3, out += 1) {
            Eigen::Vector3f color{rgb[0] * to_zero_one, rgb[1] * to_zero_one, rgb[2] * to_zero_one};
            auto fg = color_model.gmm[0].probability(color);
            auto bg = color_model.gmm[1].probability(color);
            *out = fg > bg? Trimap::Foreground : Trimap::Background;
        }

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
