#include <atomic>

template <typename T>
struct node_t;

template <typename T>
struct pointer_t
{
    pointer_t(node_t<T>* p_node = nullptr, bool deleted = false, unsigned int tag = 0 ) : 
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
        BasketsQueue()
        {
            auto node = new node_t<T>();
            tail = pointer_t<T>(node);
            head = pointer_t<T>(node);
		};
    
    private:
		pointer_t<T> tail;
		pointer_t<T> head;
    
};