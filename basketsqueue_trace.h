#pragma once

#include <atomic>
#include <iostream>
#include "htm_cas.h"


#define MAX_HOPS (3)
typedef unsigned int u_int;

template <typename T>
class BasketsQueue
{
public:

	// pure virtual functions
	virtual T dequeue(u_int tid) = 0;

	virtual bool enqueue(T value, u_int tid) = 0;

	virtual T sum() = 0;

	virtual size_t size() = 0;

};

template <typename T>
struct node_t_htm;

template <typename T>
class pointer_t_htm
{
public:
	pointer_t_htm() = default;

	pointer_t_htm(node_t_htm<T> * p_node, bool deleted = false, unsigned int tag = 0) :
		pointer(p_node),
		deleted(deleted),
		tag(tag) {};

	bool operator==(const pointer_t_htm& other) const
	{
		return pointer == other.pointer;
	}
	bool operator !=(const pointer_t_htm& other) const
	{
		return !(*this == other);
	}

	node_t_htm<T>* pointer = nullptr;
	bool deleted = false;
	unsigned int tag = 0;
};

template <typename T>
struct node_t_htm
{
	node_t_htm() {};
	node_t_htm(T value) : value(value) {};
	T value;
	pointer_t_htm<T> next;
};

template <typename T>
class BasketsQueueHTM : public BasketsQueue<T>
{
public:
	BasketsQueueHTM(u_int* failures_counters) : m_failures_counters(failures_counters)
	{
		auto node = new node_t_htm<T>();
		b_tail = pointer_t_htm<T>(node);
		b_head = pointer_t_htm<T>(node);
	}

	size_t size()
	{
		size_t number_of_elements = 0;
		auto node = b_head.pointer->next;
		while (nullptr != node.pointer)
		{
			if (false == node.deleted)
			{
				++number_of_elements;
			}
			node = node.pointer->next;
		}
		return number_of_elements;
	}

	// Guaranteed to be called without concurrent operations
	T sum()
	{
		T sum = T();
		pointer_t_htm<T> iter = b_head.pointer->next;
		while (iter.pointer != nullptr)
		{
			if (!iter.deleted)
			{
				sum += iter.pointer->value;
			}
			iter = iter.pointer->next;

		}
		return sum;

	}
	
	void free_chain(pointer_t_htm<T> head, pointer_t_htm<T> new_head, u_int tid)
	{
		if (htm_compare_and_swap<pointer_t_htm<T>>(&b_head, &head, pointer_t_htm<T>(new_head.pointer, 0, head.tag + 1)))
		{
			while (head.pointer != new_head.pointer)
			{
				pointer_t_htm<T> next = head.pointer->next;
				delete head.pointer;
				head = next.pointer;

			}
		}
		else
		{
			m_failures_counters[tid]++;
		}
	}

	T dequeue(u_int tid)
	{
		while (true)
		{
			pointer_t_htm<T> head = b_head;
			pointer_t_htm<T> tail = b_tail;
			pointer_t_htm<T> next = head.pointer->next;

			if (head == b_head)
			{
				if (head.pointer == tail.pointer)
				{
					// empty queue. return 0
					if (next.pointer == nullptr)
					{
						return T();
					}
					// if the queue is not empty but the tail wasn't updated yet.
					while ((next.pointer->next.pointer != nullptr) && (b_tail == tail))
					{
						next = next.pointer->next;
					}
					htm_compare_and_swap<pointer_t_htm<T>>(&b_tail, &tail, pointer_t_htm<T>(next.pointer, false, tail.tag + 1));
				}
				else
				{
					int hops = 0;
					pointer_t_htm<T> iter = head;
					while ((b_head == head) && (next.deleted && iter.pointer != tail.pointer))
					{
						iter = next;

						next = iter.pointer->next;
						hops++;

					}
					if (b_head != head)
					{
						continue;
					}
					else if (iter.pointer == tail.pointer)
					{
						free_chain(head, iter, tid);
					}
					else
					{
						T value = next.pointer->value;
						if (htm_compare_and_swap<pointer_t_htm<T>>(
							&iter.pointer->next, &next, pointer_t_htm<T>(next.pointer, true, tail.tag + 1)))
						{
							if (hops >= MAX_HOPS)
							{
								free_chain(head, next, tid);
							}
							return value;
						}
						else
						{
							m_failures_counters[tid]++;
						}

						// backoff scheme
					}
				}
			}
		}
	}

