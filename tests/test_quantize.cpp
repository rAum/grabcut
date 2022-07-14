#include <doctest.h>
#include <string>
#include <stb_image.h>
#include <stb_image_write.h>
#include <memory>

#include <cstdio>

#include <quantization/orchard-bouman.h>
#include <quantization/quantization_model.hpp>
#include <grabcut/segmentation_data.h>

auto data_dir = std::string(TESTDATA_DIR);

TEST_CASE("Quantize image") {
    auto handle = fopen((data_dir + "/flower1.jpg").c_str(), "rb");

    grabcut::Shape shape;
    // RGB order, continous image
    std::uint8_t* image = stbi_load_from_file(handle, &shape.width, &shape.height, &shape.channels, 3);
    auto img = std::unique_ptr<std::uint8_t, decltype(&stbi_image_free)>(image, stbi_image_free);

    std::unique_ptr<std::uint8_t[]> mask(new std::uint8_t[shape.width * shape.height * 1]);

    memset(mask.get(), 0, shape.width * shape.height * sizeof(std::uint8_t));
    // { 68, 29, 533, 494};
    for (int i = 29; i < 494+29 ; ++i) {
        for (int j = 68; j < 68+533; ++j) {
            mask[i * shape.width + j] = 1;
        }
    }
    grabcut::SegmentationData data;
    data.init_from(shape, mask.get());

    auto rgba = data.make_rgba(img.get(), 40);
    stbi_write_png("masked_img.png", shape.width, shape.height, 4, rgba.data(), shape.width * 4);

    CHECK_EQ(shape.width, 814);
    CHECK_EQ(shape.height, 547);
    CHECK_EQ(shape.channels, 3);

    QuantizationModel result;
    quantization::quantize(img.get(), shape, mask.get(), result);

    // FIXME: add some kind of verification
}