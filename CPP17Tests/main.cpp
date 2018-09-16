
#include <iostream>

template<typename T>
class Stack {
public:
	Stack() { };
	~Stack() {
		delete[] mem_;
		mem_ = nullptr;
	}

private:
	T* mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = 0;
};

int main() {
	Stack<int> s;
}
