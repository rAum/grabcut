#pragma once
#ifndef GRABCUT_TRIMAP_HPP
#define GRABCUT_TRIMAP_HPP

namespace grabcut {

enum Trimap : std::uint8_t {
    Background,
    Foreground,
    Unknown,
};

}  // namespace grabcut

#endif //GRABCUT_TRIMAP_HPP
