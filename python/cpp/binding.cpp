#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
//#include <pybind11/eigen.h>
#include <grabcut/grabcut.h>
#include <iostream>

namespace py = pybind11;

namespace {

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
    for (const auto &x : v) {
        os << '[' << x << ']';
    }
    return os;
}

std::vector<std::uint8_t> get_rect_mask(int width, int height, int x, int y, int x_width, int y_height) {
    std::vector<std::uint8_t> mask(width * height, 255);
//    for (int j = x; j < x+x_width; ++j) {
//        for (int i = y; i < y+y_height ; ++i) {
//            mask[i * width + j] = 255;
//        }
//    }
    return mask;
}

std::array<int, 4> get_rect_area(const py::list selection_roi) {
    if (selection_roi.size() != 4) {
        throw py::value_error("selection_roi must be exactly for elements");
    }
    std::array<int, 4> roi;
    auto out = roi.data();
    for (auto n : selection_roi) {
        *(out++) = n.cast<int>();
    }
    return roi;
}

} // namespace

PYBIND11_MODULE(pygrabcut, m) {
    m.doc() = R"pbdoc(
        PyGrabcut
        -----------------------
        .. currentmodule:: pygrabcut
        .. autosummary::
           :toctree: _generate
           Grabcut
           run_grabcut
    )pbdoc";

//    m.def("run_grabcut", [](const py::buffer& input, py::list selection_roi, int iterations) {
//        py::buffer_info b = input.request();
//        if (b.ndim != 3) {
//            throw std::runtime_error("Expected HWC image shape (Height Width Channels=3)");
//        }
//        int width = b.shape[0], height = b.shape[1];
//        std::cout << b.shape << std::endl;
//        auto roi = get_rect_area(selection_roi);
//        auto mask = get_rect_mask(width, height, roi[0], roi[1], roi[2], roi[3]);
//        //grabcut::Grabcut gb;
//        //gb.init((std::uint8_t*)b.ptr, mask.get(), width, height);
//        // gb.run(iterations);
//        return mask;//gb.get_result();
//    });

    using NumpyImage = py::array_t<std::uint8_t, py::array::c_style | py::array::forcecast>;
    m.def("foo", [](NumpyImage img) {
        for (int i = 0; i < 3; ++i) {
            std::cout <<  img.shape(i) << " ";
            std::cout << img.strides(i) << " stride ";
        }
        return  img;
    });

    m.def("make_mask", [](const NumpyImage & input, const py::list& selection_roi) {
        py::buffer_info b = input.request();
        if (b.ndim != 3) {
            throw std::runtime_error("Expected HWC image shape (Height Width Channels=3)");
        }
        int width = b.shape[0], height = b.shape[1];
        std::cout << b.shape << std::endl;
        std::cout << "Got image " << width << "x" << height << "x" << b.shape[2] << std::endl;
        std::cout << "Image size=" << input.size() << std::endl;
        auto roi = get_rect_area(selection_roi);
        return get_rect_mask(height, width, roi[0], roi[1], roi[2], roi[3]);
    });

    py::class_<grabcut::Grabcut>(m, "Grabcut")
            .def(py::init<>())
//            .def("init", [](grabcut::Grabcut& self, const py::buffer& input, py::list selection_roi) {
//                py::buffer_info b = input.request();
//                if (b.ndim != 3) {
//                    throw std::runtime_error("Expected HWC image shape (Height Width Channels=3)");
//                }
//                int width = b.shape[1], height = b.shape[0];
//                std::cout << b.shape << std::endl;
//                std::array<int, 4> roi;
//                std::unique_ptr<std::uint8_t[]> mask;
//                {
//                    py::gil_scoped_release nogil;
//                    roi = get_rect_area(selection_roi);
//                    mask = get_rect_mask(width, height, roi[0], roi[1], roi[2], roi[3]);
//                    self.init((std::uint8_t*)b.ptr, mask.get(), width, height);
//                }
//            })
            .def("run", [](grabcut::Grabcut& self, int iterations) {
                self.run(iterations);
            }, py::arg("iterations") = 3)
            .def("get_result", [](const grabcut::Grabcut& self) {
                return self.get_mask();
            })
            .def("__repr__",
                 [](const grabcut::Grabcut &a) {
                     return "<pygrabcut.Grabcut'>";
                 }
            );

    m.def("run_grabcut", []() {
        py::gil_scoped_release nogil;
        return true;
    }, R"pbdoc(
Runs a grabcut algorithm over given image.
)pbdoc");

    m.attr("__version__") = GRABCUT_VERSION;
}