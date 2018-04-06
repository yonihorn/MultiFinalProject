#include "basketsqueue.h"
#include <future>
#include <iostream>

BasketsQueue<int> g_queue;

void thread_callback(int number_of_enqueues)
{
	for (int i = 0; i < number_of_enqueues; i++)
	{
		g_queue.enqueue(i);
	}
}



int main()
{
	int number_of_threads = 4;
	int number_of_enqueues_per_thread = 10000;

	std::vector<std::future<void>> futures;
	for (int i = 0; i < number_of_threads; i++)
	{
		futures.emplace_back(std::async(thread_callback, number_of_enqueues_per_thread));
	}
	
	for (int i = 0; i < futures.size(); i++)
	{
		futures[i].wait();
	}
	futures.clear();

	auto size = g_queue.size();
	if (size != number_of_threads * number_of_enqueues_per_thread)
	{
		std::cout << "Invalid number of elements on the queue " << size <<
		" instead of " << number_of_threads * number_of_enqueues_per_thread << std::endl;

		return 1;
	}
	else
	{
		std::cout << "The queue seems valid! " << std::endl;
		return 0;
	}
}