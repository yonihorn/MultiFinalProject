#include <atomic>
#include <stdlib.h>
template <typename T>
struct node_t;

template <typename T>
struct pointer_t
{
    pointer_t(node_t<T>* p_node = nullptr, bool deleted = false, unsigned int tag = 0) :
        pointer(p_node),
        deleted(deleted),
        tag(tag) {}

    node_t<T>* pointer;
    bool deleted;
    unsigned int tag;
};

template <typename T>
struct node_t
{
    T value;
    pointer_t<T> next;
};


template <typename T>
class BasketsQueue
{
public:
    template<typename T>
    using pointer_t_atomic = std::atomic<pointer_t<T>>;

    BasketsQueue()
    {
        auto node = new node_t<T>();
        b_tail.store(pointer_t<T>(node));
        b_head.store(pointer_t<T>(node));
    }

    T dequeue()
    {
        while (true)
        {
            pointer_t head = b_head.load();
            pointer_t tail = b_tail.load();
            pointer_t next = head.pointer->next.load();
            if (tail == b_tail.load())
            {
                if (head.pointer == tail.pointer)
                {
                    if (next.pointer == NULL)
                    {
                        std::cout << "empty!" << std::endl;
                        break;
                    }
                    while (next.pointer->next.pointer != NULL && b_tail.load() == tail)
                    {
                        next = next.pointer->next;
                    }
                    pointer_t* new_tail = pointer(next.pointer, 0, tail.tag + 1);
                    std::atomic_compare_exchange_strong(&b_tail, &tail, *new_tail);
                }
            }
            else
            {
                int hops = 0;
                pointer_t iter = b_head.load();
                while (next.deleted && iter.pointer != tail.ptr && b_head.load() == head)
                {
                    iter = next;
                    next = iter.pointer->next;
                    hops++;
                }
                if (b_head.load() != head)
                {
                    continue;
                }
                else if (iter.pointer == tail.pointer)
                {
                    // free chain
                }
                else
                {
                    T value = next.pointer->value;
                    pointer_t new_tail = pointer_t(next.pointer, 1, tail.tag + 1);
                    if (std::atomic_compare_exchange_strong(&iter.pointer->next, next, new_tail))
                    {
                        if (hops >= MAX_HOPS)
                        {
                            // free chain
                        }

                        return value;
                    }
                    // backoff scheme
                }

            }
        }
    }

private:
    pointer_t_atomic<T> b_tail;
    pointer_t_atomic<T> b_head;
    
};

void main()
{
    BasketsQueue<int> b;


}