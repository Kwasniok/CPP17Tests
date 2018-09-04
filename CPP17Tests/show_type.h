#pragma once

template<typename T>
class type_displayer;

// Use this function to get a compiler error containing information about the type of x.
#define show_type(x) type_displayer<decltype(x)>{}
