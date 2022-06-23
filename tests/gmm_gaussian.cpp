#include <doctest.h>

#include <gmm/gaussian_model.hpp>
#include <gmm/mean_covar_precompute.hpp>
#include <gmm/build_gaussian.hpp>


TEST_CASE("GMM Gaussian test") {
    gmm::GaussianModel<double, 2> model;

    SUBCASE("Returns zero after construction") {
        auto density = model.probability_density({1, 2});
        CHECK_EQ(density, 0.);
    }

    SUBCASE("Build a gaussian model from data") {
        gmm::MeanCovariancePrecompute<double, 2> data;
        data.add(0., 0.);
        data.add({1., 0.});
        data.add({0., 1.});

        gmm::build_gaussian(model, data, data.size() + 2);

        auto mean = data.get_mean();
        auto mean_density = model.probability_density(mean);
        auto other_point = model.probability_density({1., 0.});
        auto outlier_point = model.probability_density({10., 10.});

        CHECK_GT(mean_density, 0.8);
        CHECK_GT(mean_density, other_point);
        CHECK_GT(other_point, outlier_point);
        CHECK_LT(outlier_point, 0.00001);
    }
    CHECK(true);
}