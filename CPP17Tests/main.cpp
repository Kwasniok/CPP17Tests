
#include <iostream>
#include <algorithm>

using namespace std;

template<typename T>
class Stack {

	typedef T* iterator;
	typedef const T* const_iterator;

public:
	Stack() : mem_(new T[capacity_]) { };
	Stack(const Stack& rhs) : mem_(new_copy(rhs)), size_(rhs.size_), capacity_(new_copy_capacity(rhs)) { }
	~Stack() noexcept {
		delete[] mem_;
	}

	Stack& operator=(const Stack& rhs) {

		if (this != &rhs) {
			// try allocating new memory and try copying data to it
			T* tmp = new_copy(rhs);
			// copy was successful: changes can now be applied to the program
			// use nothrow fuctions from this point on!
			delete[] mem_; // does/should not throw!
			mem_ = tmp;
			size_ = rhs.size_;
			capacity_ = new_copy_capacity(rhs);
		}
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
	inline T* new_copy(const Stack& rhs) {
		// Tries to allocate memory and tries to copy all elemens.
		// Behaves exception-neutral & -safe.
		// If this function returns, the returned pointer holds a successfull copy and
		// the ownership of the pointer is passed on.

		// try allocation a new block of memory
		T* tmp = new T[new_copy_capacity(rhs)]();
		// try copying
		try {
			copy(rhs.begin(), rhs.end(), tmp);
		} catch (...) {
			// failure: clean up!
			delete[] tmp;
			// behave exception-neutral
			throw;
		}
		// success: hand over pointer & ownership
		return tmp;
	}

	inline size_t new_copy_capacity(const Stack& rhs) {
		// returns the capacity of memory returend by new_copy if rhs was passed to it
		return rhs.size_;
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
