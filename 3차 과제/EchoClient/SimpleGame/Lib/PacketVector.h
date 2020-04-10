#pragma once

#pragma once

#include <stdint.h>
#include <memory>

#include "../../../Common/protocol.h"
#include "../../../Common/exception.h"
//#include "Packet.h"

template <size_t CAPACITY>
class PacketVector
{
public:
	PacketVector() : read_head{}, write_head{}, buffer{}
	{
		buffer = new uint8_t[CAPACITY];
	}
	PacketVector(const PacketVector& other) = delete;
	PacketVector(PacketVector&& other) noexcept = delete;
	~PacketVector()
	{
		delete buffer;
	}

	size_t read(void* dest, size_t size)
	{
		// check
		size_t fit_size;
		size_t explain_size = this->read_head + size;

		if (this->write_head < explain_size)
			fit_size = this->write_head - read_head;
		else
			fit_size = size;

		memcpy(dest, &this->buffer[this->read_head], fit_size);

		this->read_head += fit_size;

		return fit_size;
	}

	size_t write(void* src, size_t size)
	{
		// check
		if (this->write_head + size > CAPACITY)
			c2::util::crash_assert(); // 해킹 공격이 아닌 이상 불가능한 상황.

		memcpy(&this->buffer[this->write_head], src, size);

		this->write_head += size;

		return size;
	}

	size_t peek(void* dest, size_t size)
	{
		// check
		size_t fit_size;
		size_t explain_size = this->read_head + size;

		if (this->write_head < explain_size)
			fit_size = this->write_head - read_head;
		else
			fit_size = size;

		memcpy(dest, &this->buffer[this->read_head], fit_size);

		return fit_size;
	}

	void rewind()
	{
		size_t size = this->write_head - this->read_head;
		// 정리.
		memcpy(this->buffer, &this->buffer[this->read_head], size);

		this->read_head = 0;
		this->write_head = size;
	}

	constexpr size_t capacity() const // total size
	{
		return CAPACITY;
	}

	const size_t get_use_size() const
	{
		return this->write_head - this->read_head;
	}

	const size_t get_free_size() const
	{
		return CAPACITY - this->write_head;
	}

	PacketHeader get_header() const
	{
		return reinterpret_cast<PacketHeader*>(&this->buffer[read_head]);
	}

	const char* get_buffer() const
	{
		return reinterpret_cast<char*>(this->buffer);
	}

	const char* get_read_buffer() const
	{
		return reinterpret_cast<char*>(&this->buffer[this->read_head]);
	}

	const char* get_write_buffer() const
	{
		return reinterpret_cast<char*>(&this->buffer[this->write_head]);
	}

	void move_read_head(size_t n)
	{
		this->read_head += n;
	
		// 추가처리해줘야하는데 일단 씀. ㅋ
	}

	void move_write_head(size_t n)
	{
		this->write_head += n;

		// 추가처리해줘야하는데 일단 씀. ㅋ
	}

private:
	uint8_t*	buffer;

	size_t		read_head;
	size_t		write_head;
};