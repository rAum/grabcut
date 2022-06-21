#ifndef GRABCUT_GAUSSIAN_MODEL_H
#define GRABCUT_GAUSSIAN_MODEL_H
#pragma  once

#include <Eigen/Core>

namespace gmm {

/**
 * Represents Gaussian Model for distribution modelling
 */
template<class T, unsigned DIM=3>
struct GaussianModel {
    Eigen::Matrix<T, DIM, DIM> covariance;
    Eigen::Matrix<T, DIM, DIM> inverse;
    Eigen::Matrix<T, DIM, DIM> eigenvectors;
    Eigen::Matrix<T, DIM, 1> mean;
    Eigen::Matrix<T, DIM, 1> eigenvalues;
    T determinant;

    T probability_density(const Eigen::Matrix<T, DIM, 1>& v) const noexcept {
        const T pip = std::pow(M_PI, DIM);
        double probability_density(0.);
        if (determinant > 0) {
            auto diff = v - mean;
            double distribution = diff.dot((inverse * diff));
            probability_density = std::exp(-0.5 * distribution) / std::sqrt(pip * determinant);
        }
        return probability_density;
    }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};




} // namespace gmm

#endif //GRABCUT_GAUSSIAN_MODEL_H
