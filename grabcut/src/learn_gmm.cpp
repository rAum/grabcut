#include <gmm/learn_gmm.hpp>

#include <grabcut/segmentation_data.h>
#include <quantization/quantization_model.hpp>
#include <gmm/mean_covar_precompute.hpp>
#include <gmm/build_gaussian.hpp>

namespace gmm {

void learn(QuantizationModel& model, const grabcut::SegmentationData& segdata, const std::uint8_t* colors) noexcept {
    using Trimap = grabcut::Trimap;

    auto& fg = model.gmm[0];
    auto& bg = model.gmm[1];

    auto col = colors;
    auto mask = segdata.segmap.data();
    const auto end = model.component_map.data() + model.component_map.size();
    for (auto component = model.component_map.data(); component != end; ++component, ++mask, col += 3) {
        Eigen::Vector3d color(col[0], col[1], col[2]);
        *component = *mask == Trimap::Background? bg.strongest_k(color) : fg.strongest_k(color);
    }

    std::vector<gmm::MeanCovariancePrecompute<double, 3>> fg_gmm, bg_gmm;
    fg_gmm.resize(fg.size());
    bg_gmm.resize(bg.size());
    mask = segdata.segmap.data();
    col = colors;
    for (auto component = model.component_map.data(); component != end; ++component, ++mask, col += 3) {
        Eigen::Vector3d color(col[0], col[1], col[2]);
        auto& mixture = *mask == Trimap::Background? bg_gmm : fg_gmm;
        mixture[*component].add(color);
    }

    size_t total_fg_size = 0;
    for (const auto& mix : fg_gmm) {
        total_fg_size += mix.size();
    }
    for (size_t k = 0; k < fg.size(); ++k) {
        gmm::build_gaussian(fg[k], fg_gmm[k], total_fg_size);
    }

    size_t total_bg_size = 0;
    for (const auto& mix : bg_gmm) {
        total_bg_size += mix.size();
    }
    for (size_t k = 0; k < fg.size(); ++k) {
        gmm::build_gaussian(bg[k], bg_gmm[k], total_bg_size);
    }
}

}  // namespace gmm
