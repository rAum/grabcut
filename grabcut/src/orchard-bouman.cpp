#include <quantization/orchard-bouman.h>

#include <gmm/mean_covar_precompute.hpp>
#include <gmm/gaussian_mixture_model.hpp>
#include <gmm/build_gaussian.hpp>
#include "quantization/quantization_model.hpp"
#include <grabcut/segmentation_data.h>

#include <grabcut/trimap.hpp>

namespace quantization {

template<class T, int DIM>
auto split_force(const gmm::GaussianModel<T, DIM>& model,  const Eigen::Matrix<T, DIM, 1>& color) noexcept {
    return model.eigenvectors.col(2).dot(color);
}

struct DynamicGaussianComponent {
    gmm::MeanCovariancePrecompute<double, 3> data;
    gmm::GaussianModel<double, 3> gaussian;

    [[nodiscard]]
    std::size_t size() const noexcept { return data.size(); }

    DynamicGaussianComponent& clear_data() noexcept {
        data.clear();
        return *this;
    }

    DynamicGaussianComponent& update(size_t size) {
        gmm::build_gaussian(gaussian, data, size);
        return *this;
    }

    DynamicGaussianComponent& update() {
        gmm::build_gaussian(gaussian, data, data.size());
        return *this;
    }

    [[nodiscard]]
    auto max_eigenvalue() const noexcept {
        return gaussian.eigenvalues[2];
    }
};

void split_biggest_gaussian(const uint8_t *data, const grabcut::Shape &shape, const uint8_t *mask_data,
                            std::vector<DynamicGaussianComponent> &fg_gmm, std::vector<DynamicGaussianComponent> &bg_gmm,
                            std::vector<std::uint8_t> &gmm_component_map, int fg_id, int bg_id, int k) {
    if (fg_id == k && bg_id == k) {
        return;
    }

    const auto& fg = fg_gmm[fg_id].gaussian;
    const auto& bg = bg_gmm[bg_id].gaussian;

    const auto split_force_fg = split_force<double>(fg, fg.mean);
    const auto split_force_bg = split_force<double>(bg, bg.mean);

    const uint8_t* curr = data;
    const uint8_t* mask_curr = mask_data;
    const uint8_t* end = data + (shape.width * shape.height * shape.channels);
    uint8_t* gmm_c = gmm_component_map.data();

    std::size_t total_fg(0);
    for (auto& gaussian : fg_gmm) {
        total_fg += gaussian.size();
    }

    std::size_t total_bg(0);
    for (auto& gaussian : bg_gmm) {
        total_bg += gaussian.size();
    }

    if (k != fg_id) {
        fg_gmm[k].clear_data();
        fg_gmm[fg_id].clear_data();
    }

    if (k != bg_id) {
        bg_gmm[k].clear_data();
        bg_gmm[bg_id].clear_data();
    }
    while (curr != end) {
        Eigen::Vector3d color(curr[0], curr[1], curr[2]);
        if (fg_id != k && *mask_curr == grabcut::Foreground && *gmm_c == fg_id) {
            if (split_force(fg, color) > split_force_fg) {
                *gmm_c = static_cast<uint8_t>(k);
                fg_gmm[k].data.add(color);
            } else {
                fg_gmm[fg_id].data.add(color);
            }
        } else if (bg_id != k && *mask_curr == grabcut::Background && *gmm_c == bg_id) {
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

    if (k != fg_id) {
        fg_gmm[k].update(total_fg);
        fg_gmm[fg_id].update(total_fg);
    }

    if (k != bg_id) {
        bg_gmm[k].update(total_bg);
        bg_gmm[bg_id].update(total_bg);
    }
}

void quantize(const std::uint8_t* data, const grabcut::Shape& shape, const std::uint8_t* mask_data, QuantizationModel& result) {
    constexpr int max_k = 5;
    std::vector<DynamicGaussianComponent> fg_gmm, bg_gmm;
    fg_gmm.emplace_back();
    bg_gmm.emplace_back();

    const std::uint8_t* curr = data;
    const std::uint8_t* mask_curr = mask_data;
    const std::uint8_t* end = data + (shape.width * shape.height * shape.channels);

    while (curr != end) {
        Eigen::Vector3d color(curr[0], curr[1], curr[2]);
        switch(*mask_curr) {
            case grabcut::Trimap::Background:
                bg_gmm.front().data.add(color);
                break;
            case grabcut::Trimap::Foreground:
                fg_gmm.front().data.add(color);
            default:
                break;
        }
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
        split_biggest_gaussian(data, shape, mask_data, fg_gmm, bg_gmm, gmm_component_map, fg_id, bg_id, k);
        // find the highest variance (the highest eigenvalue) component to split more
        for (int i=0; i<k; ++i) {
            if (fg_gmm[fg_id].max_eigenvalue() < fg_gmm[i].max_eigenvalue()) {
                fg_id = i;
            }
            if (bg_gmm[bg_id].max_eigenvalue() < bg_gmm[i].max_eigenvalue()) {
                bg_id = i;
            }
        }
    }

    auto& final_fg_gmm = result.gmm[0];
    auto& final_bg_gmm = result.gmm[1];

    final_fg_gmm.clear();
    final_bg_gmm.clear();

    for (int i = 0; i < max_k; ++i) {
        final_fg_gmm.add(fg_gmm[i].gaussian.get_lean());
        final_bg_gmm.add(bg_gmm[i].gaussian.get_lean());
    }
}

} // namespace quantization
