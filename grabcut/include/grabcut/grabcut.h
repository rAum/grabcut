#pragma once
#ifndef GRABCUT_GRABCUT_H
#define GRABCUT_GRABCUT_H

#include <memory>
#include <vector>

#include "export_macros.h"

namespace grabcut {

class GRABCUT_EXPORT Grabcut {
public:
    Grabcut();
    ~Grabcut();

    Grabcut& init(const std::uint8_t* image, const std::uint8_t* mask, int width, int height, int channels=3);

    Grabcut& run(int steps=1);

    [[nodiscard]]
    const std::vector<std::uint8_t>& get_mask() const;

    [[nodiscard]]
    std::vector<std::uint8_t> get_result() const;

    [[nodiscard]]
    std::vector<std::uint8_t> get_component_map() const;

private:
    struct GbData;
    std::unique_ptr<GbData> impl_;
};

}  // namespace grabcut

#endif //GRABCUT_GRABCUT_H
