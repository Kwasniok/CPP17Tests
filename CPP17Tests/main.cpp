
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

template <typename T>
class StackImpl
{
public:
	StackImpl(size_t capacity);
	StackImpl( const StackImpl& ) = delete;
	~StackImpl();
	StackImpl& operator=( const StackImpl& ) = delete;
	void swap(StackImpl& rhs) noexcept;
	bool valid() const noexcept { return mem_; }
public:
	T*     mem_ = nullptr;
	size_t size_ = 0;
	size_t capacity_;
};

template<typename T>
StackImpl<T>::StackImpl(size_t capacity) : capacity_(capacity), mem_(capacity ? static_cast<T*>(operator new(sizeof(T)*capacity)) : nullptr)
{
	// allocates memory but leaves it uninitialized
}
template<typename T>
StackImpl<T>::~StackImpl()
{
	// destructs all used elements and deallocates the memory
	// check if the memory is used
	// destruct all used elemetnts
	//if(mem_==nullptr) return; <-- possible optimization
	destroy(mem_, mem_ + size_); // nullptr-safe
	// deacllocate used memory
	operator delete(mem_); // nullptr-safe if std implementation
}
template<typename T>
void StackImpl<T>::swap(StackImpl<T>& rhs) noexcept
{
	// swaps the representations
	std::swap(mem_, rhs.mem_);
	std::swap(size_, rhs.size_);
	std::swap(capacity_, rhs.capacity_);
}

class empty_stack_error: public std::runtime_error
{
public:
	explicit empty_stack_error(const std::string& what_arg) : std::runtime_error(what_arg) { }
	explicit empty_stack_error(const char* what_arg) : std::runtime_error(what_arg) { }
};

// NOTE: The usage of 'this->' is necessary for the gcc complier (see: https://gcc.gnu.org/onlinedocs/gcc/Name-lookup.html)
//       Remember: Since the base class is a template it could have ANY definition since template specialisation is possible.
//                 Hence the access to thier attributes and member functions must be explicit. (topic name: two stage lookup)
// NOTE: construct & destruct are deprecated (see: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0174r0.html#2.4)

template<typename T>
class Stack
{
public:
	static constexpr size_t default_initial_capacity = 8;
	static constexpr size_t default_extension_capacity = 8;
public:
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	Stack(size_t capacity = default_initial_capacity) : impl_(capacity) { }
	Stack(const Stack& rhs, size_t extra_capacity=0);
	Stack& operator=(const Stack& rhs);
	~Stack() = default;
public:
	void push(const T& v);
	T& top() const throw(empty_stack_error);
	void pop() throw(empty_stack_error);
	inline size_t size() const noexcept { return this->impl_.size_; }
	inline size_t capacity() const noexcept { return this->impl_.capacity_; }
	inline bool empty() const noexcept { return this->impl_.size_ == 0; }
	inline iterator begin() noexcept { return this->impl_.mem_; }
	inline iterator end() noexcept { return this->impl_.mem_ ? this->impl_.mem_ + this->impl_.size_ : nullptr; }
	inline const_iterator begin() const noexcept { return this->impl_.mem_; }
	inline const_iterator end() const noexcept { return this->impl_.mem_ ? this->impl_.mem_ + this->impl_.size_ : nullptr; }
	inline void swap(Stack<T>& rhs) noexcept { this->impl_.swap(rhs.impl_); }
private:
	StackImpl<T> impl_;
};

template<typename T>
Stack<T>::Stack(const Stack& rhs, size_t extra_capacity) : impl_(rhs.impl_.size_ + extra_capacity)
{
	// if the constructor failes the destructor of the base class is called and hence the memory is deallocated
	// try copying
	try {
		while (this->impl_.size_ < rhs.impl_.size_)
		{
			new(this->impl_.mem_ + this->impl_.size_) T(rhs.impl_.mem_[this->impl_.size_]);
			++this->impl_.size_;
		}
	} catch (...) {
		// if an exception occurs the destructor of the implementation is called and all constructed elements are destroyed
		// and the memory is deallocated
		this->impl_.~StackImpl();
		throw;
	}
}
template<typename T>
Stack<T>& Stack<T>::operator=(const Stack& rhs)
{
	// NOTE: StackImpl has no copy constructor but Stack's copy constructor is essencially the same since it adds no overhead.
	// create a copy of rhs
	Stack<T> tmp(rhs);
	// success: swap representaions
	this->impl_.swap(tmp.impl_); // noexcept
	return *this;
	// tmp (containing the old values) is implicitly destructed
}

template<typename T>
void Stack<T>::push(const T& v)
{
	// appends the new element at the end
	// extend memory if needed and push back the new element
	if (this->impl_.size_ == this->impl_.capacity_)
	{
		// copy this Stack with additional memory
		Stack tmp(*this, default_extension_capacity);
		// append the new item
		// if push fails tmp is deleted implicitly via stack unwinding
		tmp.push(v);
		this->impl_.swap(tmp.impl_);
		// tmp containing the old representation is implicitly destroyed
	} else {
		// no new memory is needed and the new element is appended in uninitialized memory
		// since the memory slot ist not initialized the new element is constructed in place
		new(this->impl_.mem_ + this->impl_.size_) T(v);
		++this->impl_.size_;
	}
}

template<typename T>
T& Stack<T>::top() const throw(empty_stack_error)
{
	// give a reference to the last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot top from empty Stack");
	// return reference
	return this->impl_.mem_[this->impl_.size_ - 1];
}
template<typename T>
void Stack<T>::pop() throw(empty_stack_error)
{
	// remove last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot pop from empty Stack");
	// destroy last element (nothrow)
	--this->impl_.size_;
	this->impl_.mem_[this->impl_.size_].~T();
}

template<typename T>
inline void swap(Stack<T>& lhs, Stack<T>& rhs) noexcept
{
	lhs.swap(rhs);
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
	// short tests for Stack
	{
		Stack<int> s;
		Stack<int> s2(s);
		s = s2;
	}
	{
		Stack<int> s(3);
		Stack<int> s2(5);
		s.push(1);
		s2.push(2);
		swap(s, s2);
	}
	{
		Stack<int> s;
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
	}
}
