#include "basketsqueue.h"
#include <future>
#include <iostream>

#define VERIFY_ELSE_RETURN(expected, actual, variable_name) if (actual != expected) { std::cout << variable_name << " should be " << expected << " but instead equals to " << actual << std::endl; return 1; }

void thread_callback(BasketsQueue<int>* queue, int number_of_enqueues)
{
	for (int i = 0; i < number_of_enqueues; i++)
	{
		queue->enqueue(i);
	}
}

int basic_enqueue_dequeue_test()
{
	BasketsQueue<int> queue;

	queue.enqueue(1);
	queue.enqueue(2);
	queue.enqueue(3);
	queue.enqueue(4);
	queue.enqueue(5);

	VERIFY_ELSE_RETURN(15, queue.sum(), "sum");

	VERIFY_ELSE_RETURN(1, queue.dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(2, queue.dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(3, queue.dequeue(), "dequeue");

	VERIFY_ELSE_RETURN(9, queue.sum(), "sum");


	VERIFY_ELSE_RETURN(4, queue.dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(5, queue.dequeue(), "dequeue");

	queue.enqueue(6);
	queue.enqueue(7);

	VERIFY_ELSE_RETURN(13, queue.sum(), "sum");

	VERIFY_ELSE_RETURN(6, queue.dequeue(), "dequeue");
	VERIFY_ELSE_RETURN(7, queue.dequeue(), "dequeue");

	return 0;
}
int async_test()
{
	BasketsQueue<int> queue;

	int number_of_threads = 4;
	int number_of_enqueues_per_thread = 10000;

	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(thread_callback, &queue, number_of_enqueues_per_thread));
	}

	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();

	VERIFY_ELSE_RETURN(number_of_threads * number_of_enqueues_per_thread, queue.size(), "size");
	VERIFY_ELSE_RETURN(number_of_threads * (number_of_enqueues_per_thread - 1) * (number_of_enqueues_per_thread / 2), queue.sum(), "sum");

	return 0;
}
int main()
{
	if (0 != basic_enqueue_dequeue_test() ||
		0 != async_test())
	{
		std::cout << "Some tests failed " << std::endl;
		return 1;
	}
	std::cout << "Tests succeeded" << std::endl;
	return 0;
	
}