#pragma once

#include <atomic>
#include "basketsqueue.h"
#include <iostream>

template <typename T>
using atomic_pointer_t = std::atomic<pointer_t<T>>;

template <typename T>
struct node_t_std : node_t<T>
{
	node_t_std() {};
	node_t_std(T value) : value(value) {};
	T value;
	atomic_pointer_t<T> next;
};

template <typename T>
class BasketsQueueSTD : public BasketsQueue<T>
{
public:
	BasketsQueueSTD()
	{
		auto node = new node_t<T>();
		b_tail = pointer_t<T>(node);
		b_head = pointer_t<T>(node);
	}

	// Guaranteed to be called without concurrent operations
	T sum()
	{
		T sum = T();
		pointer_t<T>& iter = b_head.load().pointer->next.load();
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
		auto& node = b_head.load().pointer->next.load();
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


	void free_chain(pointer_t<T> head, pointer_t<T> new_head)
	{
		if (b_head.compare_exchange_strong(head, pointer_t<T>(new_head.pointer, 0, head.tag + 1)))
		{
			while (head.pointer != new_head.pointer)
			{
				pointer_t<T> next = head.pointer->next.load();
				delete head.pointer;
				head = next.pointer;

			}
		}
	}

	T dequeue()
	{
		while (true)
		{
			pointer_t<T> head = b_head.load();
			pointer_t<T> tail = b_tail.load();
			pointer_t<T> next = head.pointer->next.load();

			if (head == b_head.load())
			{
				if (head.pointer == tail.pointer)
				{
					// empty queue. return 0
					if (next.pointer == nullptr)
					{
						std::cout << "empty!" << std::endl;
						return NULL;
					}
					// if the queue is not empty but the tail wasn't updated yet.
					while ((next.pointer->next.load().pointer != nullptr) && (b_tail.load() == tail))
					{
						next = next.pointer->next.load();
					}

					b_tail.compare_exchange_strong(tail, pointer_t<T>(next.pointer, false, tail.tag + 1));
				}
				else
				{
					int hops = 0;
					pointer_t<T> iter = head;
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
						free_chain(head, iter);
					}
					else
					{
						T value = next.pointer->value;

						if (iter.pointer->next.compare_exchange_strong(next,
							pointer_t<T>(next.pointer, true, tail.tag + 1)))
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
		node_t<T>* new_node = new node_t<T>(value);

		while (true)
		{
			pointer_t<T> tail = this->b_tail.load();
			pointer_t<T> next = tail.pointer->next.load();

			if (tail == b_tail.load())
			{
				if (nullptr == next.pointer)
				{
					new_node->next = pointer_t<T>(nullptr, false, tail.tag + 2);
					if (tail.pointer->next.compare_exchange_strong(next, pointer_t<T>(new_node, false, tail.tag + 1)))
					{
						b_tail.compare_exchange_strong(tail, pointer_t<T>(new_node, false, tail.tag + 1));
						return true;
					}
					next = tail.pointer->next.load();
					while ((next.tag == tail.tag + 1) && (false == next.deleted))
					{
						// backoff_scheme() 
						new_node->next = next;
						if (tail.pointer->next.compare_exchange_strong(next, pointer_t<T>(new_node, false, tail.tag + 1)))
							return true;
						next = tail.pointer->next.load();
					}
				}
				else
				{
					while ((nullptr != next.pointer->next.load().pointer) && (tail == b_tail.load()))
						next = next.pointer->next;
					b_tail.compare_exchange_strong(tail, pointer_t<T>(next.pointer, false, tail.tag + 1));
				}
			}
		}
	}

private:
	atomic_pointer_t<T> b_tail;
	atomic_pointer_t<T> b_head;
};
