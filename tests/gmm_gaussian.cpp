#include <doctest.h>

#include <gmm/gaussian_model.hpp>


TEST_CASE("GMM Gaussian test") {
    gmm::GaussianModel<double, 3> model;

    SUBCASE("Build something") {
        auto density = model.probability_density({1, 2, 3});
        CHECK_EQ(density, 0.);
    }
    CHECK(true);
}