	bool enqueue(T value, u_int tid)
	{
		node_t_htm<T>* new_node = new node_t_htm<T>(value);

		while (true)
		{
			pointer_t_htm<T> tail = this->b_tail;
			pointer_t_htm<T> next = tail.pointer->next;

			if (tail == b_tail)
			{
				if (nullptr == next.pointer)
				{
					new_node->next = pointer_t_htm<T>(nullptr, false, tail.tag + 2);
					if (htm_compare_and_swap<pointer_t_htm<T>>(
						&tail.pointer->next, &next, pointer_t_htm<T>(new_node, false, tail.tag + 1)))
					{
						htm_compare_and_swap<pointer_t_htm<T>>(
							&b_tail, &tail, pointer_t_htm<T>(new_node, false, tail.tag + 1));
						return true;
					}
					else
					{
						m_failures_counters[tid]++;
					}
					next = tail.pointer->next;
					while ((next.tag == tail.tag + 1) && (false == next.deleted))
					{
						// backoff_scheme() 
						new_node->next = next;
						if (htm_compare_and_swap<pointer_t_htm<T>>(
							&tail.pointer->next, &next, pointer_t_htm<T>(new_node, false, tail.tag + 1)))
						{
							return true;
						}
						else
						{
							m_failures_counters[tid]++;
						}
						next = tail.pointer->next;
					}
				}
				else
				{
					while ((nullptr != next.pointer->next.pointer) && (tail == b_tail))
						next = next.pointer->next;

					htm_compare_and_swap<pointer_t_htm<T>>(
						&b_tail, &tail, pointer_t_htm<T>(next.pointer, false, tail.tag + 1));
				}
			}
		}
	}


private:
	pointer_t_htm<T> b_tail;
	pointer_t_htm<T> b_head;
	u_int* m_failures_counters;
};


template <typename T>
struct node_t_std;

template <typename T>
class alignas(16) pointer_t_std
{
public:
	pointer_t_std() = default;

	pointer_t_std(node_t_std<T>* p_node, bool deleted = false, unsigned int tag = 0) :
		pointer(p_node),
		deleted(deleted),
		tag(tag) {};

	bool operator==(const pointer_t_std& other) const
	{
		return pointer == other.pointer;
	}
	bool operator !=(const pointer_t_std& other) const
	{
		return !(*this == other);
	}

	node_t_std<T>* pointer = nullptr;
	bool deleted = false;
	unsigned int tag = 0;
};

template <typename T>
using atomic_pointer_t_std = std::atomic<pointer_t_std<T>>;

template <typename T>
struct alignas(16) node_t_std
{
	node_t_std() {};
	node_t_std(T value) : value(value) {};
	T value;
	atomic_pointer_t_std<T> next;
};

template <typename T>
class BasketsQueueSTD : public BasketsQueue<T>
{
public:
	BasketsQueueSTD(u_int* failures_counters) : m_failures_counters(failures_counters)
	{
		auto node = new node_t_std<T>();
		b_tail = pointer_t_std<T>(node);
		b_head = pointer_t_std<T>(node);
	}

	// Guaranteed to be called without concurrent operations
	T sum()
	{
		T sum = T();
		pointer_t_std<T> iter = b_head.load().pointer->next.load();
		while (iter.pointer != nullptr)
		{
			if (!iter.deleted)
			{
				sum += iter.pointer->value;
			}
			iter = iter.pointer->next.load();

		}
		return sum;
	}

