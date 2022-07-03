#pragma once
#ifndef GRABCUT_TRIMAP_HPP
#define GRABCUT_TRIMAP_HPP

namespace grabcut {

enum Trimap : std::uint8_t {
    Background = 0,
    Foreground = 1,
    Unknown = 2,
};

}  // namespace grabcut

#endif //GRABCUT_TRIMAP_HPP
