#pragma once

template<typename T>
class type_displayer;

// Use this function to get a compiler error containing information about the type of x.
template<typename T>
constexpr auto show_type(T x) { return type_displayer<decltype(x)>{}; }
