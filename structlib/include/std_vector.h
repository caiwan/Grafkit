/**<LICENSE />*/

#ifndef __STD_VECTOR_WRAPPER_H_
#define __STD_VECTOR_WRAPPER_H_

#include <vector>
#include <allocators>

/**A wrapper that extends std::vector with operator >> and operator <<
*/

// http://www.stroustrup.com/C++11FAQ.html#11

// #define NO_VECTOR_WRAP

#ifndef NO_VECTOR_WRAP
namespace FWutils {

	template <typename T> class vector : public std::vector<T> {
	public:
		vector() : std::vector<T>() {}
		// http://en.cppreference.com/w/cpp/container/vector/vector

		//explicit vector(const Allocator& alloc = Allocator());
		//vector() : vector(Allocator()) {}
		//explicit vector(const Allocator& alloc);
		//explicit vector(size_type count, const T& value = T(), const Allocator& alloc = Allocator());
		//explicit vector(size_type count, const T& value, const Allocator& alloc = Allocator());
		// 
		explicit vector(size_type count) : std::vector<T>(count) {}
		//explicit vector(size_type count, const std::Allocator& alloc = std::Allocator()) : std::vector<T>(count, alloc) {}
		//template< class InputIt > vector(InputIt first, InputIt last, const Allocator& alloc = Allocator());
		//
		vector(const std::vector<T>& other) : std::vector<T>(other) {}
		//vector(const vector& other, const std::Allocator& alloc) : std::vector<T>(other, alloc){};
		vector(std::vector<T>&& other) : std::vector<T>(other) {}
		//vector(vector&& other, const std::Allocator& alloc) : std::vector<T>(other, alloc) {}
		//vector(std::initializer_list<T> init, const std::Allocator& alloc = std::Allocator()) : std::vector<T>(init, alloc) {}

	public:
		vector<T> operator<< (const T& other) { this->push_back(other); return *this; }
	};

}
#else 

#error "can't opt out vector wrapping"

#endif 


#endif //__STD_VECTOR_WRAPPER_H_ 
