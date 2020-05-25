#pragma once
#include "BackOff.h"
#include "concurrency.h"


// Lock - Free ConcurrentStackMemoryPool
// Stack 구조를 통함.

namespace c2::concurrency
{
	template <typename Type, size_t Capacity = 1024, bool PlacementNew = true>
	class ConcurrentStackMemoryPool
	{
		enum Keyword : size_t
		{
			DEAD = 0xDDEEAADDDDEEAADD
		};

		struct BlockNode   // aal
		{
			BlockNode() : next_block{ nullptr }, magic_number{ Keyword::DEAD }
			{
			}

			Type			data;
			size_t			magic_number;
			BlockNode* next_block;
		};

		struct alignas(16) TopNode
		{
			BlockNode* node;
			uint64_t	stamp;
		};

	public:
		ConcurrentStackMemoryPool() : top{ nullptr }, heap_handle{ INVALID_HANDLE_VALUE }
		{
			heap_handle = HeapCreate( /* HEAP_ZERO_MEMORY */ HEAP_GENERATE_EXCEPTIONS, 0, NULL);
			if (INVALID_HANDLE_VALUE == heap_handle)
			{
				size_t* invliad_ptr{};
				*invliad_ptr = Keyword::DEAD;
			}

			top = (TopNode*)HeapAlloc(heap_handle, HEAP_GENERATE_EXCEPTIONS, sizeof(TopNode));
			top->node = (BlockNode*)HeapAlloc(heap_handle, HEAP_GENERATE_EXCEPTIONS, sizeof(BlockNode) * Capacity);
			top->stamp = 1984;

			int n = 0;
			for (int n = 0; n < Capacity - 1; ++n)
			{
				new(&top->node[n]) BlockNode;
				top->node[n].next_block = &top->node[n + 1];
			}

			new(&top->node[n]) BlockNode;
			top->node[n].next_block = nullptr;

			//top->node->next_block = &top->node[1];
			//BlockNode* newBlock = nullptr;

			//for (this->freeBlock_count = 1; this->freeBlock_count < Capacity; ++this->freeBlock_count)
			//{
				//newBlock = &top->node[freeBlock_count];
				//newBlock->next_block = &top->node[freeBlock_count + 1];
			//}
			//newBlock->next_block = nullptr;
		}

		~ConcurrentStackMemoryPool()
		{
			HeapDestroy(heap_handle);
		}

		Type* alloc(void)
		{
			BlockNode* temp{ nullptr };
			TopNode		local_top{ this->top->node, this->top->stamp };
			BlockNode* new_block;
			BackOff     backoff{ BackOff::min_delay };

			for (;;)
			{
				if (nullptr == local_top.node)
				{
					// 할당해서 줌;
					//new_block = new BlockNode; //(BlockNode*)HeapAlloc(heap_handle, NULL, sizeof(BlockNode));
					new_block = (BlockNode*)HeapAlloc(heap_handle, HEAP_GENERATE_EXCEPTIONS, sizeof(BlockNode));
					new_block->next_block = nullptr;
					new_block->magic_number = Keyword::DEAD;
					//new(new_block) BlockNode;

					if (PlacementNew) // 생성자 킴.
						new(&new_block->data) Type;

					return &new_block->data;
				}

				if (local_top.stamp == top->stamp)
				{
					temp = this->top->node;

					if (1 == InterlockedCompareExchange128((int64_t*)this->top, (LONG64)(local_top.stamp + 1), (LONG64)local_top.node->next_block, (LONG64*)&local_top))
						break;
					else
						backoff.do_backoff();
				}
				else // 시도 조차 안했따면 직접 갱신.
				{
					local_top.node = top->node;
					local_top.stamp = top->stamp;
				}
			}

			InterlockedDecrement64(&this->freeBlock_count);

			if (PlacementNew) // 생성자 킴.
				new(&temp->data) Type;

			return &temp->data;
		}

		//////////////////////////////////////////////////////////////////////////
		// 사용중이던 블럭을 해제한다.
		//
		// Parameters: (DATA *) 블럭 포인터.
		// Return: (BOOL) TRUE, FALSE.
		//////////////////////////////////////////////////////////////////////////
		void  free(Type* data)
		{
			BackOff     backoff{ BackOff::min_delay };

			if (((BlockNode*)data)->magic_number != Keyword::DEAD)  // magicNumber Check
			{
				int* invliad_ptr{};
				*invliad_ptr = Keyword::DEAD;
			}


			if (PlacementNew) // dtor / ctor  호출할지는정함.
				((BlockNode*)data)->data.~Type();

			TopNode local_top{ this->top->node, this->top->stamp };

			for (;;)
			{
				((BlockNode*)data)->next_block = top->node;

				if (local_top.stamp == top->stamp)
				{
					if (1 == InterlockedCompareExchange128((int64_t*)this->top, (LONG64)(local_top.stamp + 1), (LONG64)data, (LONG64*)&local_top))
						break;
					else
						backoff.do_backoff();
				}
				else
				{
					local_top.node = top->node;
					local_top.stamp = top->stamp;
				}
			}

			InterlockedIncrement64(&this->freeBlock_count);
		}


	private:
		alignas(64) TopNode*	top;
		alignas(64) int64_t		freeBlock_count;
		HANDLE					heap_handle;
	};

}