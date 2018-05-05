#pragma once

#include <iostream>
#include "htm_cas.h"
#include "basketsqueue.h"

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
	BasketsQueueHTM()
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
	void free_chain(pointer_t_htm<T> head, pointer_t_htm<T> new_head)
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
	}

	T dequeue()
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
						std::cout << "empty!" << std::endl;
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
						free_chain(head, iter);
					}
					else
					{
						T value = next.pointer->value;
						if (htm_compare_and_swap<pointer_t_htm<T>>(
							&iter.pointer->next, &next, pointer_t_htm<T>(next.pointer, true, tail.tag + 1)))
						{
							if (hops >= MAX_HOPS)
							{
								free_chain(head, next);
							}
							return value;
						}

						// backoff scheme
					}
				}
			}
		}
	}

	bool enqueue(T value)
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
};
