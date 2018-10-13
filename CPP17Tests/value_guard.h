
#pragma once

// holds a variable of type T to set_value during the lifetime and restores the original value when destructed (RAII semantics)
// NOTE: The RAII semantics implicitly require a owning relationship of the guarded variable!
//       (I.e. The passed variable MUST BECOME READ-ONLY to the rest of the programm during the life time of this guard!)
template<typename T>
class Value_Guard
{
	T reset_value;
	T& target;
public:
	Value_Guard(T& v, T set_value) : target(v), reset_value(v) { target = set_value; }
	~Value_Guard() { target = reset_value; }
};
