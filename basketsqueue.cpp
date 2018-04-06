#include "basketsqueue.h"

int main()
{
	BasketsQueue<int> queue;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	queue.Enqueue(1);
	queue.Enqueue(2);
	queue.Enqueue(3);
	queue.Enqueue(4);
	queue.Enqueue(5);
	std::cout << "the sum is: " << queue.sum() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the sum is: " << queue.sum() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	queue.Enqueue(6);
	queue.Enqueue(7);
	std::cout << "the sum is: " << queue.sum() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the value is: " << queue.dequeue() << std::endl;
	std::cout << "the sum is: " << queue.sum() << std::endl;


	return 1;
}