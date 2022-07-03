#include <grabcut/segmentation_data.h>

namespace grabcut {

void SegmentationData::init_from(Shape _shape, std::uint8_t *mask) {
    shape = _shape;
    segmap.clear();
    trimap.clear();
    segmap.resize(shape.size(), Trimap::Background);
    trimap.resize(shape.size(), Trimap::Unknown);

    auto tri = trimap.data();
    auto seg = segmap.data();

    for (auto m=mask; m != mask + shape.size(); ++m, ++tri, ++seg) {
        if (*m) {
            *tri = Trimap::Unknown;
            *seg = Trimap::Foreground;
        }
    }
}

std::vector<std::uint8_t> SegmentationData::make_rgba(std::uint8_t* color, std::uint8_t novalue, std::uint8_t value) const {
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