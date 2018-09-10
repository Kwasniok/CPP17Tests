#pragma once

#include <iostream>
#include <boost/type_index.hpp>

template<typename T>
using add_ref = T&;

template<typename T>
using add_refref = T&&;

void double_ref_tests();
