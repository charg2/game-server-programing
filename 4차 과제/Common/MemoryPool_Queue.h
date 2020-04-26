#pragma once

#include "concurrency.h"
#include "exception.h"

namespace c2::concurrency
{
	template <typename Type, size_t Capacity = kDefaultCapacity, bool PlacementNew = true>
	class ConcurrentQueueMemoryPool
	{
		struct alignas(16) BlockNode
		{
			BlockNode() : next{ nullptr }, magic_number{ kDeadBeef }//, data{ }
			{
			}

			BlockNode*		next;
			size_t			magic_number;
			//const size_t	magic_number;
			Type			data;
		};

		struct alignas(16) EndNode
		{
			BlockNode*	node;
			uint64_t	id;
		};

	private:
		HANDLE		heap_handle;			// almost read-only
		char		chche_line_pad1[64- sizeof(heap_handle)];
		EndNode		tail;
		char		chche_line_pad2[64 - sizeof(tail)];
		EndNode		head;
		char		chche_line_pad3[64 - sizeof(head)];
		int64_t		size;
		char		chche_line_pad4[64 - sizeof(size)];


	public:
		ConcurrentQueueMemoryPool() : heap_handle{ INVALID_HANDLE_VALUE }, head{ nullptr, 1984 }, tail{ nullptr, 0x198A }
			, size{ 0 }
		{
			static_assert(Capacity > 0, "Capacity must be greater than zero.");

			heap_handle = HeapCreate( /* HEAP_ZERO_MEMORY */ NULL, Capacity * sizeof(BlockNode) * 2, NULL);            // 최대크기(자동 증가)
			if (INVALID_HANDLE_VALUE == heap_handle)
				c2::util::crash_assert();

			head.node = (BlockNode*)HeapAlloc(heap_handle, 0, sizeof(BlockNode) * Capacity);

			for (int n = 0; n < Capacity; ++n)
			{
				new(&head.node[n]) BlockNode;
				/*((BlockNode*)(&head.node[n]))->magic_number = kDeadBeef;
				((BlockNode*)(&head.node[n]))->next			= nullptr;*/
			}

			head.node->next = &head.node[1];

			BlockNode* newBlock = nullptr;

			for (this->size = 1; this->size < Capacity; ++this->size)
			{
				newBlock	   = &head.node[size];
				newBlock->next = &head.node[size + 1];
			}

			newBlock->next = nullptr;

			this->tail.node = newBlock;

			// 짠 완성! 
			//   [][][][][]][][][][][][][]
			// [head]					[tail]
		}

		~ConcurrentQueueMemoryPool()
		{
			HeapDestroy(heap_handle); // 파괴 한방컷
		}

		Type* alloc(void)
		{
			EndNode			local_head;
			EndNode			local_tail;

			for (;;)
			{
				local_head.node		= this->head.node;
				local_head.id		= this->head.id;
				local_tail.node		= this->tail.node;
				local_tail.id		= this->tail.id;


				if (local_head.node == local_tail.node) 
				{
					if (nullptr == local_head.node->next) // case : 진짜 비었을때
					{
						// 할당시 현재의 갯수 만큼.
						local_head.node = (BlockNode*)HeapAlloc( heap_handle, NULL, sizeof(BlockNode) );

						//new(local_head.node) BlockNode; 
						local_head.node->next		  = nullptr;
						local_head.node->magic_number = kDeadBeef;

						break;
					}
					else if (local_tail.node->next != nullptr) // tail이 붙어 있을때...
					{
						InterlockedCompareExchange128((int64_t*)&tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)& local_tail);
						continue;
					}
				}
				else
				{
					if (local_head.id == this->head.id) // test
					{
						if (InterlockedCompareExchange128((int64_t*)&this->head, (int64_t)(local_head.id + 1) // tas
							, (int64_t)local_head.node->next, (int64_t*)& local_head))
						{
							InterlockedDecrement64(&this->size);

							break;
						}
					}
				}
			}

			if (PlacementNew) // 생성자 킴.
				new(&local_head.node->data) Type;


			return &local_head.node->data;
		}

		void  free(Type* src)
		{
			if (nullptr == src)
				return;

			// node Block 계산해서 구하기.
			BlockNode* node = (BlockNode*)(((uint8_t*)src) - sizeof(BlockNode::next) - sizeof(BlockNode::magic_number));
			node->next = nullptr;

			if (node->magic_number != kDeadBeef)  // magicNumber Check
				c2::util::crash_assert();

			if (PlacementNew) // 필요하다면 소멸자 호출.
				node->data.~Type();



			for (;;) // 계속 넣기 위해노력을 해야 합니다.
			{
				EndNode local_tail{ tail.node, tail.id };

				if ( nullptr == tail.node->next )
				{
					if ( NULL == InterlockedCompareExchange64((int64_t*)& this->tail.node->next, (int64_t)node, NULL) )
					{
						InterlockedIncrement64(&this->size);

						if (local_tail.node->next != nullptr)
						{
							InterlockedCompareExchange128((int64_t*)&tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)&local_tail);
						}

						break;
					}
				}
				else
				{
					if (local_tail.node->next != nullptr) // 제발 누가 밀었어라;;.
						InterlockedCompareExchange128((int64_t*)& tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)& local_tail);
				}
			}
		}

		const size_t capacity(void) const noexcept
		{
			return Capacity;
		}

		size_t unsafe_size(void) const noexcept
		{
			return this->size;
		}


		inline bool empty() const
		{
			return this->head.node->next == nullptr;
		}

	};
}

