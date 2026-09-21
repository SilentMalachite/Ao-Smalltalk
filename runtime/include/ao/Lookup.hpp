#pragma once

#include "ao/Heap.hpp"
#include "ao/Oop.hpp"

namespace ao {

Oop lookup(Heap& heap, Oop startClass, Oop selector);

}  // namespace ao
