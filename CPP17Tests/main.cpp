
#include <iostream>

template<typename T>
class Stack {

	typedef T* iterator;
	typedef const T* const_iterator;

public:
	Stack() : mem_(new T[capacity_]) { };
	Stack(const Stack& rhs) : mem_(new T[rhs.size_]()) , size_(rhs.size_), capacity_(rhs.capacity_) {
		T* i = mem_;
		try {
			for (const_iterator it = rhs.begin(), end = rhs.end(); it != end; ++it, ++i) {
				*i = *it;
			}
		} catch (...) {
			for (iterator e = mem_; i != e; --i) {
				i->~T();
			}
			delete[] mem_;
			throw;
		}
	}
	~Stack() noexcept {
		delete[] mem_;
	}

	Stack& operator=(const Stack& rhs) {
		delete[] mem_;
		mem_ = nullptr;

		T* tmp = new T[rhs.size_]();

		T* i = tmp;
		try {
			for (const_iterator it = rhs.begin(), end = rhs.end(); it != end; ++it, ++i) {
				*i = *it;
			}
		} catch (...) {
			for (iterator e = tmp; i != e; --i) {
				i->~T();
			}
			delete[] tmp;
			throw;
		}

		mem_ = tmp;
		size_ = rhs.size_;
		capacity_ = rhs.capacity_;
		return *this;
	}

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
