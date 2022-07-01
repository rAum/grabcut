#ifndef GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
#define GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

struct Shape {
    int width, height, channels;
    int stride;
};

namespace quantization {

std::vector<std::uint8_t> quantize(const std::uint8_t* data, const Shape& shape, const std::uint8_t* mask);

} // namespace quantization

#endif //GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
