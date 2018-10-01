
#include <iostream>
#include <algorithm>

using namespace std;

class empty_stack_error: public std::runtime_error
{
public:
	explicit empty_stack_error(const std::string& what_arg) : std::runtime_error(what_arg) { }
	explicit empty_stack_error(const char* what_arg) : std::runtime_error(what_arg) { }
};

template <typename T>
class StackImpl
{
public:
	static constexpr size_t default_capacity = 8;

	StackImpl(size_t capacity = default_capacity);
	StackImpl( const StackImpl& ) = delete;
	~StackImpl();
	StackImpl& operator=( const StackImpl& ) = delete;
	void swap(StackImpl& rhs) noexcept;
	T*     mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_ = default_capacity;
};

template<typename T>
StackImpl<T>::StackImpl(size_t capacity) : capacity_(capacity), mem_(capacity ? reinterpret_cast<T*>(new char[sizeof(T)*capacity]) : nullptr) { }
template<typename T>
StackImpl<T>::~StackImpl()
{
	// destructs all used elements and deallocates the memory
	for (size_t i = 0; i < size_; ++i, ++mem_) {
		delete mem_;
	}
}
template<typename T>
void StackImpl<T>::swap(StackImpl<T>& rhs) noexcept
{
	// swaps the representations
	std::swap(mem_, rhs.mem_);
	std::swap(size_, rhs.size_);
	std::swap(capacity_, rhs.capacity_);
}

template<typename T>
class Stack
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	Stack() : mem_(new T[capacity_]) { }
	Stack(const size_t n) : mem_(new T[n]), capacity_(n) { }
	Stack(const Stack& rhs) : mem_(new_copy(rhs.begin(), rhs.end(), rhs.size())), size_(rhs.size()), capacity_(rhs.size()) { }
	Stack& operator=(const Stack& rhs);
	~Stack() noexcept { delete[] mem_; }
public:
	void push(const T& v);
	T& top() const throw(empty_stack_error);
	void pop() throw(empty_stack_error);
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
public:
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
	mem_[size_] = v;
	++size_;
}

template<typename T>
T& Stack<T>::top() const throw(empty_stack_error)
{
	// give a reference to the last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot top from empty Stack");
	// return reference
	return mem_[size_ - 1];
}
template<typename T>
void Stack<T>::pop() throw(empty_stack_error)
{
	// remove last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot pop from empty Stack");
	// release resources of original (nothrow)
	mem_[size_ - 1].~T();
	// update stack state
	--size_;
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
	T* tmp = new_copy(this->begin(), this->end(), this->capacity() + n);
	// copy was successful: changes can now be applied to the program
	// use nothrow fuctions from this point on!
	delete[] mem_; // does/should not throw!
	mem_ = tmp;
	capacity_ += n;;
}

template<typename T>
inline void print_stack(const Stack<T>& s, const char* name)
{
	cout << name << " = {";
	for (auto it : s) { cout << it << ", "; }
	cout << "}" << endl;
}

int main()
{
	// short tests for StackImpl
	{
		StackImpl<int> si1(3);
		StackImpl<int> si2(2);
		si1.mem_[0] = 1;
		si2.mem_[0] = 2;
		si1.swap(si2);
	}

	// short tests for STack
	/*/
	Stack<int> s;
	Stack<int> s2(s);
	s = s2;
	print_stack(s, "s");
	cout << "s.push(1)" << endl;
	s.push(1);
	cout << "s.top() = " << s.top() << endl;
	print_stack(s, "s");
	cout << "s.pop()" << endl;
	s.pop();
	print_stack(s, "s");
	cout << "s.pop()" << endl;
	try {
		s.pop();
	} catch (empty_stack_error e) {
		cout << e.what() << endl;
	}
	cout << "s.top() = " << endl;
	try {
		s.top();
	} catch (empty_stack_error e) {
		cout << e.what() << endl;
	}
	*/
}
