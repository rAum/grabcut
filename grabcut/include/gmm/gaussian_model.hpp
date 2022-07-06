#ifndef GRABCUT_GAUSSIAN_MODEL_H
#define GRABCUT_GAUSSIAN_MODEL_H
#pragma  once

#include <Eigen/Core>

namespace gmm {

namespace detail {

template <class T>
constexpr T pow(T x, unsigned y) noexcept {
    return y == 0 ? 1 : x * pow(x, y - 1);
}

}

/**
 * Represents Gaussian Model for distribution modelling
 */
template<class T, int DIM=3>
struct GaussianModel {
    using VecT = Eigen::Matrix<T, DIM, 1>;
    using MatT = Eigen::Matrix<T, DIM, DIM>;

    MatT covariance;
    MatT inverse;
    MatT eigenvectors;
    VecT mean;
    VecT eigenvalues;
    T determinant = T(0);
    T a_priori_weight = T(0);

    T probability_density(const VecT& v) const noexcept {
        constexpr T pip = detail::pow<T>(2*M_PI, DIM);
        double probability_density(0.);
        // TODO: yeah... there might be numerical issues with inverse
        // so for now let's ignore anything with small determinant
        if (determinant > T(5e-11)) {
            auto diff = v - mean;
            double distribution = diff.transpose() * (inverse * diff);
            probability_density = std::exp(-0.5 * distribution) / std::sqrt(pip * determinant);
        }

        return probability_density;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};




} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MODEL_H
