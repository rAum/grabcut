#ifndef GRABCUT_QUANTIZATION_MODEL_HPP
#define GRABCUT_QUANTIZATION_MODEL_HPP

#include <gmm/gaussian_mixture_model.hpp>

struct QuantizationModel {
    std::array<gmm::GaussianMixtureModel<double, 3>, 2> gmm;
};

#endif //GRABCUT_QUANTIZATION_MODEL_HPP
