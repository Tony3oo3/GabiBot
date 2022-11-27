
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "gabibbot.cpp"

namespace py = pybind11;

PYBIND11_MODULE(gabibbot, handle){
    handle.doc() = 
        "GabibBot module."
        "The objective of this module is to compute the"
        "best move given a Tablut board.";

    py::enum_<Player>(handle, "Player")
            .value("Black", Black)
            .value("White", White)
            .export_values();

    py::enum_<Piece>(handle, "Piece")
            .value("Empty_P", Empty_P)
            .value("White_P", White_P)
            .value("Black_P", Black_P)
            .value("King", King)
            .export_values();

    py::class_<State>(handle, "State")
        .def("print_board", &State::print_board, py::arg("padding")="  ")
        .def("get_player", &State::get_player);

    py::class_<Move>(handle, "Move")
        .def("get_to", &Move::get_to)
        .def("get_from", &Move::get_from);

    py::class_<GabibBot>(handle, "GabibBot")
            .def(py::init())
            .def("min_max_iterative_d", &GabibBot::min_max_iterative_d);

    handle.def("create_state_from_str", &create_state_from_str);

}
