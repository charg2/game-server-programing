#ifndef CHARG2_CIRCULAR_BUFFER_H
#define CHARG2_CIRCULAR_BUFFER_H

#include <vcruntime_string.h>
#include <windows.h>
#define OUT

// SPSC Queue

// 1page
constexpr size_t Capacity = 65536;
//template <size_t Capacity = 65536>
class CircularBuffer
{
public:
	CircularBuffer(void)
		: buffer{ new char[Capacity] {} }/*, bufferEnd{ buffer + Capacity - 1 }*/, front{ 0 }, rear{ 0 }//, size{ 0 }
	{
		static_assert(Capacity >= 2, "Capacity must be greater than 2.");
	}

	~CircularBuffer()
	{
		if (buffer != nullptr)
			delete[] buffer;
	}

	// ���� ��� ���� ������ ũ��.
	size_t get_use_size(void)
	{
		if (front >= rear)
			return front - rear;
		else
			return Capacity - rear + front;
	}

	// ���� ������ ũ��/
	size_t get_free_size(void)		// ���� ���ۿ� ���� �뷮 ���. 
	{
		size_t used_size;

		if (front >= rear)
			used_size = front - rear;
		else
			used_size = Capacity - rear + front;

		return Capacity - used_size - 1;
	}

	size_t	direct_enqueue_size(void)
	{
		if (rear > front)
			return rear - front - 1;
		else
			return Capacity - front;
	}
	size_t	direct_dequeue_size(void)
	{
		if (rear > front)
			return Capacity - rear;
		else
			return front - rear;//-1;
	}

	size_t	peek(char* dest, size_t size)
	{
		size_t tempFront = this->front;
		size_t tempRear = this->rear;  

		size_t useSize = tempFront >= tempRear ? tempFront - tempRear : Capacity - tempRear + tempFront;

		if (useSize == 0)
			return 0;

		size_t tempDequeueSize = tempRear > tempFront ? Capacity - tempRear : tempFront - tempRear;
		size_t sizeToPeek = size > useSize ? useSize : size;
		size_t sizeOfCurrentSpace = tempDequeueSize > sizeToPeek ? sizeToPeek : tempDequeueSize;
		size_t sizeOfNextSpace = sizeToPeek - sizeOfCurrentSpace;

		memcpy(dest, &buffer[tempRear], sizeOfCurrentSpace);

		if (sizeOfNextSpace)
			memcpy(dest + sizeOfCurrentSpace, &buffer[0], sizeOfNextSpace);

		return sizeToPeek;
	}

	size_t	dequeue(char* dest, size_t size)
	{
		size_t tempRear = this->rear; 
		size_t tempFront = this->front;

		size_t useSize = tempFront >= tempRear ? tempFront - tempRear : Capacity - tempRear + tempFront;//
		if (useSize == 0)
			return 0;

		size_t tempDequeueSize = tempRear > tempFront ? Capacity - tempRear : tempFront - tempRear;
		size_t sizeToLoad = size > useSize ? useSize : size;
		size_t sizeOfCurrentSpace = tempDequeueSize > sizeToLoad ? sizeToLoad : tempDequeueSize;
		size_t sizeOfNextSpace = sizeToLoad - sizeOfCurrentSpace;


		memcpy(dest, &buffer[tempRear], sizeOfCurrentSpace);

		if (sizeOfNextSpace)
			memcpy(dest + sizeOfCurrentSpace, &buffer[0], sizeOfNextSpace);

		tempRear += sizeToLoad;
		tempRear %= Capacity;

		this->rear = tempRear;

		return sizeToLoad;
	}

	// ���� ���� ���� ũ�� +1 -1 ���� �Ⱥ�.
	size_t	enqueue(char* data, size_t size)
	{
//CRITICAL_SECTION///////////////////////
		size_t tempRear = this->rear;  //
		size_t tempFront = this->front;//
//CRITICAL_SECTION///////////////////////

		size_t useSize = tempFront >= tempRear ? tempFront - tempRear : Capacity - tempRear + tempFront;
		size_t freeSize = Capacity - useSize - 1;
	
		if (freeSize == 0)
			return 0;

		// ������ ������ �ִ���? // �Ѿ�� ũŰ�� �ִ���? 
		size_t tempDirectEnqueueSize = tempRear > tempFront ? tempRear - tempFront - 1 : Capacity - tempFront;
		size_t sizeToStore = size > freeSize ? freeSize : size;
		size_t sizeOfCurrentSpace = tempDirectEnqueueSize < sizeToStore ? tempDirectEnqueueSize : sizeToStore;
		size_t sizeOfNextSpace = sizeToStore - sizeOfCurrentSpace;

		// �������մ� ��ŭ�� �ְ�
		memcpy(&buffer[tempFront], data, sizeOfCurrentSpace);

		// �������� ũ�� �� ����.
		if (sizeOfNextSpace)
			memcpy(&buffer[0], data + sizeOfCurrentSpace, sizeOfNextSpace);

		tempFront += sizeToStore;
		tempFront %= Capacity;

		this->front = tempFront;

		return sizeToStore;
	}

	void clear(void)
	{
		front = rear = 0;
	}

	void move_rear(size_t size)
	{
		this->rear += size;
		this->rear %= Capacity;
	}

	void move_front(size_t size)
	{
		this->front += size;
		this->front %= Capacity;
	}

	char* get_buffer(void) const
	{
		return buffer;
	}

	char* get_rear_buffer(void) const
	{
		return buffer + rear;
	}

	char* get_front_buffer(void) const
	{
		return buffer + front;
	}


	bool empty() const noexcept
	{
		return front == rear;
	}

	constexpr size_t capacity()
	{
		return Capacity;
	}


private:
	size_t front; // front�� ����Ű�� ������ ��ǿ� ������ ���� ��.
	size_t rear;

	char* buffer;
	//char* bufferEnd;
};


#endif