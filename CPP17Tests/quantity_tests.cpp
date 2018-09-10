#include "quantity_tests.h"

using namespace MKS;

void quantity_tests() {

	Value x = 1;
	Length l = 2;
	Frequency f = 3;
	Time t = 4;
	Speed v = 5;

	auto a = v + v;
	auto b = v * f;
	auto c = l / t;
}
