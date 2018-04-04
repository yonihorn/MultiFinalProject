#pragma once

#include <atomic>

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

    node_t<T>* pointer;
    bool deleted;
    unsigned int tag;
};

template <typename T>
using atomic_pointer_t = std::atomic<pointer_t<T>>;

template <typename T>
struct node_t
{
	node_t() {};
	node_t(T value) : value(value) {};
	T value;
    atomic_pointer_t<T> next;
};



template <typename T>
class BasketsQueue
{
    public:
        BasketsQueue()
        {
            auto node = new node_t<T>();
            b_tail = pointer_t<T>(node);
            b_head = pointer_t<T>(node);
		}

		bool Enqueue(T value)
		{
			node_t<T>* new_node = new node_t<T>(value);
			
			while (true)
			{
				pointer_t<T> tail = this->b_tail;
				pointer_t<T> next = tail.pointer->next;

				if (tail == b_tail)
				{
					if (nullptr == next.pointer)
					{
						new_node->next = pointer_t<T>(nullptr, 0, tail.tag + 2);
						if (std::atomic_compare_exchange_strong(&tail.pointer->next, &next, pointer_t<T>(new_node, 0, tail.tag + 1)))
						{
							std::atomic_compare_exchange_strong(&b_tail, &tail, pointer_t<T>(new_node, 0, tail.tag + 1));
							return true;
						}
						next = tail.pointer->next;
						while ((next.tag == tail.tag + 1) && (false == next.deleted))
						{
							// backoff_scheme() ??? 
							new_node->next = next;
							if (std::atomic_compare_exchange_strong(&tail.pointer->next, &next, pointer_t<T>(new_node, 0, tail.tag + 1)))
								return true;
							next = tail.pointer->next;
						}
					}
					else
					{
						while ((nullptr != next.pointer->next.pointer) && (tail == b_tail))
							next = next.pointer->next;
						std::atomic_compare_exchange_strong(&b_tail, &tail, poiner_t<T>(next.pointer, 0, tail.tag + 1));
					}
				}
			}
		}
    
    private:
		atomic_pointer_t<T> b_tail;
		atomic_pointer_t<T> b_head;
};

