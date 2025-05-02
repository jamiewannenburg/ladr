#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <string>
#include <cstdlib>

extern "C" {
    #include "../../../ladr/strbuf.h"
}

namespace py = pybind11;

namespace pybind11 { namespace detail {
    template <>
    struct type_caster<String_buf> {
    public:
        PYBIND11_TYPE_CASTER(String_buf, _("io.BytesIO"));

        // Python io.BytesIO -> C String_buf
        bool load(py::handle src, bool convert) {
            py::module io = py::module::import("io");
            if (!py::isinstance(src, io.attr("BytesIO")))
                return false;

            py::object val = src.attr("getvalue")();
            if (!py::isinstance<py::bytes>(val))
                return false;

            std::string data = val.cast<std::string>();
            value = get_string_buf();
            for (unsigned char c : data)
                sb_append_char(value, c);
            return true;
        }

        // C String_buf -> Python io.BytesIO
        static py::handle cast(String_buf src, py::return_value_policy policy, py::handle parent) {
            int size = sb_size(src);
            char *buf = sb_to_malloc_char_array(src);
            if (!buf)
                return py::none().release();

            py::bytes pyb(buf, size);
            free(buf);

            py::module io = py::module::import("io");
            py::object bio = io.attr("BytesIO")(pyb);
            return bio.release();
        }
    };
}} // namespace pybind11::detail 