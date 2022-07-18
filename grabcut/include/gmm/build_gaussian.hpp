#pragma once
#ifndef GRABCUT_BUILD_GAUSSIAN_HPP
#define GRABCUT_BUILD_GAUSSIAN_HPP

#include "gmm/gaussian_model.hpp"
#include "gmm/mean_covar_precompute.hpp"
#include <Eigen/Dense>

namespace gmm {

/**
 * From precomputed mean and covariance it computes GaussianModel fields like covariance inverce, determinant
 * eigen values and eigen vectors etc.
 * @tparam T numeric type
 * @tparam DIM dimensions > 1
 * @param mean_cov precomputed mean and covariance for subset of samples
 * @param gaussian a target GaussianModel object
 * @param total_samples total number of observations
 * @return ratio between total samples and points in mean_cov
 */
template<class T, int DIM>
void build_gaussian(gmm::GaussianModel<T, DIM> &gaussian, const gmm::MeanCovariancePrecompute<T, DIM>& mean_cov, unsigned total_samples) noexcept {
    static_assert(DIM > 0, "Dimension size must be at least 1");

    gaussian.a_priori_weight = 0;
    if (mean_cov.size() != 0) {
        gaussian.a_priori_weight = mean_cov.size() / decltype(gaussian.a_priori_weight)(total_samples);
    }
    gaussian.mean = mean_cov.get_mean();
    gaussian.covariance = mean_cov.get_covariance();
    gaussian.determinant = gaussian.covariance.determinant();
    gaussian.inverse = gaussian.covariance.inverse();

    Eigen::SelfAdjointEigenSolver<decltype(gaussian.covariance)> solver(gaussian.covariance);
    gaussian.eigenvectors = solver.eigenvectors();
    gaussian.eigenvalues = solver.eigenvalues();
}

/**
 * From precomputed mean and covariance it computes GaussianModelLite fields like covariance inverce, determinant etc
 * @tparam T numeric type
 * @tparam DIM dimensions > 1
 * @param mean_cov precomputed mean and covariance for subset of samples
 * @param gaussian a target GaussianModel object
 * @param total_samples total number of observations
 * @return ratio between total samples and points in mean_cov
 */
template<class T, int DIM>
void build_gaussian(gmm::GaussianModelLean<T, DIM> &gaussian, const gmm::MeanCovariancePrecompute<T, DIM>& mean_cov, unsigned total_samples) noexcept {
    static_assert(DIM > 0, "Dimension size must be at least 1");

    double a_priori_weight(0);
    if (mean_cov.size() != 0) {
        a_priori_weight = mean_cov.size() / decltype(a_priori_weight)(total_samples);
    }
    auto covariance = mean_cov.get_covariance();
    gaussian = gmm::GaussianModelLean<T, DIM>{covariance.inverse(), mean_cov.get_mean(), covariance.determinant(), a_priori_weight};
}

}  // namespace gmm

#endif //GRABCUT_BUILD_GAUSSIAN_HPP
