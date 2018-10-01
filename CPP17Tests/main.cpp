
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

template <typename T>
class StackImpl
{
protected:
	StackImpl(size_t capacity);
	StackImpl( const StackImpl& ) = delete;
	~StackImpl();
	StackImpl& operator=( const StackImpl& ) = delete;
	void extend(const size_t n);
	void swap(StackImpl& rhs) noexcept;
	bool valid() const noexcept { return mem_; }
protected:
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
template<typename T>
void StackImpl<T>::extend(const size_t n)
{
	// try allocation a new block of memory
	T* tmp = static_cast<T*> (operator new(sizeof(T)*(capacity_ + n)));
	// try copying
	try {
		copy(mem_, mem_+size_, tmp);
	} catch (...) {
		// failure: clean up!
		operator delete(tmp);
		// behave exception-neutral
		throw;
	}
	// copy was successful: changes can now be applied to the program
	// use nothrow fuctions from this point on!
	// destruct all old used elemetnts & deallocate the old memory
	//if(mem_!=nullptr) { ... <-- possible optimization
	destroy(mem_, mem_+size_); // nullptr-safe
	operator delete (mem_); // nullptr-safe if std implementation
	// apply changes
	mem_ = tmp;
	capacity_ += n;;
}

class empty_stack_error: public std::runtime_error
{
public:
	explicit empty_stack_error(const std::string& what_arg) : std::runtime_error(what_arg) { }
	explicit empty_stack_error(const char* what_arg) : std::runtime_error(what_arg) { }
};

// NOTE: The usage of 'this->' is necessary for the gcc complier (see: https://gcc.gnu.org/onlinedocs/gcc/Name-lookup.html)
template<typename T>
class Stack : private StackImpl<T>
{
public:
	static constexpr size_t default_capacity_chunk_size = 8;
public:
	typedef T* iterator;
	typedef const T* const_iterator;
public:
	Stack(size_t capacity = default_capacity_chunk_size) : StackImpl<T>(capacity) { }
	Stack(const Stack& rhs);
	Stack& operator=(const Stack& rhs);
	~Stack() noexcept { }
public:
	void push(const T& v);
	T& top() const throw(empty_stack_error);
	void pop() throw(empty_stack_error);
	inline size_t size() const noexcept { return this->size_; }
	inline size_t capacity() const noexcept { return this->capacity_; }
	inline bool empty() const noexcept { return this->size_ == 0; }
	inline iterator begin() noexcept { return this->mem_; }
	inline iterator end() noexcept { return this->mem_ ? this->mem_ + this->size_ : nullptr; }
	inline const_iterator begin() const noexcept { return this->mem_; }
	inline const_iterator end() const noexcept { return this->mem_ ? this->mem_ + this->size_ : nullptr; }
};

template<typename T>
Stack<T>::Stack(const Stack& rhs) : StackImpl<T>(rhs.size_)
{
	// if the super constructor failes the destructor of the base class is called and hence the memory is deallocated
	// try copying
	copy(rhs.begin(), rhs.end(), this->mem_);
}
template<typename T>
Stack<T>& Stack<T>::operator=(const Stack& rhs)
{
	if (this != &rhs)
	{
		// create a copy of rhs
		Stack<T> tmp(rhs);
		// success!
		this->swap(tmp); // noexcept
	}
	return *this;
}

template<typename T>
void Stack<T>::push(const T& v)
{
	// appends the new element at the end
	// extend memory if needed
	if (size() == capacity()) this->extend(default_capacity_chunk_size);
	// since the memory slot ist not initialized the new element is constructed in place
	new(this->mem_ + this->size_) T(v);
	++this->size_;
}

template<typename T>
T& Stack<T>::top() const throw(empty_stack_error)
{
	// give a reference to the last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot top from empty Stack");
	// return reference
	return this->mem_[this->size_ - 1];
}
template<typename T>
void Stack<T>::pop() throw(empty_stack_error)
{
	// remove last element
	// an empty stack throws an empty_stack_error
	if (empty()) throw empty_stack_error("empty_stack_error: cannot pop from empty Stack");
	// release resources of original (nothrow)
	this->mem_[this->size_ - 1].~T();
	// update stack state
	--this->size_;
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
}
