#pragma once
#ifndef GRABCUT_SEGMENTATION_DATA_H
#define GRABCUT_SEGMENTATION_DATA_H

#include <cstdint>
#include <vector>

#include "trimap.hpp"

namespace grabcut {

struct Shape {
    int width, height, channels;
    int size() const noexcept { return width * height; }
    int chsize() const noexcept { return size() * channels; }
};

struct SegmentationData {
    Shape shape;
    std::vector<std::uint8_t> segmap;
    std::vector<Trimap> trimap;

    void init_from(Shape shape, const std::uint8_t* mask);

    // for temporary debug
    std::vector<std::uint8_t> make_rgba(const std::uint8_t* color, std::uint8_t novalue = 128, std::uint8_t value=255) const;
};

}

#endif //GRABCUT_SEGMENTATION_DATA_H
