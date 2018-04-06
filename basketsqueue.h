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
    node_t<T>* pointer = nullptr;
    bool deleted = false;
    unsigned int tag = 0;
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


    void free_chain(pointer_t<T>& head, pointer_t<T> new_head)
    {
        if (std::atomic_compare_exchange_strong(&b_head, &head, pointer_t<T>(new_head.pointer, 0, head.tag + 1)))
        {
            while (!(head.pointer == new_head.pointer))
            {
                pointer_t<T> next = head.pointer->next;
                delete head.pointer;
                head = next;
                
            }
        }
    }

    T dequeue()
    {
        while (true)
        {
            pointer_t<T>& head = b_head.load();
            pointer_t<T>& tail = b_tail.load();
            pointer_t<T>& next = head.pointer->next.load();
            if (tail == b_tail.load())
            {
                if (head.pointer == tail.pointer)
                {
                    if (next.pointer == NULL)
                    {
                        std::cout << "empty!" << std::endl;
                        return NULL;
                    }
                    while (next.pointer->next.load().pointer != NULL && b_tail.load() == tail)
                    {
                        next = next.pointer->next;
                    }
                    std::atomic_compare_exchange_strong(&b_tail, &tail,
                        pointer_t<T>(next.pointer, 0, tail.tag + 1));
                }

                else
                {
                    int hops = 0;
                    pointer_t<T> iter = b_head.load();
                    while (next.deleted && iter.pointer != tail.pointer && b_head.load() == head)
                    {
                        iter = next;
                        next = iter.pointer->next;
                        hops++;
                    }
                    if (!(b_head.load() == head))
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
                        if (std::atomic_compare_exchange_strong(&iter.pointer->next, &next,
                            pointer_t<T>(next.pointer, 1, tail.tag + 1)))
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

    int sum()
    {
        pointer_t<T> iter = b_head.load();
        while (next.deleted && iter.pointer != tail.pointer && b_head.load() == head)
        {
            iter = next;
            next = iter.pointer->next;
            hops++;
        }
    }
    bool Enqueue(T value)
    {
        node_t<T>* new_node = new node_t<T>(value);

        while (true)
        {
            pointer_t<T>& tail = this->b_tail.load();
            pointer_t<T>& next = tail.pointer->next.load();

				if (tail == b_tail)
				{
					if (nullptr == next.pointer)
					{
						new_node->next = pointer_t<T>(nullptr, 0, tail.tag + 2);
						if (tail.pointer->next.compare_exchange_strong(next, pointer_t<T>(new_node, false, tail.tag + 1)))
						{
							b_tail.compare_exchange_strong(tail, pointer_t<T>(new_node, false, tail.tag + 1));
							return true;
						}
						next = tail.pointer->next;
						while ((next.tag == tail.tag + 1) && (false == next.deleted))
						{
							// backoff_scheme() ??? 
							new_node->next = next;
							if (tail.pointer->next.compare_exchange_strong(next, pointer_t<T>(new_node, false, tail.tag + 1)))
								return true;
							next = tail.pointer->next.load();
						}
					}
					else
					{
						while ((nullptr != next.pointer->next.load().pointer) && (tail == b_tail))
							next = next.pointer->next;
						b_tail.compare_exchange_strong(tail, pointer_t<T>(next.pointer, false, tail.tag + 1));
					}
				}
			}
		}

		size_t Size() const
		{
			size_t number_of_elements = 0;
			auto& node = b_head.load();
			while (nullptr != node.pointer->next.load().pointer)
			{
				++number_of_elements;
				node = node.pointer->next.load();
			}
			return number_of_elements;
		}
    
    private:
		atomic_pointer_t<T> b_tail;
		atomic_pointer_t<T> b_head;
};
