
#include <iostream>

template<typename T>
class Stack {
public:
	Stack() : mem_(new T[capacity_]) { };
	~Stack() {
		delete[] mem_;
	}

private:
	T* mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = 10;
};

int main() {
	Stack<int> s;
}
