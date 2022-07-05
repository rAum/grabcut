#pragma once
#ifndef GRABCUT_LEARN_GMM_HPP
#define GRABCUT_LEARN_GMM_HPP

#include <cstdint>

struct QuantizationModel;

namespace grabcut {
    struct SegmentationData;
}

namespace gmm {

void learn(QuantizationModel& model, const grabcut::SegmentationData& segdata, const std::uint8_t* colors) noexcept;

}  // namespace gmm

#endif //GRABCUT_LEARN_GMM_HPP
