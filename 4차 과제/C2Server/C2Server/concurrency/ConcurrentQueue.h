#pragma once

#include <cstdint>  
#include <Windows.h>  

#include "BackOff.h"

//#include "CoucurrentQueueObjectPool.h"
#include "MemoryPool_Queue.h"
//#include "MemoryPool_Stack.h"


//#define MT_PROFILE_ON
// ĳ�ö��� 

#ifdef MT_PROFILE_ON

#include "Exception.h"
#include "ThreadCallHistory.h"
#include "CallHistory.h"

#endif


namespace c2::concurrency
{
	template<class T>
	class ConcurrentQueue
	{
	public:
		struct alignas(16)  Node
		{
			Node* next;
			T	  data;
		};

		struct alignas(16) EndNode
		{
			Node* node;
			uint64_t id;
		};

		c2::concurrency::\
			//ConcurrentQueueMemoryPool<Node, 4096, false>* node_pool;
			ConcurrentQueueMemoryPool<Node, 4096, false>* node_pool;
		EndNode* head;
		EndNode* tail;
		char		chcae_line_pad1[64 - (sizeof(void*) * 3)];
		int64_t		size;
		char		chcae_line_pad2[64 - (sizeof(int64_t))];


	public:
		// �׻� ���� ��� 1���� ����.
		//					 ��--->[ dummy_node ]<------��
		//		��>[ EndNode{ | , 0xBEEF}]	[ EndNode{ | , 0xCAFE}]<��
		//		��												    ��
		//  [head]													[tail]
		ConcurrentQueue() : head{ }, tail{ }, size{ 0 }
		{
			node_pool = new c2::concurrency::ConcurrentQueueMemoryPool<Node, 4096, false>;

			head = (EndNode*)_aligned_malloc(sizeof(EndNode), 64);
			tail = (EndNode*)_aligned_malloc(sizeof(EndNode), 64);

			head->node = tail->node = node_pool->alloc();
			//head->node = tail->node = new Node;

			head->node->next = nullptr;

			head->id = 0xBEEF;
			tail->id = 0xCAFE;
		}
		ConcurrentQueue(const ConcurrentQueue& other) = delete;
		ConcurrentQueue(ConcurrentQueue&& other) = delete;
		~ConcurrentQueue()
		{
			delete node_pool;

			_aligned_free(head);
			_aligned_free(tail);
		}

		// tail �� �Ѱ��� �Ѱ� �ٴ� ���� ����.
		// �ᱹ �ι�° �༮�� ��� �о���
		void push(T src)
		{
			Node* node = node_pool->alloc();
			node->next = nullptr;
			node->data = src;
			c2::concurrency::BackOff	backoff{ c2::concurrency::BackOff::min_delay };

			//c2::concurrency::BackOff backoff	{ 0 };

			for (;;)
			{
				EndNode local_tail{ tail->node, tail->id }; // ĸó

				if (nullptr == local_tail.node->next)		// first test �������?
				{
					if (NULL == InterlockedCompareExchange64((int64_t*)&this->tail->node->next, (int64_t)node, NULL)) // second test  ������ ���̱�.
					{
						InterlockedIncrement64(&this->size);		// ī��Ʈ 

						if (tail->node->next != nullptr)			// �� ��� ���� �о��� �������� ��� ���� ���ص� ��.. 
						{
							InterlockedCompareExchange128((int64_t*)tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)&local_tail);
						}

						break;
					}
				}
				else // node�� �� �ִ� ��� ������ ����.
				{
					if (local_tail.node->next != nullptr) // ���� ���� �о����;;.
					{
						if (0 == InterlockedCompareExchange128((int64_t*)tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)&local_tail))
						{
							backoff.do_backoff();
						}
					}
				}
			}
		}


		bool try_pop(Out T& dest)
		{
			//// node ������ 0���� return
			// count�� ���ϸ� ������ ����.
			if (0 > InterlockedDecrement64(&this->size))
			{
				InterlockedIncrement64(&this->size);
				return false;
			}

			// ĸó.
			alignas(16) EndNode		local_head;
			alignas(16) EndNode		local_tail;
			Node* next_node;
			c2::concurrency::BackOff	backoff{ c2::concurrency::BackOff::min_delay };

			for (;;)
			{
				local_head.node = this->head->node;
				local_head.id = this->head->id;
				local_tail.node = this->tail->node;
				local_tail.id = this->tail->id;
				next_node = local_head.node->next;

				if (local_head.node == local_tail.node) // ��� �ȹз�����.
				{
					if (nullptr == local_head.node->next)
					{
						continue;
					}
					else if (local_tail.node->next != nullptr)
					{
						if (0 == InterlockedCompareExchange128((int64_t*)tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)&local_tail))
						{
							backoff.do_backoff();
						}
						continue;
					}
				}
				else if (next_node != nullptr)
				{
					dest = next_node->data;

					if (1 == InterlockedCompareExchange128((int64_t*)this->head, (int64_t)(local_head.id + 1)
						, (int64_t)local_head.node->next, (int64_t*)&local_head))
					{
						//InterlockedDecrement64(&this->size);

						node_pool->free(local_head.node);

						return true;
					}
					else
					{
						backoff.do_backoff();
					}
				}
			}
		}

		bool empty() const
		{
			return this->head.node->next == nullptr;
		}


		// this method don't garantee concurrency-safe
		size_t unsafe_size() const
		{
			return this->size;
		}

	};

}