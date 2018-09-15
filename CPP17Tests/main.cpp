
#include <iostream>
#include <cassert>
#include <string>


// GENERAL TEMPLATES

template <class IT>
constexpr char to_std_repr(char c) {
	// converts characters to thier standard representation
	// e.g. for case intensitivity 'A' and 'a' are mapped to 'a' etc. (lowercase ist standard representation)
	// default/fallback is identity
	return c;
}

template<class IT>
struct insensitive_char_traits: public std::char_traits<char> {
	// holds character traits for insensitive _string class template
	static constexpr bool compare(const char * lhs, const char * rhs, size_t n) {
		// adapted from: http://www.cplusplus.com/reference/string/char_traits/compare/
		// could be optimized
		while (n--) {if (!eq(*lhs,*rhs)) return lt(*lhs,*rhs)?-1:1; ++lhs; ++rhs;}
		return 0;
	}
	static constexpr bool eq(char lhs, char rhs) {
		return to_std_repr<IT>(lhs) == to_std_repr<IT>(rhs);
	}
	static constexpr bool lt(char lhs, char rhs) {
		return to_std_repr<IT>(lhs) < to_std_repr<IT>(rhs);
	}
	static constexpr const char* find(const char* s, int n, char a) {
		while (n-- > 0 && to_std_repr<IT>(*s) != to_std_repr<IT>(a)) { ++s; }
		return n > 0 ? s : nullptr;
	}
};

// define insensitive_string class template
template<class IT>
using insensitive_string = std::basic_string<char, insensitive_char_traits<IT>, std::allocator<char>>;

template <class IT>
constexpr  insensitive_string<IT> to_std_repr(const insensitive_string<IT>& istr) {
	// returns copy of insensitive string in standard represenation
	insensitive_string<IT> tmp = istr;
	for (auto& c : tmp) {
		c = to_std_repr<IT>(c);
	}
	return tmp;
}

// I/O function templates for insensitive_class template
template<class charT, class traits, class IT>
inline std::basic_istream<charT, traits>&
operator>>(std::basic_istream<charT, traits>& is, insensitive_string<IT>& str)
{
	std::string s;
	is >> s;
	str = std::move(insensitive_string<IT>(s.c_str()));
	return is;
}
template<class charT, class traits, class IT>
inline std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const insensitive_string<IT>& str)
{
	return os << str.c_str();
	// alternative:
	//return os << to_std_repr(str).c_str();
}


// TEMPLATE INSTANCIATION

// list of supported types
namespace intensitivity {
	// These are the classes to distinguish the type of intensitivity.
	// They lack any declaration since they are template parameters only.
	class NONE;
	class CASE;
	class DIGIT;
	// extend here ...
}
// instantiations of mappings to standard representation
// (only this template needs to be instantiated)
template<>
constexpr char to_std_repr<intensitivity::CASE>(char c) {
	// override for insensitivity of cases
	// converts A-Z to a-z (assuming ASCII)
	// A := 0x41, Z := 0x5A
	// a := 0x61, z := 0x7A
	if (0x41 <= c && c <= 0x5A) return c + 0x20;
	return c;
}
template<>
constexpr char to_std_repr<intensitivity::DIGIT>(const char c) {
	// override for insensitivity of digits
	// converts 0-9 to 0 (assuming ASCII)
	// 0 := 0x30, 9 := 0x39
	if (0x30 <= c && c <= 0x39) return 0x30;
	return c;
}

// ALIAS definitions of insensitive string classes
using ci_string = insensitive_string<intensitivity::CASE>;
using di_string = insensitive_string<intensitivity::DIGIT>;

int main() {
	{
		ci_string s("AbCdE");
		assert(s == "abcde");
		assert(s == "ABCDE");
		assert(strcmp(s.c_str(), "AbCdE") == 0);
		assert(strcmp(s.c_str(), "abcde") != 0);
	}
	{
		di_string s("Hello 12");
		assert(s == "Hello 89");
		assert(s == "Hello 26");
		assert(strcmp(s.c_str(), "Hello 12") == 0);
		assert(strcmp(s.c_str(), "Hello 00") != 0);
	}
}
