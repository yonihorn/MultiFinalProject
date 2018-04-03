#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <ctime>
#include "msqueue.h"
#include <cstdlib>
#include <vector>


int VALUE_RANGE = 10;

class OperationsList {
    private:
    class Operation {
        public:
        virtual bool performOperation(MSQueue<u_int>* queue);
    };

    class InsertionOperation {
        protected:
        u_int m_value;
        public:
        InsertionOperation(u_int value) : m_value(value) {};
        virtual bool performOperation(MSQueue<u_int>* queue) {
            queue->enqueue(m_value);
            return true;
        }
    };

    class DeletionOperation {
        public:
        virtual bool performOperation(MSQueue<u_int>* queue) {
            u_int outValue;
            return queue->dequeue(&outValue);
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

    bool operateNextOperation(MSQueue<u_int>* queue) {
        return m_operations[m_operations.size() - operationsLeft--]->performOperation(queue);
    };

};


void queueOperationsWorker(MSQueue<u_int>* queue, OperationsList* opsList) {
    while(opsList->operationsLeft) {
        //std::cout << "operating" << std::endl;
        opsList->operateNextOperation(queue);
    }
}

bool run(int nthreads, int ntrials, int noperations, double ratio, bool prefill, bool HTM_CAS) {
    std::vector<std::thread> threads;
    for (u_int i = 0; i<ntrials; i++) {
        MSQueue<u_int>* queue;
        if (HTM_CAS) {
            queue = reinterpret_cast<MSQueue<u_int>*>(new HTM_CAS_MSQueue<u_int>());
        } else {
            queue = reinterpret_cast<MSQueue<u_int>*>(new STD_CAS_MSQueue<u_int>());
        }
        std::vector<OperationsList*> opsLists;
        for (u_int i = 0; i< nthreads; i++) {
            opsLists.push_back(new OperationsList(noperations, ratio));
        }
        for (u_int i = 0; i < nthreads; i++) {
            threads.push_back(std::thread(queueOperationsWorker, queue, opsLists[i]));
        }
    }
    // wait for all threads to finish
    for (auto& t : threads)
    {
        t.join();
    }
};

int main(int argc, char** argv) {
    // Usage - queue_benchmark <nthreads> <ntrials> <nopeations> <ratio>
    int argsNumber = 4;
    if (argc < argsNumber) {
        std::cerr << "expected 4 paramerts" << std::endl;
        return -1;
    }

    u_int nthreads = atoi(argv[1]);
    u_int ntrials = atoi(argv[2]);
    u_int noperations = atoi(argv[3]);
    double ratio = atof(argv[4]);

    auto t1 = std::chrono::high_resolution_clock::now();
    run(nthreads, ntrials, noperations, ratio, true, true);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto microseconds_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "htm cas: " << microseconds_elapsed.count() << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    run(nthreads, ntrials, noperations, ratio, true, false);
    t2 = std::chrono::high_resolution_clock::now();
    microseconds_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    std::cout << "std cas: " << microseconds_elapsed.count() << std::endl;
};