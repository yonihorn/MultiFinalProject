#pragma once

#include <atomic>
#include <iostream>
#define MAX_HOPS (3)



template <typename T>
class BasketsQueue
{
public:

	// pure virtual functions
	virtual T dequeue() = 0;

	virtual bool enqueue(T value) = 0;

	virtual T sum() = 0;

	virtual size_t size() = 0;

};
