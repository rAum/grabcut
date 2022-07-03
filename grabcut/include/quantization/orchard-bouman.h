#ifndef GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
#define GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace grabcut {
struct Shape;
}  // namespace grabcut

struct QuantizationModel;

namespace quantization {

void quantize(const std::uint8_t* data, const grabcut::Shape& shape, const std::uint8_t* mask, QuantizationModel& result);

} // namespace quantization

#endif //GRABCUT_QUANTIZATION_ORCHARD_BOUMAN_H
