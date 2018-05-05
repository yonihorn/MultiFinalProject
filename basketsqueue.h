#pragma once

#include <atomic>
#include <iostream>
#define MAX_HOPS (3)

template <typename T>
struct node_t;

template <typename T>
class pointer_t
{
public:
	pointer_t() = default;

	pointer_t(node_t<T>* p_node, bool deleted = false, unsigned int tag = 0) :
		pointer(p_node),
		deleted(deleted),
		tag(tag) {};

	bool operator==(const pointer_t& other) const
	{
		return pointer == other.pointer;
	}
	bool operator !=(const pointer_t& other) const
	{
		return !(*this == other);
	}

	node_t<T>* pointer = nullptr;
	bool deleted = false;
	unsigned int tag = 0;
};


template <typename T>
class BasketsQueue
{
public:

	// pure virtual functions
	virtual void free_chain(pointer_t<T> head, pointer_t<T> new_head) = 0;

	virtual T dequeue() = 0;

	virtual bool enqueue(T value) = 0;

	virtual T sum() = 0;

	virtual size_t size() = 0;

};
