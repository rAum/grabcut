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
 * Smaller structure to only make probability density estimation
 * @tparam T
 * @tparam DIM
 */
template<class T, int DIM=3>
struct GaussianModelLean {
    using VecT = Eigen::Matrix<T, DIM, 1>;
    using MatT = Eigen::Matrix<T, DIM, DIM>;

    GaussianModelLean(const MatT& inverse, const VecT& mean, T determinant, T priori)
        : inverse(inverse), mean(mean), a_priori_weight(priori) {
        constexpr T pip = detail::pow<T>(2*M_PI, DIM);
        normalizer = 1.0 / std::sqrt(pip * determinant);
        if (determinant <= T(5e-11)) {
            a_priori_weight = 0;
        }
    }

private:
    MatT inverse;
    VecT mean;
    double normalizer;

public:
    T a_priori_weight = T(0);
    T probability_density(const VecT& v) const noexcept {
        auto diff = v - mean;
        double distribution = diff.transpose() * (inverse * diff);
        return std::exp(-0.5 * distribution) * normalizer;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


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

    /**
     * Prepares a leaner object to calculcate probability density
     * @return GAussianModelLean object with copied some fields
     */
    GaussianModelLean<T, DIM> get_lean() const noexcept {
        return GaussianModelLean<T, DIM>{
          .inverse = inverse,
          .mean = mean,
          .determinant = determinant,
          .a_priori_weight = a_priori_weight,
        };
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MODEL_H
