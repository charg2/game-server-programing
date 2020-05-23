#pragma once

#include "concurrency.h"
#include "../util/exception.h"

namespace c2::concurrency
{
	template <typename Type, size_t Capacity = kDefaultCapacity, bool PlacementNew = true>
	class ConcurrentQueueMemoryPool
	{
		struct BlockNode
		{
			BlockNode() : next{ nullptr }, magic_number{ kDeadBeef }//, data{ }
			{
			}

			BlockNode*		next;
			size_t			magic_number;
			//const size_t	magic_number;
			Type			data;
		};

		struct alignas(64) EndNode
		{
			BlockNode*	node;
			uint64_t	id;
		};

	private:
		EndNode		tail;			// always read write
		EndNode		head;			
		int64_t		size;			
		static inline HANDLE		heap_handle{ INVALID_HANDLE_VALUE };	// only once write almost read-only...
		// ������Ī�� ���� ���ϰٴ�;

	public:
		ConcurrentQueueMemoryPool() : /*heap_handle{ INVALID_HANDLE_VALUE }, */ head{ nullptr, 1984 }, tail{ nullptr, 0x198A }
			, size{ 0 }

		{
			static_assert(Capacity > 0, "Capacity must be greater than zero.");

			if (INVALID_HANDLE_VALUE == ConcurrentQueueMemoryPool::heap_handle)
			{
				ConcurrentQueueMemoryPool::heap_handle = HeapCreate( /* HEAP_ZERO_MEMORY */ HEAP_GENERATE_EXCEPTIONS, /*Capacity * sizeof(BlockNode) * 2 */0, NULL);            // �ִ�ũ��(�ڵ� ����)
				if (INVALID_HANDLE_VALUE == ConcurrentQueueMemoryPool::heap_handle)
				{
					c2::util::crash_assert();
				}
			}

			head.node = (BlockNode*)HeapAlloc(ConcurrentQueueMemoryPool::heap_handle, HEAP_GENERATE_EXCEPTIONS, sizeof(BlockNode) * Capacity);

			//printf("%s total size : %d  block size : %d  block count : %d ptr : %p \n-------------------\n", "ConcurrentQueueMPool", sizeof(BlockNode)* Capacity, sizeof(BlockNode), Capacity, head.node);

			for (int n = 0; n < Capacity; ++n)
			{
				new(&head.node[n]) BlockNode;
				//printf("%d ��° ptr : %p total size : %llu   block size : %d  block count : %llu \n", n, &head.node[n], sizeof(BlockNode)* Capacity, sizeof(BlockNode), Capacity);
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

			//if (0 == HeapValidate(heap_handle, 0, NULL))
			//{
			//	printf("------------------\n %s \n total size : %d  \n block size : %d  block count : %d \n-------------------\n", __FILE__, sizeof(BlockNode) * Capacity, sizeof(BlockNode), Capacity);
			//}
			// § �ϼ�! 
			//   [][][][][]][][][][][][][]
			// [head]					[tail]
		}

		~ConcurrentQueueMemoryPool()
		{
			//HeapFree(ConcurrentQueueMemoryPool::heap_handle, 0, ); // �ı� �ѹ���
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
					if (nullptr == local_head.node->next) // case : ��¥ �������
					{
						// �Ҵ�� ������ ���� ��ŭ.
						local_head.node = (BlockNode*)HeapAlloc(ConcurrentQueueMemoryPool::heap_handle, HEAP_GENERATE_EXCEPTIONS, sizeof(BlockNode) );
						//new(local_head.node) BlockNode; 
						local_head.node->next		  = nullptr;
						local_head.node->magic_number = kDeadBeef;

						break;
					}
					else if (local_tail.node->next != nullptr) // tail�� �پ� ������...
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

			if (PlacementNew) // ������ Ŵ.
				new(&local_head.node->data) Type;


			return &local_head.node->data;
		}

		void  free(Type* src)
		{
			if (nullptr == src)
				return;

			// node Block ����ؼ� ���ϱ�.
			BlockNode* node = (BlockNode*)(((uint8_t*)src) - sizeof(BlockNode::next) - sizeof(BlockNode::magic_number));
			node->next = nullptr;

			if (node->magic_number != kDeadBeef)  // magicNumber Check
				c2::util::crash_assert();

			if (PlacementNew) // �ʿ��ϴٸ� �Ҹ��� ȣ��.
				node->data.~Type();



			for (;;) // ��� �ֱ� ���س���� �ؾ� �մϴ�.
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
					if (local_tail.node->next != nullptr) // ���� ���� �о����;;.
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

