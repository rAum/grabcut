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

    std::vector<gmm::MeanCovariancePrecompute<double, 3>> fg_gmm, bg_gmm;
    fg_gmm.resize(fg.size());
    bg_gmm.resize(bg.size());
    auto col = colors;
    const auto end = segdata.segmap.data() + segdata.segmap.size();
    for (auto mask = segdata.segmap.data(); mask != end; ++mask, col += 3) {
        Eigen::Vector3d color(col[0], col[1], col[2]);
        const bool is_bg = *mask == Trimap::Background;
        const auto k = is_bg? bg.strongest_k(color) : fg.strongest_k(color);
        auto& mixture = is_bg? bg_gmm : fg_gmm;
        mixture[k].add(color);
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
