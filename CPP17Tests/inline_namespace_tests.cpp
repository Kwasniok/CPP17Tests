#include "inline_namespace_tests.h";

void inline_namespace_tests() {

	// calls default (NEW)
	f();
	// calls explicitly new variant
	NEW::f();
	// calls explicitly old variant
	OLD::f();
}
