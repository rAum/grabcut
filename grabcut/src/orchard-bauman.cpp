#include <quantization/orchard-bauman.h>

#include <gmm/mean_covar_precompute.hpp>
#include <gmm/gaussian_mixture_model.hpp>
#include <gmm/build_gaussian.hpp>

#include <grabcut/trimap.hpp>

#include <memory>

namespace quantization {

std::unique_ptr<float[]> to_float(const std::uint8_t*data, const Shape& shape) {
    const int total_values = shape.width * shape.height * shape.channels;
    const std::uint8_t* curr = data;
    const std::uint8_t* end = data + (shape.width * shape.height * shape.channels);
    std::unique_ptr<float[]> float_img(new float[total_values]);
    float* out = float_img.get();
    // should gamma correct?
    for (; curr != end; ++curr, ++out) {
        *out = *curr / 255.f;
    }
    return float_img;
}

void quantize(const std::uint8_t* data, const Shape& shape, const std::uint8_t* mask_data) {
    (void)data;
    (void)shape;
    gmm::GaussianMixtureModel<float, 3> foreground_gmm;
    gmm::GaussianMixtureModel<float, 3> background_gmm;

    int max_k = 5;
    gmm::MeanCovariancePrecompute<float, 3> fg_pixels, bg_pixels;

    const int total_values = shape.width * shape.height * shape.channels;
    const std::uint8_t* curr = data;
    const std::uint8_t* mask_curr = mask_data;
    const std::uint8_t* end = data + (shape.width * shape.height * shape.channels);

    constexpr float to_zero_one = 1.f/255.f;
    while (curr != end) {
        Eigen::Vector3f color(curr[0], curr[1], curr[2]);
        color *= to_zero_one;
        switch(*mask_curr) {
            case grabcut::Trimap::Background:
                bg_pixels.add(color);
                break;
            case grabcut::Trimap::Foreground:
                fg_pixels.add(color);
            default:
                break;
        };
        curr += 3;
        ++mask_curr;
    }

    gmm::build_gaussian<>
}

} // namespace quantization
