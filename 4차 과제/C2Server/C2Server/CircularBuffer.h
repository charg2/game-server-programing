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

	// 현재 사용 중인 버퍼의 크기.
	size_t get_use_size(void)
	{
		if (front >= rear)
			return front - rear;
		else
			return Capacity - rear + front;
	}

	// 남은 버퍼의 크기/
	size_t get_free_size(void)		// 현재 버퍼에 남은 용량 얻기. 
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

	// 주의 사항 세부 크기 +1 -1 아직 안봄.
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

		// 공간이 나눠져 있는지? // 넘어서는 크키가 있는지? 
		size_t tempDirectEnqueueSize = tempRear > tempFront ? tempRear - tempFront - 1 : Capacity - tempFront;
		size_t sizeToStore = size > freeSize ? freeSize : size;
		size_t sizeOfCurrentSpace = tempDirectEnqueueSize < sizeToStore ? tempDirectEnqueueSize : sizeToStore;
		size_t sizeOfNextSpace = sizeToStore - sizeOfCurrentSpace;

		// 넣을수잇는 만큼만 넣고
		memcpy(&buffer[tempFront], data, sizeOfCurrentSpace);

		// 다음공간 크기 를 넣음.
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
	size_t front; // front가 가리키는 공간은 당므에 데이터 넣을 곳.
	size_t rear;

	char* buffer;
	//char* bufferEnd;
};


#endif