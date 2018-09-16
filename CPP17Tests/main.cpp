
#include <iostream>
#include <algorithm>

using namespace std;

template<typename T>
class Stack {

	typedef T* iterator;
	typedef const T* const_iterator;

public:
	Stack() : mem_(new T[capacity_]) { };
	Stack(const Stack& rhs) : mem_(new T[rhs.size_]()) , size_(rhs.size_), capacity_(rhs.capacity_) {
		try {
			copy(rhs.begin(), rhs.end(), mem_);
		} catch (...) {
			delete[] mem_;
			throw;
		}
	}
	~Stack() noexcept {
		delete[] mem_;
	}

	Stack& operator=(const Stack& rhs) {
		T* tmp = new T[rhs.size_]();

		try {
			copy(rhs.begin(), rhs.end(), tmp);
		} catch (...) {
			delete[] tmp;
			throw;
		}

		delete[] mem_;
		mem_ = tmp;
		size_ = rhs.size_;
		capacity_ = rhs.size_;
		return *this;
	}

public:
	inline bool valid() noexcept { return mem_; }

	iterator begin() noexcept {
		return mem_;
	}
	iterator end() noexcept {
		return mem_ ? mem_ + size_ : nullptr;
	}
	const_iterator begin() const noexcept {
		return mem_;
	}
	const_iterator end() const noexcept {
		return mem_ ? mem_ + size_ : nullptr;
	}

private:
	T* mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = 10;
};

int main() {
	Stack<int> s;
	Stack<int> s2(s);
	s = s2;
}
