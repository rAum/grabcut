#include <doctest.h>
#include <string>
#include <stb_image.h>
#include <stb_image_write.h>
#include <memory>

#include <grabcut/grabcut.h>

#include <cstdio>

auto data_dir = std::string(TESTDATA_DIR);

std::unique_ptr<std::uint8_t[]> get_flower_rect_mask(int width, int height) {
    std::unique_ptr<std::uint8_t[]> mask(new std::uint8_t[width * height]);
    memset(mask.get(), 0, width * height * sizeof(std::uint8_t));
    // { 68, 29, 533, 494};
    for (int i = 29; i < 494+29 ; ++i) {
        for (int j = 68; j < 68+533; ++j) {
            mask[i * width + j] = 1;
        }
    }
    return mask;
}

TEST_CASE("Test grabcut implementation") {
    auto handle = fopen((data_dir + "/flower1.jpg").c_str(), "rb");
    int width, height, channels;
    // RGB order, continous image
    std::uint8_t* image = stbi_load_from_file(handle, &width, &height, &channels, 3);
    auto img = std::unique_ptr<std::uint8_t, decltype(&stbi_image_free)>(image, stbi_image_free);
    CHECK_EQ(width, 814);
    CHECK_EQ(height, 547);
    CHECK_EQ(channels, 3);

    grabcut::Grabcut grabcut;
    auto mask = get_flower_rect_mask(width, height);

    SUBCASE("Init grabcut") {
        grabcut.init(img.get(), mask.get(), width, height, channels);
        auto rgba = grabcut.get_result();
        CHECK_EQ(rgba.size(), width * height * 4);
        stbi_write_png("grab_0.png", width, height, 4, rgba.data(), width * 4);
        std::vector<std::uint8_t> res_mask = grabcut.get_mask();
        CHECK_EQ(res_mask.size(), width * height);
        for (auto& m : res_mask) m *= 255;
        stbi_write_png("grab_postinit_mask.png", width, height, 1, res_mask.data(), width);

        SUBCASE("Run 1 step") {
            grabcut.run();
            rgba = grabcut.get_result();
            CHECK_EQ(rgba.size(), width * height * 4);
            stbi_write_png("grab_1.png", width, height, 4, rgba.data(), width * 4);
            res_mask = grabcut.get_component_map();
            CHECK_EQ(res_mask.size(), width * height);
            auto v = *std::max_element(res_mask.begin(), res_mask.end());
            CHECK_LE(v, 5);
            for (auto& m : res_mask) m *= 255 / 5;
            stbi_write_png("comp_1.png", width, height, 1, res_mask.data(), width);
        }

        SUBCASE("Run 2 step") {
            grabcut.run(2);
            rgba = grabcut.get_result();
            CHECK_EQ(rgba.size(), width * height * 4);
            stbi_write_png("grab_2.png", width, height, 4, rgba.data(), width * 4);
            res_mask = grabcut.get_component_map();
            CHECK_EQ(res_mask.size(), width * height);
            auto v = *std::max_element(res_mask.begin(), res_mask.end());
            CHECK_LE(v, 5);
            for (auto& m : res_mask) m *= 255 / 5;
            stbi_write_png("comp_2.png", width, height, 1, res_mask.data(), width);
        }

        constexpr bool run_more = false;
        if (run_more) {
        SUBCASE("Run 5 steps") {
            grabcut.run(5);
            rgba = grabcut.get_result();
            CHECK_EQ(rgba.size(), width * height * 4);
            stbi_write_png("grab_5.png", width, height, 4, rgba.data(), width * 4);
            res_mask = grabcut.get_component_map();
            CHECK_EQ(res_mask.size(), width * height);
            auto v = *std::max_element(res_mask.begin(), res_mask.end());
            CHECK_LE(v, 5);
            for (auto& m : res_mask) m *= 255 / 5;
            stbi_write_png("comp_5.png", width, height, 1, res_mask.data(), width);
        }

        SUBCASE("Run 10 steps") {
            grabcut.run(10);
            rgba = grabcut.get_result();
            CHECK_EQ(rgba.size(), width * height * 4);
            stbi_write_png("grab_10.png", width, height, 4, rgba.data(), width * 4);
            res_mask = grabcut.get_component_map();
            CHECK_EQ(res_mask.size(), width * height);
            auto v = *std::max_element(res_mask.begin(), res_mask.end());
            CHECK_LE(v, 5);
            for (auto& m : res_mask) m *= 255 / 5;
            stbi_write_png("comp_10.png", width, height, 1, res_mask.data(), width);
        }

        } // run more
    }
}