#include <doctest.h>
#include <iostream>

#include <Eigen/Dense>

#include <gmm/mean_covar_precompute.hpp>

TEST_CASE("Test mean variance precompute") {
    gmm::MeanCovariancePrecompute pre;

    CHECK_EQ(pre.size(), 0);

    SUBCASE("Adding one point give a valid values") {
        Eigen::Vector3d a(1., 1., 1.);
        pre.add(a);
        CHECK_EQ(pre.size(), 1);
        CHECK_EQ(pre.get_mean(), a);
        auto covariance = pre.get_covariance();
        std::cout << "det= " << covariance.determinant() << std::endl;
        CHECK_GT(covariance.determinant(), 0.);
    }

    SUBCASE("Adding two points gives a valid values") {
        Eigen::Vector3d a(1., 1., 1.);
        Eigen::Vector3d b(1., 2., 3.);
        pre.add(a);
        pre.add(b);
        CHECK_EQ(pre.size(), 2);
        auto mean = (a + b) * 0.5;
        CHECK(pre.get_mean().isApprox(mean));
        auto covariance = pre.get_covariance();
        CHECK_GT(covariance.determinant(), 0.);
    }
}