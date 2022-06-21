#include "gmm/gaussian_mixture_model.hpp"

template<class T>
constexpr T clamp(T a, T min, T max) noexcept {
    return  a < min? min : (a > max? max : a);
}

namespace gmm {

GaussianMixtureModel::GaussianMixtureModel(unsigned k_mixtures) : k_mixtures_(k_mixtures) {

}

double GaussianMixtureModel::probability(const Eigen::Vector3f& vector) const noexcept {
    double total_probability(0.);

    for (int i = 0; i < gaussian_weight_.size(); ++i) {
        if (gaussian_weight_[i] > 0)
            total_probability += gaussian_weight_[i] * probability_given_k(vector, i);
    }
    return clamp(total_probability, 0., 1.);
}

double GaussianMixtureModel::probability_given_k(const Eigen::Vector3f& vec, unsigned k) const noexcept {
    double probability_density(0.);

    if (k < gaussian_weight_.size())
        probability_density = mixture_[k].probability_density(vec);

    return probability_density;
}

} // namespace gmm