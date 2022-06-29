#ifndef GRABCUT_QUANTIZATION_ORCHARD_BAUMAN_H
#define GRABCUT_QUANTIZATION_ORCHARD_BAUMAN_H
#pragma once

#include <cstdint>
#include <array>
#include <stdexcept>

struct Shape {
    int width, height, channels;
    int stride;
};

namespace quantization {

void quantize(const std::uint8_t* data, const Shape& shape);

} // namespace quantization

#endif //GRABCUT_QUANTIZATION_ORCHARD_BAUMAN_H
