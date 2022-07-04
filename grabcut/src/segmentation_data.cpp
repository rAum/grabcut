#include <grabcut/segmentation_data.h>

namespace grabcut {

void SegmentationData::init_from(Shape _shape, const std::uint8_t *mask) {
    shape = _shape;
    segmap.clear();
    segmap.assign(shape.size(), Trimap::Background);
    trimap.clear();
    trimap.assign(shape.size(), Trimap::Background);

    auto tri = trimap.data();
    auto seg = segmap.data();

    for (auto m=mask; m != mask + shape.size(); ++m, ++tri, ++seg) {
        if (*m) {
            *seg = Trimap::Foreground;
            *tri = Trimap::Unknown;
        }
    }
}

std::vector<std::uint8_t> SegmentationData::make_rgba(const std::uint8_t* color, std::uint8_t novalue, std::uint8_t value) const {
    std::vector<std::uint8_t> result(segmap.size() * 4);
    auto seg = segmap.data();
    for (auto out = result.data(); out != result.data() + result.size(); out += 4) {
        out[0] = *(color++); // red
        out[1] = *(color++); // green
        out[2] = *(color++); // blue
        out[3] = *(seg++)? value : novalue; // alpha
    }
    return result;
}

}  // namespace grabcut