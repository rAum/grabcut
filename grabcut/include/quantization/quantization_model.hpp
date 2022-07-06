#ifndef GRABCUT_QUANTIZATION_MODEL_HPP
#define GRABCUT_QUANTIZATION_MODEL_HPP

#include <gmm/gaussian_mixture_model.hpp>

struct QuantizationModel {
    std::array<gmm::GaussianMixtureModel<double, 3>, 2> gmm;
    std::array<int, 2> strongest_k;
    std::vector<std::uint8_t> component_map;
};

#endif //GRABCUT_QUANTIZATION_MODEL_HPP
