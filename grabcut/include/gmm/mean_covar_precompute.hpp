#ifndef GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
#define GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
#pragma once

#include <Eigen/Core>

namespace gmm {

/**
 * This class is a way to online precompute mean & covariance matrix
 */
class MeanCovariancePrecompute {
public:
    /***
     * adds new observation to the set
     * @param value input vector / scalar
     */
    void add(const Eigen::Vector3d& value) noexcept {
        sum_ += value;
        Eigen::Matrix3d product = value * value.transpose();
        prod_ += product;
        ++n_;
    }

    /**
     * Simple getter
     * @return number of values added
     */
    [[nodiscard]]
    std::size_t size() const noexcept { return n_; }

    /**
     * Calculates mean value
     * @return mean
     */
    [[nodiscard]]
    auto get_mean() const noexcept { return (sum_ / n_).eval(); }

    /**
     * Calculates covariance matrix
     * @return covariance matrix
     */
    [[nodiscard]]
    auto get_covariance() const noexcept {
        auto mean = get_mean();
        decltype(prod_) mean_product = mean * mean.transpose();

        decltype(mean) phi;
        phi.setConstant(5e-15);
        decltype(prod_) ensure_invertible = phi.asDiagonal();

        decltype(prod_) covariance = ((prod_ / n_) - mean_product) + ensure_invertible;
        return covariance;
    }

private:
    Eigen::Vector3d sum_ = Eigen::Vector3d::Zero();
    Eigen::Matrix3d prod_ = Eigen::Matrix3d::Zero();
    std::size_t n_ = 0;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace gmm

#endif //GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
