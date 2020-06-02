#pragma once
#include <cstdint>
#include <Windows.h>
#include <type_traits>
#include "MemoryPool_Stack.h"
//#include "ThreadLocalMemoryPool.h"

#define Out
// no TTAS no Backoff
// 어째선지.. 내머신(3700x)에선 이게 성능이더 잘나옴...
// 노트북에선 안나오는데;

// 내머신(3700x)에선 ttas backoff spin이 더 안나옴.


// back off spin 

template<typename Type, size_t Reserve = 1024/* MemoryPool reserve */ >
class ConcurrentStack
{
	struct alignas(16) Node
	{
		Node* next;
		Type	data;
	};

	struct alignas(16) TopNode
	{
		Node* node;
		uint64_t	id;
	};

public:
	ConcurrentStack() : top{ nullptr, 1984 }, element_count{}
	{}

	~ConcurrentStack()
	{}

	void push(Type data)
	{
		BackOff backoff{ BackOff::min_delay };

		//Node* new_node = new Node();
		Node* new_node = node_pool.alloc();
		new_node->data = data;

		TopNode local_top{ this->top.node, this->top.id };

		for (;;)
		{
			if (this->top.id != local_top.id)
			{
				local_top.node = this->top.node;
				local_top.id = this->top.id;			// 사실 이거만 갱신해줘도 됨 일단은 ;; 

				continue;
			}
			else // 같으면 
			{
				new_node->next = local_top.node;

				if (0 == InterlockedCompareExchange128((int64_t*)&this->top, (int64_t)(this->top.id + 1), (int64_t)new_node, (int64_t*)&local_top))
				{
					backoff.do_backoff();

					continue;
				}
				else
				{
					return;
				}
			}
		}
	}

	bool try_push(Type& new_node)
	{
		TopNode local_top{ this->top.node, this->top.id };

		return InterlockedCompareExchange128((int64_t*)&this->top, (int64_t)(this->top.id + 1), (int64_t)new_node, (int64_t*)&local_top);
	}

	bool try_pop(Out Type& dest)
	{
		BackOff backoff{ BackOff::min_delay };
		TopNode local_top{ this->top.node, this->top.id };

		for (;;)
		{
			if (nullptr == local_top.node)
			{
				return false;
			}

			if (this->top.id != local_top.id)
			{
				local_top.node = this->top.node;
				local_top.id = this->top.id;

				continue;
			}
			else
			{
				if (0 == InterlockedCompareExchange128((int64_t*)&this->top, (int64_t)(local_top.id + 1), (int64_t)local_top.node->next, (int64_t*)&local_top))
				{
					backoff.do_backoff();

					continue;
				}
				else
				{
					dest = local_top.node->data;

					//delete local_top.node;
					node_pool.free(local_top.node);

					return true;
				}
			}

		}
	}

	size_t unsfae_size()
	{
		return element_count;
	}

private:
	TopNode									top;
	size_t									element_count;
	ConcurrentStackMemoryPool<Node, Reserve, false>	node_pool;
	//ThreadLocalMemoryPool<Node, Reserve, false>	node_pool;
	//char	cache_line[64 - sizeof(TopNode) - sizeof(size_t)];
};


