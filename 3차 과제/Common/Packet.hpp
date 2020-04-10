#include "Packet.h"
#include "exception.h"


//c2::concurrency::MemoryPoolTLS<Packet> Packet::packet_pool;

namespace c2
{

	Packet::Packet() : buffer{ nullptr }, payload_capacity{ kMaximumSegmentSize }, write_head{ 0 }, read_head{ 0 }//, ref_count{ 0 }
	{
		buffer = new char[kMaximumSegmentSize];
	}

	Packet::Packet(const Packet& other) : buffer{ other.buffer }, payload_capacity{ other.payload_capacity }, write_head{ other.write_head }, read_head{ other.read_head }//, ref_count{ other.ref_count }
	{
		//InterlockedIncrement64(&ref_count);
	}

	Packet::Packet(Packet&& other) noexcept : buffer{ other.buffer }, payload_capacity{ other.payload_capacity }, write_head{ other.write_head }, read_head{ other.read_head }//, ref_count{ other.ref_count }
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		other.buffer = nullptr;
	}

	Packet& Packet::operator=(const Packet& other)
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		if (this == &other)
			return *this;

		this->buffer = other.buffer;
		this->payload_capacity = other.payload_capacity;
		this->write_head = other.write_head;
		this->read_head = other.read_head;
		//this->ref_count = other.ref_count;

		//*this->ref_count += 1;

		return *this;
	}


	Packet& Packet::operator=(Packet&& other) noexcept
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		if (this == &other)
			return *this;

		this->buffer = other.buffer;
		this->payload_capacity = other.payload_capacity;
		this->write_head = other.write_head;
		this->read_head = other.read_head;
		//this->ref_count = other.ref_count;

		//memset(this ,0, sizeof(Packet));
		other.buffer = nullptr;
		other.payload_capacity = 0;
		other.write_head = 0;
		other.read_head = 0;
		//other.ref_count = nullptr;

		return *this;
	}

	Packet::~Packet()
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		//release();
		delete[] buffer;

		read_head = 0;
		write_head = 0;
	}

	void Packet::clear(void)
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		write_head = 0;
		read_head = 0;
	}
	//
	//void Packet::addRef()
	//{
	//	InterlockedIncrement64(&ref_count);
	//	//if (0 > InterlockedIncrement64(&ref_count))
	//	//{
	//		//// 해제.
	//		//packet_pool.free(this);
	//	//}
	//}
	//
	//void Packet::releaseRef()
	//{
	//	if (0 >= InterlockedDecrement64(&ref_count))
	//	{
	//		packet_pool.free(this);
	//	}
	//}

	char* Packet::get_buffer() const noexcept
	{
		return buffer;
	}

	void Packet::rewind() noexcept
	{
		this->write_head = this->read_head = 0;
	}

	void Packet::resize(size_t length)
	{
		char* new_buffer = new char[length] {};
		size_t new_capacity = length > payload_capacity ? payload_capacity : length;

		memcpy(new_buffer, this->buffer, new_capacity);

		delete[] this->buffer;

		this->buffer = new_buffer;
		this->payload_capacity = length;
		this->write_head = length > this->write_head ? this->write_head : length;
		this->read_head = length > this->read_head ? this->read_head : length;
	}

	size_t Packet::capacity() const noexcept
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		return payload_capacity;
	}

	size_t Packet::size() const noexcept
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		return write_head;
	}

	void Packet::move_write_head(size_t length)
	{
		write_head += length;
	}

	inline void Packet::move_read_head(size_t length)
	{
	}

	void Packet::write(const void* data, size_t size)
	{
#ifdef PROFILE_ON
		PROFILE_FUNC;
#endif
		if (write_head + size >= payload_capacity) // idx니깐 같아져도 문제가 생김.
		{
			size_t new_payload_capacity = payload_capacity * 2;
			size_t new_size = write_head + size > new_payload_capacity ? write_head + size : new_payload_capacity;

			resize(new_size);
		}

		// memcpy 때리는거보다.
		switch (size)
		{
		case 0:
			break;
		case 1:
			this->buffer[write_head] = *(uint8_t*)data;
			break;

		case 2:
			*(uint16_t*)(&this->buffer[write_head]) = *(uint16_t*)data;
			break;

		case 3:
			this->buffer[write_head] = *(uint8_t*)data;
			*(uint16_t*)(&this->buffer[write_head + 1]) = *((uint16_t*)((uint8_t*)data + 1));
			break;

		case 4:
			*(uint32_t*)(&this->buffer[write_head]) = *(uint32_t*)data;
			break;

		case 5:
			this->buffer[write_head] = *(uint8_t*)data;
			*(uint32_t*)(&this->buffer[write_head + 1]) = *((uint32_t*)((uint8_t*)data + 1));
			break;

		case 6:
			*(uint16_t*)(&this->buffer[write_head]) = *(uint16_t*)data;
			*(uint32_t*)(&this->buffer[write_head + 2]) = *((uint32_t*)((uint8_t*)data + 2));
			break;

		case 7:
			this->buffer[write_head] = *(uint8_t*)data;
			*(uint16_t*)(&this->buffer[write_head + 1]) = *((uint16_t*)((uint8_t*)data + 1));
			*(uint32_t*)(&this->buffer[write_head + 3]) = *((uint32_t*)((uint8_t*)data + 3));
			break;

		case 8:
			*(uint64_t*)(&this->buffer[write_head]) = *(uint64_t*)data;
			break;

		default: // 8 초과하는 데이터를이렇게 받는다.
			memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
			break;
		}

		write_head += size;
	}

	void Packet::read(Out void* data, size_t size)
	{
		//#ifdef PROFILE_ON
		//		PROFILE_FUNC;
		//#endif
		if (read_head + size > write_head) // idx니깐 같아져도 문제가 생김.
		{
			// 이건 무족너 문제 있는 것.
			// error handling... 
			c2::util::assert_if_false(false);
		}


		// 1 2 4  8 바이트 읽기에 대해서도 최적화를 해준다.
		switch (size)
		{
		case 0:
			break;

		case 1:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			break;

		case 2:
			*(uint16_t*)data = *(uint16_t*)&this->buffer[read_head];
			break;

		case 3:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
			break;

		case 4:
			*(uint32_t*)data = *(uint32_t*)&this->buffer[read_head];
			break;

		case 5:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint32_t*)((uint8_t*)data + 1)) = *(uint32_t*)&this->buffer[read_head + 1];
			break;

		case 6:
			*(uint16_t*)(data) = *(uint16_t*)&this->buffer[read_head];
			*((uint32_t*)((uint8_t*)data + 2)) = *(uint32_t*)&this->buffer[read_head + 2];
			break;


		case 7:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
			*((uint32_t*)((uint8_t*)data + 3)) = *(uint32_t*)&this->buffer[read_head + 3];
			break;

		case 8:
			*(uint64_t*)data = *(uint64_t*)&this->buffer[read_head];
			break;

		default: // 8 초과하는 데이터를이렇게 받는다.
			//memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
			memcpy(data, &buffer[read_head], size);
			break;
		}

		read_head += size;
	}

	void Packet::peek(Out void* data, size_t size)
	{
		if (read_head + size > write_head)
		{
			c2::util::assert_if_false(false);
		}

		// 1 2 4  8 바이트 읽기에 대해서도 최적화를 해준다.
		switch (size)
		{
		case 0:
			break;

		case 1:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			break;

		case 2:
			*(uint16_t*)data = *(uint16_t*)&this->buffer[read_head];
			break;

		case 3:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
			break;

		case 4:
			*(uint32_t*)data = *(uint32_t*)&this->buffer[read_head];
			break;

		case 5:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint32_t*)((uint8_t*)data + 1)) = *(uint32_t*)&this->buffer[read_head + 1];
			break;

		case 6:
			*(uint16_t*)(data) = *(uint16_t*)&this->buffer[read_head];
			*((uint32_t*)((uint8_t*)data + 2)) = *(uint32_t*)&this->buffer[read_head + 2];
			break;

		case 7:
			*(uint8_t*)data = *(uint8_t*)&this->buffer[read_head];
			*((uint16_t*)((uint8_t*)data + 1)) = *(uint16_t*)&this->buffer[read_head + 1];
			*((uint32_t*)((uint8_t*)data + 3)) = *(uint32_t*)&this->buffer[read_head + 3];
			break;

		case 8:
			*(uint64_t*)data = *(uint64_t*)&this->buffer[read_head];
			break;

		default: // 8 초과하는 데이터를이렇게 받는다.
			//memcpy(&buffer[write_head], data, size); // 현재 인덱스에 복사를 함.
			memcpy(data, &buffer[read_head], size);
			break;
		}

	}

	void Packet::reset()
	{
		write_head = read_head = 0; 
	}

	//Packet* Packet::alloc()
	//{
	//	return packet_pool.alloc();
	//}
	//
	//void Packet::free(Packet* packet)
	//{
	//	packet_pool.free(packet);
	//}

	//// primitive types version 
	//template<typename T>
	//Packet& Packet::operator<<(const T& src)
	//{
	//	//static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types"); // is_arithmetic<> int or float

	//	write(&src, sizeof(T));

	//	return *this;
	//}

	//template<typename T>
	//inline Packet& Packet::operator<<(T&& src)
	//{
	//	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types"); // is_arithmetic<> int or float
	//
	//	write(&src, sizeof(T));
	//
	//	return *this;
	//}

	//template<typename T>
	//Packet& Packet::operator>>(Out T& dest)
	//{
	//	//static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types"); // is_arithmetic<> int or float

	//	read(&dest, sizeof(T));

	//	return *this;
	//};

	//	// template specialization
	//	// string
	//	template<>
	//	Packet& Packet::operator<<(const std::string& data)
	//	{
	//#ifdef PROFILE_ON
	//		PROFILE_FUNC_T(" (std::string&)");
	//#endif
	//		const size_t length = data.size();
	//
	//		write(&length, sizeof(size_t));
	//		write(data.c_str(), length);
	//
	//		return *this;
	//	};
	//
	//
	//	template<>
	//	Packet& Packet::operator<<(const std::wstring& data)
	//	{
	//#ifdef PROFILE_ON
	//		PROFILE_FUNC_T(" (std::wstring&) ");
	//#endif
	//		const size_t length = data.size() * sizeof(wchar_t);
	//
	//		write(&length, sizeof(size_t));
	//		write(data.c_str(), length);
	//
	//		return *this;
	//	};
	//
	//	template<>
	//	Packet& Packet::operator>>(OUT std::string& data)
	//	{
	//#ifdef PROFILE_ON
	//		PROFILE_FUNC_T(" (std::string&) ");
	//#endif
	//		size_t length = {};
	//		read(&length, sizeof(size_t));
	//
	//		char* temp = new char[length + 1]{};
	//		read(temp, length);
	//
	//		data = temp;
	//
	//		delete[] temp;
	//
	//		return *this;
	//	};
	//
	//	template<>
	//	Packet& Packet::operator>>(OUT std::wstring& data)
	//	{
	//#ifdef PROFILE_ON
	//		PROFILE_FUNC_T(" (std::wstring&) ");
	//#endif
	//		int length = {};
	//		read(&length, sizeof(int));
	//
	//		wchar_t* temp = new wchar_t[length + 1]{};
	//		read(temp, length);
	//
	//		data = temp;
	//
	//		delete[] temp;
	//
	//		return *this;
	//	};
	//
	//
	//	// STL example 
	//	template<>
	//	Packet& Packet::operator<<(const std::list<int*>& data)
	//	{
	//#ifdef PROFILE_ON 
	//		PROFILE_FUNC_T(" (std::list<int*>&) ");
	//#endif
	//		const size_t length = data.size();
	//
	//		write(&length, sizeof(size_t));
	//
	//		for (int* a : data)
	//			write(&(*a), sizeof(int));
	//
	//		return *this;
	//	};
	//
	//	template<>
	//	Packet& Packet::operator>>(OUT std::list<int*>& data)
	//	{
	//#ifdef PROFILE_ON
	//		PROFILE_FUNC_T(" (std::list<int*>&) ");
	//#endif
	//		int length = {};
	//		read(&length, sizeof(int));
	//
	//		for (int i = 0; i < length; ++i)
	//		{
	//			int temp{};
	//			read(&temp, sizeof(int));
	//
	//			data.emplace_back(new int{ temp });
	//		}
	//
	//		return *this;
	//	};
}
//
//#include <iostream>
//#include <string>
//#include <list>
//
////#include "Profiler.h"
//
//#define OffsetOf(c, mv) ((size_t) & (static_cast<c*>(nullptr)->mv))

