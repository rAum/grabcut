#ifndef GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
#define GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
#pragma once

#include <Eigen/Core>

namespace gmm {

/**
 * This class is a way to online precompute mean & covariance matrix/
  * @tparam T numeric type for values
 * @tparam DIM dimension size
 */
template<class T, unsigned DIM=3>
class MeanCovariancePrecompute {
public:
    using MatT = Eigen::Matrix<T, DIM, DIM>;
    using VecT = Eigen::Matrix<T, DIM, 1>;

    /***
     * adds new observation to the set
     * @param value input vector / scalar
     */
    void add(const VecT& value) noexcept {
        sum_ += value;
        prod_ += value * value.transpose();
        ++n_;
    }

    /**
     * Adds multiple values by inserting 3 x N matrix
     * @param values mmatrix to be inserted
     */
    template<int Cols>
    void add(const Eigen::Matrix<T, DIM, Cols>& values) noexcept {
        sum_ += values.rowwise().sum();
        prod_ += values * values.transpose();
        n_ += values.cols();
    }

    /***
     * Adds a single elment as sequence of values (in number equal to dimension)
     * @tparam V variadic sequence of numeric values
     * @param v a value
     */
    template<class ...V>
    void add(V... v) noexcept {
        static_assert(sizeof...(V) == DIM, "Only defined if dimensions match");
        add(Eigen::Matrix<T, sizeof...(V), 1>{v..., });
    }

    /**
     * Simple getter
     * @return number of values added
     */
    [[nodiscard]]
    std::size_t size() const noexcept { return n_; }

    /**
     *  Clears all values (sets everything to zero)
     */
    void clear() noexcept {
        prod_ = MatT::Zero();
        sum_ = VecT::Zero();
        n_ = 0;
    }

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
    MatT prod_ = MatT::Zero();
    VecT sum_ = VecT::Zero();
    std::size_t n_ = 0;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace gmm

#endif //GRABCUT_MEAN_VAR_PRECOMPUTE_HPP
