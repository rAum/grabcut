#pragma once
#ifndef GRABCUT_FG_BG_GRAPHCUT_HPP
#define GRABCUT_FG_BG_GRAPHCUT_HPP

#include "export_macros.h"

struct QuantizationModel;

namespace grabcut {

struct SegmentationData;

/***
 * It uses maxflow - mincut to segment image
 */
class GRABCUT_EXPORT FgBgGraphCut {
public:

    FgBgGraphCut();
    ~FgBgGraphCut();

    void build_graph(Shape shape, const std::uint8_t* imgdata);

    void update_sink_source(const QuantizationModel& color_model, const std::uint8_t* imgdata, const SegmentationData& segdata);

    bool run(SegmentationData& segdata);

    void estimate_beta(Shape shape, const std::uint8_t* imgdata) noexcept;

    void precompute_edge_weights(Shape shape, const std::uint8_t* imgdata);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace grabcut

#endif //GRABCUT_FG_BG_GRAPHCUT_HPP
