#pragma once


#include "tls.h"
#include "concurrency.h"
#include "../util/dump.h"
#include "../util/exception.h"

//#include "MemoryPool_Queue.h"
//#include "MemPool_Queue64.h"
#include "MemoryPool_Stack.h"
//#include "MemoryPool_Stack_Light.h"

// 블럭당 갯수등 바꿔보면서 테스트하자.

namespace c2::concurrency
{
	template<typename Type, size_t Capacity = 1024, bool PlacementNew = true>
	class ThreadLocalMemoryPool
	{
		enum Config : size_t
		{
			NumberOfBlockInChunk = 256,
			MaxBlockIndex		 = NumberOfBlockInChunk - 1
		};

		struct Chunk;
		struct alignas(16)  Block
		{
			Type	data;   // 순서 변경시 오프셋 계산 다시 
			Chunk* chunk;  // 
		};

		typedef struct Chunk
		{
			Chunk() : allocated_count{ -1 }, released_count{ -1 }//,	//sequential_block{ },  
			{																					// 
				for (size_t i = 0; i < Config::NumberOfBlockInChunk; ++i)							// 
					sequential_block[i].chunk = this;// 
			}

			~Chunk()
			{}

			Type* alloc()
			{
				++allocated_count;
				//if (count_of_release >= count_of_alloc)
				//	c2::util::crash_assert(false);

				if (allocated_count == Config::MaxBlockIndex)
					reset_tls(owner->pool_tls_idx);

				//sequential_block[count_of_alloc].chunk = this;

				return &sequential_block[allocated_count].data;
			}

			void free()
			{
				if (maxBlockIndex == InterlockedIncrement64(&released_count))
					owner->chunk_pool->free(this);
			}

			Block					sequential_block[Config::NumberOfBlockInChunk];
			ThreadLocalMemoryPool* owner;
			int64_t					released_count;
			int64_t					allocated_count;
		};

	public:
		ThreadLocalMemoryPool() : /*count_of_alloc{ -1 }, */count_of_free{ -1 }
		{
			//using namespace c2::concurrency;
			//chunk_pool = new ConcurrentQueueMemoryPool<Chunk, Capacity, PlacementNew>;
			//chunk_pool = new ConcurrentQueueMemoryPool64<Chunk, Capacity, PlacementNew >;
			chunk_pool = new ConcurrentStackMemoryPool<Chunk, Capacity, PlacementNew>();
			//chunk_pool = new ConcurrentLightStackMemoryPool<Chunk, Capacity, PlacementNew >;

			pool_tls_idx = get_tls_idx();
		}

		~ThreadLocalMemoryPool()
		{
			delete chunk_pool;
		}


		__forceinline Type* alloc() // this, Chunk*, size_t
		{
			Chunk* current_chunk = reinterpret_cast<Chunk*>(get_tls(pool_tls_idx));
			//Chunk* current_chunk = reinterpret_cast<Chunk*>( get_tls(pool_tls_idx) );
			if (nullptr == current_chunk)
			{
				current_chunk = chunk_pool->alloc();

				/*if (nullptr == current_chunk)
					c2::util::crash_assert(false);*/

				current_chunk->owner = this;

				set_tls(pool_tls_idx, current_chunk);

				//InterlockedIncrement64(&count_of_alloc);
			}

			// Chunk::alloc();
			//size_t chunk_allocated_count = current_chunk->allocated_count += 1;
			size_t chunk_allocated_count = current_chunk->allocated_count += 1;
			if (chunk_allocated_count == Config::MaxBlockIndex)
				reset_tls(current_chunk->owner->pool_tls_idx);

			return &current_chunk->sequential_block[chunk_allocated_count].data;
			//return current_chunk->alloc();
		}


		void free(Type* src) const
		{
			//((Block*)(src))->chunk->free();
			Chunk* chunk = ((Block*)(src))->chunk;

			if (Config::MaxBlockIndex == InterlockedIncrement64(&chunk->released_count))
			{
				//chunk->owner->chunk_pool->free(chunk);
				this->chunk_pool->free(chunk);
			}
		}

	private:
		//c2::concurrency::
		//ConcurrentQueueMemoryPool<Chunk, Capacity, PlacementNew>* chunk_pool;
		//ConcurrentQueueMemoryPool64<Chunk, Capacity, PlacementNew >* chunk_pool;
		ConcurrentStackMemoryPool<Chunk, Capacity, PlacementNew>* chunk_pool;
		// ConcurrentLightStackMemoryPool<Chunk, Capacity, PlacementNew >* chunk_pool; 


		alignas(64) int64_t		pool_tls_idx;
		//alignas(64) int64_t		count_of_alloc;
		alignas(64) int64_t		count_of_free;
	};
}