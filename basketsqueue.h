#include <atomic>

template <typename T>
struct node_t;

template <typename T>
struct poitner_t
{
    poitner_t(node_t<T>* p_node = nullptr, bool deleted = false, uint tag = 0 ) : 
    pointer(p_node),
    deleted(deleted),
    tag(tag) {}

    node_t<T>* pointer;
    bool deleted;
    uint tag;
}

template <typename T>
struct node_t
{
    T value;
    poitner_t next;
}

template <typename T>
class BasketsQueue
{
    public:
        BasketsQueue()
        {

        }
    
    private:
        poitner_t tail;
        poitner_t head;
    
}