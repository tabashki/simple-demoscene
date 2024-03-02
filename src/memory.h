#pragma once

#include "utility.h"

#include <cstdlib>
#include <type_traits>

// Wrappers to avoid using operator new/delete

template<typename T>
T* mem_alloc()
{
	void* alloc = malloc(sizeof(T));
	ASSERT(alloc);

	return reinterpret_cast<T*>(alloc);
}

template<typename T, typename ...Args>
T* mem_new(Args&& ...args)
{
	T* alloc = mem_alloc<T>();
	new (alloc) T(std::forward<Args>(args)...);

	return alloc;
}

template<typename T>
void mem_delete(T* ptr)
{
	if constexpr (!std::is_trivially_destructible_v<T>)
	{
		ASSERT(ptr);
		ptr->~T();
	}
	free(ptr);
}
