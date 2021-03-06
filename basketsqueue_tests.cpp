#include "basketsqueue.h"
#include "basketsqueue_std.h"
#include "basketsqueue_htm.h"
#include <future>
#include <iostream>
#include <vector>

#define VERIFY_ELSE_RETURN(expected, actual, variable_name) if (actual != expected) { std::cout << variable_name << " should be " << expected << " but instead equals to " << actual << std::endl; return 1; }

void enqueue_callback(BasketsQueue<int>* queue, int number_of_enqueues)
{
	for (int i = 0; i < number_of_enqueues; i++)
	{
		queue->enqueue(i);
	}
}

void dequeue_callback(BasketsQueue<int>* queue, int number_of_enqueues)
{
	for (int i = 0; i < number_of_enqueues; i++)
	{
		queue->dequeue();
	}

}


void enqueue_dequeue_callback(BasketsQueue<int>* queue, int number_of_enqueues)
{
	for (int i = 0; i < number_of_enqueues; i++)
	{
		// after each insterion, delete element
		queue->enqueue(i);
		queue->dequeue();
	}
}

/* sanity test for basket queue, one thread */
int basic_enqueue_dequeue_test(BasketsQueue<int> * queue)
{
	std::cout << "basic_enqueue_dequeue_test in progress.." << std::endl;

	queue->enqueue(1);
	queue->enqueue(2);
	queue->enqueue(3);
	queue->enqueue(4);
	queue->enqueue(5);

	VERIFY_ELSE_RETURN(15, queue->sum(), "sum");

	VERIFY_ELSE_RETURN(1, queue->dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(2, queue->dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(3, queue->dequeue(), "dequeue");

	VERIFY_ELSE_RETURN(9, queue->sum(), "sum");


	VERIFY_ELSE_RETURN(4, queue->dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(5, queue->dequeue(), "dequeue");

	queue->enqueue(6);
	queue->enqueue(7);

	VERIFY_ELSE_RETURN(13, queue->sum(), "sum");

	VERIFY_ELSE_RETURN(6, queue->dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(7, queue->dequeue(), "dequeue");
	
	return 0;
}

/* test enqueue operation, using some threads  (verify size of queue and sum of elements) */
int async_test_enqueue(BasketsQueue<int> * queue)
{
	std::cout << "async_test_enqueue in progress.." << std::endl;


	int number_of_threads = 10;
	int number_of_enqueues_per_thread = 50000;

	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(enqueue_callback, queue, number_of_enqueues_per_thread));
	}

	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();

	int expected_sum = number_of_threads * (number_of_enqueues_per_thread - 1) * (number_of_enqueues_per_thread / 2);

	VERIFY_ELSE_RETURN(number_of_threads * number_of_enqueues_per_thread, queue->size(), "size");
	VERIFY_ELSE_RETURN(expected_sum, queue->sum(), "sum");

	return 0;
}

/* test dequeue operation, using some threads. dequeue only half of the elements. verify size and sum of elemenets. */
int async_test_dequeue_half(BasketsQueue<int> * queue)
{
	std::cout << "async_test_dequeue_half in progress.." << std::endl;

	int number_of_elements = 10000;
	int number_of_threads = 10;
	
	// Preparing the queue
	for (int i = 0; i < number_of_threads * number_of_elements; i++)
	{
		queue->enqueue(i);
	}
	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(dequeue_callback, queue, number_of_elements / 2));
	}

	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();
	int expected_size = number_of_elements * number_of_threads / 2; // we dequeue only half
	// sum of an arithmetic progression
	int expected_sum = expected_size * (expected_size + (expected_size * 2 - 1)) / 2;
	VERIFY_ELSE_RETURN(expected_size, queue->size(), "size");
	VERIFY_ELSE_RETURN(expected_sum, queue->sum(), "sum");


	return 0;
}

/* test dequeue operation, using some threads. dequeue all nodes. (verify that the queue is empty in the end) */
int async_test_dequeue_all(BasketsQueue<int> * queue)
{
	std::cout << "async_test_dequeue_all in progress.." << std::endl;

	int number_of_dequeues_per_thread = 5000;
	int number_of_threads = 4;

	// Preparing the queue
	for (int i = 0; i < number_of_threads * number_of_dequeues_per_thread; i++)
	{
		queue->enqueue(i);
	}
	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(dequeue_callback, queue, number_of_dequeues_per_thread));
	}

	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();

	VERIFY_ELSE_RETURN(0, queue->size(), "size");

	return 0;
}

int async_test_enqueue_dequeue(BasketsQueue<int> * queue)
{
	std::cout << "async_test_enqueue_dequeue in progress.." << std::endl;


	int number_of_threads = 4;
	int number_of_enqueues_per_thread = 5000;

	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(enqueue_dequeue_callback, queue, number_of_enqueues_per_thread));
	}

	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();

	VERIFY_ELSE_RETURN(0, queue->size(), "size");

	return 0;
}

int run_tests_for_std_cas()
{
	std::cout << "running std cas tests" << std::endl;

	BasketsQueueSTD<int> q[5];
	if (0 != basic_enqueue_dequeue_test(&q[0]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_enqueue(&q[1]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_dequeue_all(&q[2]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_dequeue_half(&q[3]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_enqueue_dequeue(&q[4]))
	{
		std::cout << "Test failed" << std::endl;
	}
	
	return 0;
}

int run_tests_for_htm_cas()
{
	std::cout << "running htm cas tests" << std::endl;
	BasketsQueueHTM<int> q[5];
	if (0 != basic_enqueue_dequeue_test(&q[0]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_enqueue(&q[1]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_dequeue_all(&q[2]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_dequeue_half(&q[3]))
	{
		std::cout << "Test failed" << std::endl;
	}

	if (0 != async_test_enqueue_dequeue(&q[4]))
	{
		std::cout << "Test failed" << std::endl;
	}
	return 0;
}

int main()
{
	run_tests_for_std_cas();
	run_tests_for_htm_cas();
	std::cout << "all tests finished" << std::endl;
	return 0;

}