	size_t size()
	{
		size_t number_of_elements = 0;
		auto node = b_head.load().pointer->next.load();
		while (nullptr != node.pointer)
		{
			if (false == node.deleted)
			{
				++number_of_elements;
			}
			node = node.pointer->next.load();
		}
		return number_of_elements;
	}


	void free_chain(pointer_t_std<T> head, pointer_t_std<T> new_head, u_int tid)
	{
		if (b_head.compare_exchange_strong(head, pointer_t_std<T>(new_head.pointer, 0, head.tag + 1)))
		{
			while (head.pointer != new_head.pointer)
			{
				pointer_t_std<T> next = head.pointer->next.load();
				delete head.pointer;
				head = next.pointer;

			}
		}
		else
		{
			m_failures_counters[tid]++;
		}
	}

	T dequeue(u_int tid)
	{
		while (true)
		{
			pointer_t_std<T> head = b_head.load();
			pointer_t_std<T> tail = b_tail.load();
			pointer_t_std<T> next = head.pointer->next.load();

			if (head == b_head.load())
			{
				if (head.pointer == tail.pointer)
				{
					// empty queue. return 0
					if (next.pointer == nullptr)
					{
						return T();
					}
					// if the queue is not empty but the tail wasn't updated yet.
					while ((next.pointer->next.load().pointer != nullptr) && (b_tail.load() == tail))
					{
						next = next.pointer->next.load();
					}

					b_tail.compare_exchange_strong(tail, pointer_t_std<T>(next.pointer, false, tail.tag + 1));
				}
				else
				{
					int hops = 0;
					pointer_t_std<T> iter = head;
					while ((b_head.load() == head) && (next.deleted && iter.pointer != tail.pointer))
					{
						iter = next;

						next = iter.pointer->next.load();
						hops++;

					}
					if (b_head.load() != head)
					{
						continue;
					}
					else if (iter.pointer == tail.pointer)
					{
						free_chain(head, iter, tid);
					}
					else
					{
						T value = next.pointer->value;

						if (iter.pointer->next.compare_exchange_strong(next,
							pointer_t_std<T>(next.pointer, true, tail.tag + 1)))
						{
							if (hops >= MAX_HOPS)
							{
								free_chain(head, next, tid);
							}
							return value;
						}
						else
						{
							m_failures_counters[tid]++;
						}

						// backoff scheme
					}
				}
			}
		}
	}

	bool enqueue(T value, u_int tid)
	{
		node_t_std<T>* new_node = new node_t_std<T>(value);

		while (true)
		{
			pointer_t_std<T> tail = this->b_tail.load();
			pointer_t_std<T> next = tail.pointer->next.load();

			if (tail == b_tail.load())
			{
				if (nullptr == next.pointer)
				{
					new_node->next = pointer_t_std<T>(nullptr, false, tail.tag + 2);
					if (tail.pointer->next.compare_exchange_strong(next, pointer_t_std<T>(new_node, false, tail.tag + 1)))
					{
						b_tail.compare_exchange_strong(tail, pointer_t_std<T>(new_node, false, tail.tag + 1));
						return true;
					}
					else
					{
						m_failures_counters[tid]++;
					}
					next = tail.pointer->next.load();
					while ((next.tag == tail.tag + 1) && (false == next.deleted))
					{
						// backoff_scheme() 
						new_node->next = next;
						if (tail.pointer->next.compare_exchange_strong(next, pointer_t_std<T>(new_node, false, tail.tag + 1)))
							return true;
						else
							m_failures_counters[tid]++;
						next = tail.pointer->next.load();
					}
				}
				else
				{
					while ((nullptr != next.pointer->next.load().pointer) && (tail == b_tail.load()))
						next = next.pointer->next;
					b_tail.compare_exchange_strong(tail, pointer_t_std<T>(next.pointer, false, tail.tag + 1));
				}
			}
		}
	}

private:
	atomic_pointer_t_std<T> b_tail;
	atomic_pointer_t_std<T> b_head;


	u_int* m_failures_counters;
};
