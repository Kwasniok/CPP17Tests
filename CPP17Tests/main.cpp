
#include <iostream>
#include <algorithm>

template<typename T, size_t size> class fixed_vector {
public:
	typedef T*       iterator;
	typedef const T* const_iterator;
	iterator       begin()       { return v_; }
	iterator       end()         { return v_ + (v_?size:0); }
	const_iterator begin() const { return v_; }
	const_iterator end()   const { return v_ + (v_?size:0); }

	fixed_vector() { }

	fixed_vector(const T (&v)[size]) {
		std::copy(std::begin(v),
				  std::end(v),
				  this->begin());
	}

	template<class RAIter>
	fixed_vector(RAIter beginR, RAIter endR) {
		std::copy(beginR, std:: min(endR, beginR + size) , this->begin());
	}

	template<typename TR, size_t sizeR>
	fixed_vector(const fixed_vector<TR, sizeR> rhs) {
		*this = rhs;
	}

	template<class RAIter>
	fixed_vector& copy(RAIter beginR, RAIter endR) {
		std::copy(beginR, std::min(endR, beginR + size), this->begin());
		return *this;
	}

	template<typename TR, size_t sizeR>
	fixed_vector& operator=(const fixed_vector<TR, sizeR>& rhs) {
		std::copy(rhs.begin(),
				  std:: min(rhs.end(), rhs.begin() + size),
				  this->begin());
		return *this;
	}

private:
	T v_[size];
};

int main() {
	using ivec_t = fixed_vector<int, 3>;
	using cvec_t = fixed_vector<char, 3>;
	using svec_t = fixed_vector<std::string, 3>;

	ivec_t v1({1,2,3});
	ivec_t v2;
	v2 = v1;
	ivec_t v3(v2);
	v3.copy(v1.begin(), v1.end());

	cvec_t c1({1,2,3});
	cvec_t c2;
	c2 = c1;
	cvec_t c3(v2);
	c3.copy(c1.begin(), c1.end());

	svec_t s1({std::string("abc"),std::string("def"),std::string("ghi")});
	svec_t s2;
	s2 = s1;
	svec_t s3(s2);
	s3.copy(s1.begin(), s1.end());

	v1 = c1;
	v3.copy(v1.begin(), v1.end());
	ivec_t v4(c1);
}
