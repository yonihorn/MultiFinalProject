// Implemenation of the MS Queue algorithm, using HTM-implemented CAS operation

// T must be default-constructable  and movable?

#include "htm_cas.h"
#include <atomic>


template<typename T>
class MSQueue {
public:

    class QueueNode;
    struct CountedPtr;

    virtual void enqueue(T value) = 0;
    virtual bool dequeue(T* outValue) = 0;
};

template<typename T>
class HTM_CAS_MSQueue : MSQueue<T> {
public:

    class QueueNode;
    struct CountedPtr {
        QueueNode* ptr;
        //unsigned int count;
        CountedPtr() : ptr(nullptr) {};

        bool operator==(const CountedPtr& rhs) { return ptr == rhs.ptr;}
    };

    class QueueNode {
    public:
        T value;
        CountedPtr next;
        bool sentinel;
        QueueNode(T value_) : value(value_), sentinel(false){};
        QueueNode() : sentinel(true) {};
    };


    HTM_CAS_MSQueue() {
        QueueNode* newNode = new QueueNode(); // create sentinel node
        m_head.ptr = newNode;
        m_tail.ptr = newNode;
    }

    virtual void enqueue(T value) override {
        QueueNode* newNode = new QueueNode(value);
        CountedPtr newTail;
        newTail.ptr = newNode;
        CountedPtr tail;
        while (true) {
            tail = m_tail;
            CountedPtr next = tail.ptr->next;
            if (tail == m_tail) {
                if (next.ptr == nullptr) {
                    if (htm_compare_and_swap<CountedPtr>(&tail.ptr->next, &next, newTail)) {
                        break;
                    }
                } else {
                    CountedPtr swingTail;
                    swingTail.ptr = next.ptr;
                    htm_compare_and_swap<CountedPtr>(&m_tail, &tail, swingTail);
                }
            }
        }
        CountedPtr swingTail;
        swingTail.ptr = newNode;
        htm_compare_and_swap<CountedPtr>(&m_tail, &tail, swingTail);
    }

    virtual bool dequeue(T* outValue) {
        while (true) {
            CountedPtr head = m_head;
            CountedPtr tail = m_tail;
            CountedPtr next = head.ptr->next;
            if (head == m_head) {
                if (head.ptr == tail.ptr) {
                    // queue is either empty or tail is falling behind
                    if (next.ptr == nullptr) {
                        // queue is empty
                        return false;
                    } else {
                        // tail is falling behind, try to swing it
                        CountedPtr swingTail;
                        swingTail.ptr = next.ptr;
                        htm_compare_and_swap<CountedPtr>(&m_tail, &tail, swingTail);
                    }
                } else {
                    // queue is not empty and tail is fine
                    // read the value
                    *outValue = next.ptr->value;
                    CountedPtr newHead;
                    newHead.ptr = next.ptr;
                    if (htm_compare_and_swap<CountedPtr>(&m_head, &head, newHead)) {
                        // great success
                        break;
                    }
                }
            }
        }
        //free(head.ptr); // freeing here is not really safe?
        return true;
    }

private:
    CountedPtr m_head;
    CountedPtr m_tail;

};


template<typename T>
class STD_CAS_MSQueue : MSQueue<T> {
public:

    class QueueNode;
    using QueueNodeAtomicPtr = std::atomic<QueueNode*>;

    struct CountedPtr {
        QueueNode* ptr;
        //unsigned int count;
        CountedPtr() : ptr(nullptr) {};

        bool operator==(const CountedPtr& rhs) { return ptr == rhs.ptr;}
    };

    class QueueNode {
    public:
        T value;
        QueueNodeAtomicPtr next;
        bool sentinel;
        QueueNode(T value_) : value(value_), sentinel(false){};
        QueueNode() : sentinel(true) {};
    };


    STD_CAS_MSQueue() {
        QueueNode* newNode = new QueueNode(); // create sentinel node
        m_head.store(newNode);
        m_tail.store(newNode);
    }

    virtual void enqueue(T value) override {
        QueueNode* newNode = new QueueNode(value);
        QueueNode* currentTail;
        while (true) {
            currentTail = m_tail.load();
            QueueNode* next;
            next = currentTail->next.load();
            if (currentTail == m_tail.load()) {
                if (next == nullptr) {
                    if (std::atomic_compare_exchange_strong(&currentTail->next, &next, newNode)) {
                        break;
                    }
                } else {
                    std::atomic_compare_exchange_strong(&m_tail, &currentTail, next);
                }
            }
        }
        std::atomic_compare_exchange_strong(&m_tail, &currentTail, newNode);
    }

    virtual bool dequeue(T* outValue) {
        while (true) {
            QueueNode* currentHead = m_head.load();
            QueueNode* currentTail = m_tail.load();
            QueueNode* next = currentHead->next.load();
            if (currentHead == m_head) {
                if (currentHead == currentTail) {
                    // queue is either empty or tail is falling behind
                    if (next == nullptr) {
                        // queue is empty
                        return false;
                    } else {
                        // tail is falling behind, try to swing it
                        std::atomic_compare_exchange_strong(&m_tail, &currentTail, next);
                    }
                } else {
                    // queue is not empty and tail is fine
                    // read the value
                    *outValue = next->value;
                    if (std::atomic_compare_exchange_strong(&m_head, &currentHead, next)) {
                        // great success
                        break;
                    }
                }
            }
        }
        //free(head.ptr); // freeing here is not really safe?
        return true;
    }

private:
    QueueNodeAtomicPtr m_head;
    QueueNodeAtomicPtr m_tail;

};

