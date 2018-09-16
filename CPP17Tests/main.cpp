
#include <iostream>

template<typename T>
class Stack {
public:
	Stack() {
		T* tmp = nullptr;
		try {
			tmp = new T[capacity_];
		} catch (std::bad_alloc e) {
			capacity_ = 0;
			throw;
		}
		if (tmp) {
			mem_ = tmp;
		}
	};
	~Stack() {
		delete[] mem_;
		mem_ = nullptr;
	}

private:
	T* mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = 10;
};

int main() {
	Stack<int> s;
}
