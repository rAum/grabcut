#include <doctest.h>
#include <Eigen/Dense>

#include <gmm/mean_covar_precompute.hpp>

TEST_CASE("Test mean variance precompute") {
    Eigen::Vector3d a(1., 1., 1.);
    gmm::MeanCovariancePrecompute<double, 3> pre;
    CHECK_EQ(pre.size(), 0);

    SUBCASE("Clearing zeroes values") {
        pre.add(a);
        CHECK_EQ(pre.size(), 1);
        pre.clear();
        CHECK_EQ(pre.size(), 0);
    }

    SUBCASE("Test adding single vector") {
        pre.add(a);
        CHECK_EQ(pre.size(), 1);
        CHECK_EQ(pre.get_mean(), a);
        auto covariance = pre.get_covariance();
        CHECK_GT(covariance.determinant(), 0.);
    }

    SUBCASE("Adding two points gives a valid values") {
        Eigen::Vector3d b(1., 2., 3.);
        pre.add(a);
        pre.add(b);
        CHECK_EQ(pre.size(), 2);
        auto mean = (a + b) * 0.5;
        CHECK(pre.get_mean().isApprox(mean));
        auto covariance = pre.get_covariance();
        CHECK_GT(covariance.determinant(), 0.);
    }

    SUBCASE("Test variadic template interface for add") {
        pre.clear();
        pre.add(1., 2., 3.);
        CHECK_EQ(pre.size(), 1);
    }

    SUBCASE("Test matrix interface for add") {
        pre.clear();
        Eigen::Vector3d b(1, 2, 3);
        Eigen::Matrix<double, 3, 2> vectors;
        vectors << a, b;
        pre.add(vectors);
        CHECK(pre.size() == 2);
        decltype(pre) other;
        other.add(a);
        other.add(b);
        CHECK_EQ(other.get_mean(), pre.get_mean());
        CHECK_EQ(other.get_covariance(), pre.get_covariance());
    }
}
