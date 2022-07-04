#ifndef GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
#define GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
#pragma once

#include "gaussian_model.hpp"

#include <vector>

namespace gmm {

template<class T>
constexpr T clamp(T a, T min, T max) noexcept {
    return  a < min? min : (a > max? max : a);
}

/***
 * Models mixture of gaussians (GMM)
 */
template<class T, unsigned DIM>
class GaussianMixtureModel {
public:
    static_assert(DIM > 0, "At least dim must be 1");

    using GaussianT = GaussianModel<T, DIM>;

    GaussianMixtureModel() = default;

    GaussianMixtureModel(std::initializer_list<const GaussianT&> &list)
    : mixture_(list) {}

    [[nodiscard]]
    std::size_t size() const noexcept { return mixture_.size(); }

    [[nodiscard]]
    bool empty() const noexcept { return mixture_.empty(); }

    void clear() noexcept {
        mixture_.clear();
    }

    void add(GaussianT&& gaussian) {
        mixture_.template emplace_back(gaussian);
    }

    [[nodiscard]]
    auto max_variance_gaussian_index() const noexcept {
        int i = 0;
        auto it = std::max_element(mixture_.begin(), mixture_.end(), [](const auto& a, const auto& b) {
           return a.eigenvalues[2] < b.eigenvalues[2];
        });
        return std::distance(mixture_.begin(), it);
    }

    void remove(unsigned k) {
        if (k < mixture_.size())
            mixture_.erase(mixture_.begin() + k);
    }

    [[nodiscard]]
    T probability(const typename GaussianT::VecT & vec) const noexcept {
        T total_probability(0);
        T total_weights(0);
        for (const auto& gaussian : mixture_) {
            if (gaussian.a_priori_weight > 0) {
                total_probability += gaussian.a_priori_weight * gaussian.probability_density(vec);
                total_weights += gaussian.a_priori_weight;
            }
        }
        if (total_weights == 0) return 0;
        return total_probability / total_weights;
    }

    [[nodiscard]]
    T probability_given_k(const typename GaussianT::VecT & vec, unsigned k) const noexcept {
        T probability_density(0.);
        if (k < mixture_.size())
            probability_density = mixture_[k].probability_density(vec);
        return probability_density;
    }

    const GaussianT& operator[](unsigned k) const {
        return mixture_.at(k);
    }

    GaussianT& operator[](unsigned k) {
        return mixture_.at(k);
    }

    [[nodiscard]]
    T a_priori_weight_given_k(unsigned k) const noexcept {
        T a_priori(0.);
        if (k < mixture_.size())
            a_priori = mixture_[k].a_priori_weight;
        return a_priori;
    }

private:
    std::vector<GaussianT> mixture_;
};

struct FgBgGMM {
    GaussianMixtureModel<float, 3> fg;
    GaussianMixtureModel<float, 3> bg;
};

} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
