#ifndef GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
#define GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
#pragma once

#include "gaussian_model.hpp"

#include <vector>

namespace gmm {

/***
 * Models mixture of gaussians (GMM)
 */
class GaussianMixtureModel {
public:
    explicit GaussianMixtureModel(unsigned k_mixtures);

    [[nodiscard]]
    constexpr unsigned get_count() const noexcept { return k_mixtures_; }

    [[nodiscard]]
    double probability(const Eigen::Vector3f& vector) const noexcept;

    [[nodiscard]]
    double probability_given_k(const Eigen::Vector3f& vec, unsigned k) const noexcept;

private:
    const unsigned k_mixtures_;
    std::vector<GaussianModel<float, 3>> mixture_;
    std::vector<float> gaussian_weight_;
};

} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
