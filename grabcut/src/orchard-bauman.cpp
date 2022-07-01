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

float split_force(const gmm::GaussianModel<float, 3>& model,  const Eigen::Vector3f& color) noexcept {
    return model.eigenvectors.col(2).dot(color);
}

struct DynamicGaussianComponent {
    gmm::MeanCovariancePrecompute<float, 3> data;
    gmm::GaussianModel<float, 3> gaussian;

    std::size_t size() const noexcept { return data.size(); }

    DynamicGaussianComponent& clear_data() noexcept {
        data.clear();
        return *this;
    }

    DynamicGaussianComponent& update() {
        gmm::build_gaussian(gaussian, data, data.size());
        return *this;
    }

    float max_eigenvalue() const noexcept {
        return gaussian.eigenvalues[2];
    }
};

struct ForegroundBackgroundDynamicGMM {
    std::array<std::vector<DynamicGaussianComponent>, 2> gmm;
    std::array<int, 2> strongest_k;
    std::vector<std::uint8_t> component_map;
};

void grow_gmm(const uint8_t *data, const Shape &shape, const uint8_t *mask_data,
              std::vector<DynamicGaussianComponent> &fg_gmm, std::vector<DynamicGaussianComponent> &bg_gmm,
              std::vector<std::uint8_t> &gmm_component_map, int fg_id, int bg_id, int k) {
    constexpr float to_zero_one = 1.f / 255.f;

    const auto& fg = fg_gmm[fg_id].gaussian;
    const auto& bg = bg_gmm[bg_id].gaussian;

    const auto split_force_fg = split_force(fg, fg.mean);
    const auto split_force_bg = split_force(bg, bg.mean);

    const uint8_t* curr = data;
    const uint8_t* mask_curr = mask_data;
    const uint8_t* end = data + (shape.width * shape.height * shape.channels);
    uint8_t* gmm_c = gmm_component_map.data();
    while (curr != end) {
        Eigen::Vector3f color(curr[0], curr[1], curr[2]);
        color *= to_zero_one;
        if (*mask_curr == grabcut::Foreground && *gmm_c == fg_id) {
            if (split_force(fg, color) > split_force_fg) {
                *gmm_c = static_cast<uint8_t>(k);
                fg_gmm[k].data.add(color);
            } else {
                fg_gmm[fg_id].data.add(color);
            }
        } else if (*mask_curr == grabcut::Background && *gmm_c == bg_id) {
            if (split_force(bg, color) > split_force_bg) {
                *gmm_c = static_cast<uint8_t>(k);
                bg_gmm[k].data.add(color);
            } else {
                bg_gmm[bg_id].data.add(color);
            }
        }
        curr += 3;
        ++gmm_c;
        ++mask_curr;
    }

    for (auto idx : {k, fg_id}) {
        fg_gmm[idx].update().clear_data();
        bg_gmm[idx].update().clear_data();
    }
}

std::vector<std::uint8_t> quantize(const std::uint8_t* data, const Shape& shape, const std::uint8_t* mask_data) {
    constexpr int max_k = 5;
    std::vector<DynamicGaussianComponent> fg_gmm, bg_gmm;
    fg_gmm.emplace_back();
    bg_gmm.emplace_back();

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
                bg_gmm.front().data.add(color);
                break;
            case grabcut::Trimap::Foreground:
                fg_gmm.front().data.add(color);
            default:
                break;
        };
        curr += 3;
        ++mask_curr;
    }
    if (fg_gmm.front().size() == 0 || bg_gmm.front().size() == 0) {
        throw std::runtime_error("Failed to produce gaussian as fg/bg pixels count is "
                                 + std::to_string(fg_gmm.front().size()) + "/" + std::to_string(bg_gmm.front().size()));
    }

    fg_gmm.front().update();
    bg_gmm.front().update();

    std::vector<std::uint8_t> gmm_component_map(shape.width * shape.height, 0);

    fg_gmm.resize(max_k);
    bg_gmm.resize(max_k);

    // Build GMM
    auto fg_id = 0;
    auto bg_id = 0;
    // TODO: probably can be rewritten to be more effective
    for (int k = 0; k < max_k; ++k) {
        grow_gmm(data, shape, mask_data, fg_gmm, bg_gmm, gmm_component_map, fg_id, bg_id, k);
        // find the highest variance (highest eigenvalue) component to split more
        for (int i=0; i<k; ++i) {
            if (fg_gmm[fg_id].max_eigenvalue() < fg_gmm[i].max_eigenvalue()) {
                fg_id = i;
            }
            if (bg_gmm[bg_id].max_eigenvalue() < bg_gmm[i].max_eigenvalue()) {
                bg_id = i;
            }
        }
    }

//    gmm::GaussianMixtureModel<float, 3> final_fg_gmm;
//    gmm::GaussianMixtureModel<float, 3> final_bg_gmm;
//
//    for (int i = 0; i < max_k; ++i) {
//        final_fg_gmm.add(std::move(fg_gmm[i].gaussian));
//        final_bg_gmm.add(std::move(bg_gmm[i].gaussian));
//    }
    return gmm_component_map;
}

} // namespace quantization
