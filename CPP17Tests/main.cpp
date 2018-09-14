
#include <iostream>
#include <cassert>
#include <string>

struct ci_char_traits: public std::char_traits<char> {
	static bool compare(const char * const lhs, const char * const rhs, const size_t n) {
		return _memicmp(lhs, rhs, n);
	}
	static bool eq(char lhs, char rhs) {
		return tolower(lhs) == tolower(rhs);
	}
	static bool lt(char lhs, char rhs) {
		return tolower(lhs) < tolower(rhs);
	}
	static const char* find(const char* s, int n, char a) {
		while (n-- > 0 && tolower(*s) != tolower(a)) { ++s; }
		return n > 0 ? s : 0;
	}
};

using ci_string = std::basic_string<char, ci_char_traits, std::allocator<char>>;

int main() {
	ci_string s("AbCdE");
	assert( s == "abcde" );
	assert( s == "ABCDE" );
	assert( strcmp( s.c_str(), "AbCdE" ) == 0 );
	assert( strcmp( s.c_str(), "abcde" ) != 0 );
}
