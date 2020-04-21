#pragma once

#include <cstdint>  
#include <Windows.h>  

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
		struct Node
		{
			Node* next;
			T	  data;
	
		};

		struct alignas(16) EndNode
		{
			Node*	 node;
			uint64_t id;
		};

		c2::concurrency::\
			//ConcurrentQueueMemoryPool<Node, 4096, false>* node_pool;
		ConcurrentQueueMemoryPool<Node, 4096, false>* node_pool;
		EndNode*	head;
		EndNode*	tail;
		char		chcae_line_pad1[64- ( sizeof(void*) *3 )];
		int64_t		size;
		char		chcae_line_pad2[64 - (sizeof(int64_t)) ];


	public:
		// �׻� ���� ��� 1���� ����.
		//					 ��--->[ dummy_node ]<------��
		//		��>[ EndNode{ | , 0xBEEF}]	[ EndNode{ | , 0xCAFE}]<��
		//		��												    ��
		//  [head]													[tail]
		ConcurrentQueue() : head { }, tail { }, size{ 0 }
		{
			node_pool = new c2::concurrency::ConcurrentQueueMemoryPool<Node, 4096, false>;

			head = (EndNode*)_aligned_malloc(sizeof(EndNode), 16);
			tail = (EndNode*)_aligned_malloc(sizeof(EndNode), 16);

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
			Node* node		= node_pool->alloc();
			node->next		= nullptr;
			node->data		= src;

			for (;;)
			{
				EndNode local_tail{ tail->node, tail->id };

				if (nullptr == local_tail.node->next) // �������?
				{
					if (NULL == InterlockedCompareExchange64((int64_t*)&this->tail->node->next, (int64_t)node, NULL)) // ������ ���̱�.
					{
						InterlockedIncrement64(&this->size);

						if (tail->node->next != nullptr) // �� ��� ���� �о��� ������������ ��� ���� ���ص� ��.. 
						{
							InterlockedCompareExchange128((int64_t*)tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)&local_tail);
						}

						break;
					}
				}
				else // node�� �� �ִ� ��� ������ ����.
				{
					if (local_tail.node->next != nullptr) // ���� ���� �о����;;.
						InterlockedCompareExchange128((int64_t*)& tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)& local_tail);
				}
			}

			
		}


		bool pop(Out T& dest)
		{
			//// node ������ 0���� return
			// count�� ���ϸ� ������ ����.
			if ( 0 > InterlockedDecrement64(&this->size) )
			{
				InterlockedIncrement64(&this->size);
				return false;
			}

			// ������
			EndNode		local_head;
			EndNode		local_tail;
			Node*		next_node;

			for (;;)
			{
				local_head.node = this->head->node;
				local_head.id	= this->head->id;
				local_tail.node = this->tail->node;
				local_tail.id	= this->tail->id;
				next_node		= local_head.node->next;

				if (local_head.node == local_tail.node) // ��� �ȹз�����.
				{
					if (nullptr == local_head.node->next) // 
					{
						continue;
					}
					else if (local_tail.node->next != nullptr) 
					{
						InterlockedCompareExchange128((int64_t*)& tail, local_tail.id + 1, (int64_t)local_tail.node->next, (int64_t*)& local_tail);
						continue;
					}
				}
				else if ( next_node != nullptr )
				{
					dest = next_node->data;

					if (InterlockedCompareExchange128((int64_t*)this->head, (int64_t)(local_head.id + 1)
						, (int64_t)local_head.node->next, (int64_t*)& local_head))
					{
						//InterlockedDecrement64(&this->size);
						
						node_pool->free(local_head.node);

						return true;
					}
				}
			}
		}

		bool empty() const
		{
			return this->head.node->next == nullptr;
		}

		size_t unsafe_size() const
		{
			return this->size;
		}
	
	};

}