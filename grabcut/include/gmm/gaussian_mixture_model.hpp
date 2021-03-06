#pragma once
#ifndef GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
#define GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP

#include "gaussian_model.hpp"

#include <vector>

namespace gmm {

/***
 * Models mixture of gaussians (GMM)
 */
template<class T, unsigned DIM>
class GaussianMixtureModel {
public:
    static_assert(DIM > 0, "At least dim must be 1");

    using GaussianT = GaussianModelLean<T, DIM>;

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

    [[nodiscard]]
    size_t strongest_k(const typename GaussianT::VecT& vec) const noexcept {
        size_t k = 0;
        T max_prob = -1;
        size_t max_k = 0;
        for (; k < mixture_.size(); ++k) {
            auto prob = probability_given_k(vec, k);
            if (max_prob < prob) {
                max_k = k;
                max_prob = prob;
            }
        }
        return max_k;
    }

    void remove(unsigned k) {
        if (k < mixture_.size())
            mixture_.erase(mixture_.begin() + k);
    }

    [[nodiscard]]
    T probability(const typename GaussianT::VecT & vec) const noexcept {
        T total_probability(0);
        for (const auto& gaussian : mixture_) {
            if (gaussian.a_priori_weight > 0) {
                total_probability += gaussian.a_priori_weight * gaussian.probability_density(vec);
            }
        }
        return total_probability;
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

} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MIXTURE_MODEL_HPP
