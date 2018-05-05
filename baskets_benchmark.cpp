#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <ctime>
#include "basketsqueue.h"
#include "basketsqueue_htm.h"
#include "basketsqueue_std.h"
#include <cstdlib>
#include <vector>


int VALUE_RANGE = 10;

class OperationsList {
    private:
    class Operation {
        public:
        virtual void performOperation(BasketsQueue<u_int>* queue);
    };

    class InsertionOperation {
        protected:
        u_int m_value;
        public:
        InsertionOperation(u_int value) : m_value(value) {};
        virtual void performOperation(BasketsQueue<u_int>* queue) {
            queue->enqueue(m_value);
        }
    };

    class DeletionOperation {
        public:
        virtual void performOperation(BasketsQueue<u_int>* queue) {
            queue->dequeue();
        };
    };

    std::vector<Operation*> m_operations;
    public:
    u_int operationsLeft;
    OperationsList(u_int operationsNumber, float insToDelRatio) {
        std::srand(std::time(nullptr));
        u_int del = operationsNumber / (insToDelRatio + 1);
        u_int ins = operationsNumber - del;
        for (int i = 0; i < del; i++) {
            m_operations.push_back(reinterpret_cast<Operation*>(new DeletionOperation()));
        }
        for (int i = 0; i < ins; i++) {
            int randomValue = rand() % VALUE_RANGE;
            m_operations.push_back(reinterpret_cast<Operation*>(new InsertionOperation(randomValue)));
        }
        std::random_shuffle(m_operations.begin(), m_operations.end());
        operationsLeft = m_operations.size();
    };

    ~OperationsList() {
        for (Operation* op : m_operations) {
            delete op;
        }
    };

    void operateNextOperation(BasketsQueue<u_int>* queue) {
        m_operations[m_operations.size() - operationsLeft--]->performOperation(queue);
    };

};


void queueOperationsWorker(BasketsQueue<u_int>* queue, OperationsList* opsList) {
    while(opsList->operationsLeft) {
        //std::cout << "operating" << std::endl;
        opsList->operateNextOperation(queue);
    }
}

u_int run(int nthreads, int ntrials, int noperations, double ratio, bool prefill, bool HTM_CAS) {
    u_int results_microseconds = 0;
    for (u_int j = 0; j<ntrials; j++) {
        std::vector<std::thread> threads;
        BasketsQueue<u_int>* queue;
        if (HTM_CAS) {
            queue = reinterpret_cast<BasketsQueue<u_int>*>(new BasketsQueueHTM<u_int>());
        } else {
            queue = reinterpret_cast<BasketsQueue<u_int>*>(new BasketsQueueSTD<u_int>());
        }
        std::vector<OperationsList*> opsLists;
        for (u_int i = 0; i< nthreads; i++) {
            opsLists.push_back(new OperationsList(noperations, ratio));
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        for (u_int i = 0; i < nthreads; i++) {
            threads.push_back(std::thread(queueOperationsWorker, queue, opsLists[i]));
        }
        // wait for all threads to finish
        for (auto& t : threads)
        {
            t.join();
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto microseconds_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        results_microseconds += microseconds_elapsed.count();
        
    }
    return results_microseconds / ntrials;
};

int main(int argc, char** argv) {
    // Usage - queue_benchmark <nthreads> <ntrials> <nopeations> <ratio>
    int argsNumber = 4;
    if (argc < argsNumber) {
        std::cerr << "expected 4 paramerts" << std::endl;
        std::cerr << "Usage: queue_benchmark <nthreads> <ntrials> <noperations> <ratio>" << std::endl;
        return -1;
    }

    u_int nthreads = atoi(argv[1]);
    u_int ntrials = atoi(argv[2]);
    u_int noperations = atoi(argv[3]);
    double ratio = atof(argv[4]);

    std::cout << "Running " << noperations << " operations on " << nthreads << " threads, " << ntrials << " trials " << " with ratio " << ratio << std::endl;

    std::cout << "std cas: "  << run(nthreads, ntrials, noperations, ratio, true, false) << std::endl;;
    
    std::cout << "htm cas: "  << run(nthreads, ntrials, noperations, ratio, true, true) << std::endl;;


    return 1;
};
