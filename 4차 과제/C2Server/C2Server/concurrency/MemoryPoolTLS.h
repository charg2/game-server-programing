#pragma once


#include "tls.h"
#include "concurrency.h"
#include "../util/dump.h"
#include "../util/exception.h"

#include "MemoryPool_Queue.h"
//#include "MemPool_Queue64.h"
//#include "MemoryPool_Stack.h"
//#include "MemoryPool_Stack_Light.h"

// 블럭당 갯수등 바꿔보면서 테스트하자.


namespace c2::concurrency
{
	template<typename Type, size_t Capacity = defaultChunkCapacity, bool PlacementNew = true>
	class MemoryPoolTLS
	{
		struct Chunk;

		struct Block
		{
			Type	data;
			Chunk*  chunk;
		};

		struct Chunk
		{
		public:
			Chunk() : count_of_alloc{ -1 }, count_of_release{ -1 }//,	//sequential_block{ },  
			{																					// 
				for (size_t i = 0; i < numberOfBlockInChunk; ++i)							// 
					sequential_block[i].chunk = this;// 
			}

			~Chunk()
			{
			}

			Type* alloc()
			{
				++count_of_alloc;

				//if (count_of_release >= count_of_alloc)
				//	c2::util::crash_assert(false);

				if (count_of_alloc == maxBlockIndex)
					reset_tls(owner->pool_tls_idx);

				//sequential_block[count_of_alloc].chunk = this;

				return &sequential_block[count_of_alloc].data;
			}

			void free()
			{
				if (maxBlockIndex == InterlockedIncrement64(&count_of_release))
					owner->chunk_pool.free(this);
			}

			MemoryPoolTLS*	owner;
			int64_t			count_of_release;
			Block			sequential_block[numberOfBlockInChunk];
			int64_t			count_of_alloc;
		};

	public:
		MemoryPoolTLS() : /*count_of_alloc{ -1 }, */count_of_free{ -1 }
		{
			//using namespace c2::concurrency;

			//chunk_pool = new ConcurrentQueueMemoryPool<Chunk, Capacity, PlacementNew>;
			//chunk_pool = new ConcurrentQueueMemoryPool64<Chunk, Capacity, PlacementNew >;
			//chunk_pool = new ConcurrentStackMemoryPool<Chunk, Capacity, PlacementNew >;
			//chunk_pool = new ConcurrentLightStackMemoryPool<Chunk, Capacity, PlacementNew >;

			pool_tls_idx = get_tls_idx();
		}

		~MemoryPoolTLS()
		{
			//delete chunk_pool;
		}


		__forceinline Type* alloc()
		{
			Chunk* current_chunk = (Chunk*)get_tls(pool_tls_idx);

			if (nullptr == current_chunk)
			{
				current_chunk = chunk_pool.alloc();

				if (nullptr == current_chunk)
					c2::util::crash_assert();

				current_chunk->owner = this;

				set_tls(pool_tls_idx, current_chunk);

				//InterlockedIncrement64(&count_of_alloc);
			}

			return current_chunk->alloc();
		}


		void free(Type* src) const
		{
			((Block*)(src))->chunk->free();
		}

	private:
		//c2::concurrency::
		ConcurrentQueueMemoryPool<Chunk, Capacity, PlacementNew> chunk_pool;
		//ConcurrentQueueMemoryPool64<Chunk, Capacity, PlacementNew >* chunk_pool;
		 //ConcurrentStackMemoryPool<Chunk, Capacity, PlacementNew> chunk_pool;  // no ptr
		// ConcurrentLightStackMemoryPool<Chunk, Capacity, PlacementNew >* chunk_pool; 


		int64_t		pool_tls_idx;
		char		chcae_line_pad1[64 - sizeof(pool_tls_idx)];
		//int64_t		count_of_alloc;
		////char		count_of_alloc;
		//char		chcae_line_pad2[64 - sizeof(count_of_alloc)];
		int64_t		count_of_free;
		char		chcae_line_pad3[64 - sizeof(count_of_free)];
	};
}