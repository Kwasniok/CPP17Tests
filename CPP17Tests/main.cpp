
#include <iostream>
#include <algorithm>

using namespace std;

class empty_stack_error: public std::runtime_error
{
public:
	explicit empty_stack_error(const std::string& what_arg) : std::runtime_error(what_arg) { }
	explicit empty_stack_error(const char* what_arg) : std::runtime_error(what_arg) { }
};

template<typename T>
class Stack
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	Stack() : mem_(new T[capacity_]) { }
	Stack(const Stack& rhs) : mem_(new_copy(rhs.begin(), rhs.end(), rhs.size())), size_(rhs.size()), capacity_(rhs.size()) { }
	Stack& operator=(const Stack& rhs);
	~Stack() noexcept { delete[] mem_; }
public:
	void push(const T& v);
	T pop() throw(empty_stack_error);
	inline bool valid() const noexcept { return mem_; }
	inline size_t size() const noexcept { return size_; }
	inline size_t capacity() const noexcept { return capacity_; }
	inline bool empty() const noexcept { return size_ == 0; }
	inline iterator begin() noexcept { return mem_; }
	inline iterator end() noexcept { return mem_ ? mem_ + size_ : nullptr; }
	inline const_iterator begin() const noexcept { return mem_; }
	inline const_iterator end() const noexcept { return mem_ ? mem_ + size_ : nullptr; }
private:
	inline T* new_copy(const_iterator src_begin, const_iterator src_end, const size_t capacity);
	inline void extend(const size_t);
private:
	T* mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = 10;
};

template<typename T>
Stack<T>& Stack<T>::operator=(const Stack& rhs)
{
	if (this != &rhs)
	{
		// try allocating new memory and try copying data to it
		T* tmp = new_copy(rhs.begin(), rhs.end(), rhs.size());
		// copy was successful: changes can now be applied to the program
		// use nothrow fuctions from this point on!
		delete[] mem_; // does/should not throw!
		mem_ = tmp;
		size_ = rhs.size_;
		capacity_ = rhs.size();
	}
	return *this;
}

template<typename T>
void Stack<T>::push(const T& v)
{
	if (size() == capacity()) extend(10);
	T tmp = v;
	swap(tmp, *end());
	++size_;
}

template<typename T>
T Stack<T>::pop() throw(empty_stack_error)
{
	if (empty()) throw empty_stack_error("empty_stack_error: cannot pop from empty Stack"); // upgrade!!
	iterator it = end();
	--it;
	T tmp = *it;
	it->~T();
	--size_;
	return tmp; // move?
}

template<typename T>
inline T* Stack<T>::new_copy(const_iterator src_begin, const_iterator src_end, const size_t capacity) {
	// Tries to allocate memory and tries to copy all elemens.
	// Behaves exception-neutral & -safe.
	// If this function returns, the returned pointer holds a successfull copy and
	// the ownership of the pointer is passed on.

	// try allocation a new block of memory
	T* tmp = new T[capacity]();
	// try copying
	try {
		copy(src_begin, src_end, tmp);
	} catch (...) {
		// failure: clean up!
		delete[] tmp;
		// behave exception-neutral
		throw;
	}
	// success: hand over pointer & ownership
	return tmp;
}
template<typename T>
void Stack<T>::extend(const size_t n)
{

	// try allocating new memory and try copying data to it
	T* tmp;
	if (capacity_ == 0) {
		tmp = new T[n];
	} else {
		tmp = new_copy(this->begin(), this->end(), this->capacity() + n);
	}
	// copy was successful: changes can now be applied to the program
	// use nothrow fuctions from this point on!
	delete[] mem_; // does/should not throw!
	mem_ = tmp;
	capacity_ += n;;
}


int main() {
	Stack<int> s;
	Stack<int> s2(s);
	s = s2;
}