//struct Test
//{
//	int a;
//	char b;
//	size_t c;
//	float d;
//};

//void main()
//{
//	//LProfiler.reset(new Profiler);
//
//
////for (int i = 0; i < 10'0000; ++i)
////{
//	Packet packet;
//
//	int n[11]{ 0, 1, 0, 2, 2, 3, 4, 0, 1, 0, 0 };
//	double f[11]{ 0.10, 2.234, 0.100 };
//	float f2[11];
//	int n2[11]{};
//
//	for (int i = 0; i < 11; ++i)
//		packet << n[i];
//
//	for (auto& i : n2)
//		packet >> i;
//
//	for (int d : n2)
//		std::cout << d;
//	std::cout << std::endl;
//
//	for (int i = 0; i < 11; ++i)
//		packet << f[i];
//
//	for (auto& i : f2)
//		packet >> i;
//
//	for (auto d : f2)
//		std::cout << d;
//	std::cout << std::endl;
//
//
//	std::string str{ "orz" };
//	std::string temp2{ "" };
//
//	std::wstring wstr{ L"orz" };
//	std::wstring temp3{ L"" };
//
//	packet << str;
//	packet >> temp2;
//
//	packet << wstr;
//	packet >> temp3;
//
//
//	std::cout << str << std::endl;
//	std::wcout << wstr << std::endl;
//	std::cout << temp2 << std::endl;
//	std::wcout << temp3 << std::endl;
//
//
//	{
//		char text[] = "fjaslkdfjkalsfklasjkljklfkjalsfjlkljakldfjalkfjaskl";
//		char temp[100]{};
//
//		//PROFILE_AREA("memcpy");
//		memcpy(temp, text, sizeof(text));
//	}
//	////new떄문에 문제의 소지가 있음.
//	{
//		std::list<int*> iList;
//		std::list<int*> iList2;
//		for (int i = 0; i < 100; ++i)
//			iList.emplace_back(new int{ i * 7 });
//
//		packet << iList;
//		packet >> iList2;
//
//		std::cout << "=============================================" << std::endl;
//		for (int* d : iList2)
//			std::cout << *d << std::endl;
//	}
//
//

//	//struct TT
//	//{
//	//	int a;
//	//	float b;
//	//	double c;
//	//	int32_t d;
//	//	char e;
//	//};
//
//	//std::cout << ((size_t)& (static_cast<TT*>(nullptr)->a)) << std::endl;
//	//std::cout << ((size_t)& (static_cast<TT*>(nullptr)->b)) << std::endl;
//	//std::cout << ((0xFFFFFFFFFFFFFFFF) & (static_cast<TT*>(nullptr)->e)) << std::endl;
//
//
//}
