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
template<class T, int DIM>
class GaussianMixtureModel {
public:
    using GaussianT = GaussianModel<T, DIM>;

    GaussianMixtureModel() = default;

    GaussianMixtureModel(std::initializer_list<const GaussianT&> &list)
    : mixture_(list) {}

    [[nodiscard]]
    std::size_t size() const noexcept { return mixture_.size(); }

    [[nodiscard]]
    bool empty() const noexcept { return mixture_.empty(); }

    void add(GaussianT&& gaussian) {
        mixture_.template emplace_back(gaussian);
    }

    void remove(unsigned k) {
        if (k < mixture_.size())
            mixture_.erase(mixture_.begin() + k);
    }

    [[nodiscard]]
    T probability(const typename GaussianT::VecT & vec) const noexcept {
        T total_probability(0.);

        for (const auto& gaussian : mixture_) {
            if (gaussian.a_priori_weight > 0)
                total_probability += gaussian.a_priori_weight * gaussian.probability_density(vec);
        }
        return clamp(total_probability, 0., 1.);
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

} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
