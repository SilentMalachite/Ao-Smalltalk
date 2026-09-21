#pragma once
#include "ao/Oop.hpp"
#include "ao/WellKnown.hpp"
#include <string_view>
namespace ao {
namespace Symbol {
Oop intern(WellKnown& wk, std::string_view utf8);
std::string_view bytes(const Heap& heap, Oop sym);
}
}
