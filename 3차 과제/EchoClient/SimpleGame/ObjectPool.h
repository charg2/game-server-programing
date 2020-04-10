#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <new.h>
//#include "exception.h"
//#include "Profiler.h"

// 오직 싱글 스레드용... 
// 나름 캐시 고려함...
// page크기로 할당하는게 맞지 않을까..
namespace c2
{
	/*constexpr size_t CAPACITY { 0'1024 };*/

#ifdef _WIN64
	constexpr size_t kDeadBeef{ 0xDDEEAADDBBEEEEFF };
	constexpr size_t kDeadDead{ 0xDEADDEADDEADDEAD };
#else
	constexpr size_t kDeadBeef{ 0xDEADBEEF };
	constexpr size_t kDeadDead{ 0xDEADDEAD };
#endif

#define simple_crash() { int* hazard_ptr { nullptr }; *hazard_ptr = 0xDEADDEAD; }


	template <typename Type, size_t Capacity = 1024, bool PlacementNew = false>
	class ObjectPool
	{
		typedef struct BlockNode   // aal
		{
			BlockNode() : next_block{ nullptr }, magic_number{ kDeadBeef }, data{ }
			{}
			~BlockNode()
			{}

			BlockNode*		next_block;
			const size_t	magic_number;
			Type			data;
		};

		//typedef struct BlockNodeChunck
		//{
		//	BlockNode blocks[Capacity];
		//};

	private:
		BlockNode*			free_block_header;	// object_list 
		BlockNode*			begin_block_header;	// 지울 블록 리스트.
		//BlockNodeChunck*	begin_bl_header;
		size_t				free_block_count;

	public:
		// -> header []-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[]-[nullptr]
		// cache hit 고려.
		ObjectPool()
		{
			static_assert(Capacity > 0, "Capacity must be greater than zero.");
		}

		bool init()
		{
			free_block_count = 0;
			size_t local_capacity = Capacity;
			free_block_header = new BlockNode[local_capacity];
			begin_block_header = free_block_header;

			free_block_header->next_block = &free_block_header[1];

			BlockNode* newBlock{ nullptr };

			for (this->free_block_count = 1; this->free_block_count < local_capacity; ++this->free_block_count)
			{
				newBlock = &free_block_header[free_block_count];
				newBlock->next_block = &free_block_header[free_block_count + 1];
			}

			newBlock->next_block = nullptr;

			return true;
		}

		virtual ~ObjectPool()
		{
			//#ifdef PROFILE_ON
			//			PROFILE_THIS;
			//#endif
			delete[] this->begin_block_header;

			//delete[] freeBlockHeader;

			//for (size_t n{} ; n < numberOfFreeBlock ; ++n)
			//{
			//	BlockNode* temp = freeBlockHeader->nextBlock;
			//	
			//	//if (PlacementNew)
			//	//	freeBlockHeader->data.~Type();

			//	delete &freeBlockHeader[n];
			//	
			//	//freeBlockHeader = temp;
			//}
		}

		Type* alloc(void)
		{
#ifdef PROFILE_ON
			PROFILE_THIS;
#endif
			// Node + sizeof(head) + sizeof(magicNumber)
			// 리스트에 더 꺼낼것이잇는지 보고 
			// 있으면 꺼내줌.
			// freeNode 뒤로 밀어줌.
			if (free_block_count)
			{
				BlockNode* temp = this->free_block_header;
				this->free_block_header = this->free_block_header->next_block;

				if (PlacementNew) // 생성자 킴.
					new(&temp->data) Type();

				--this->free_block_count;

				return &temp->data;
			}
			else
			{
				return nullptr;
			}
		}

		// 사용중이던 블럭을 해제한다.
		// Parameters: (DATA *) 블럭 포인터.
		void  free(Type* data)
		{
#ifdef PROFILE_ON
			PROFILE_THIS;
#endif
			BlockNode* temp = (BlockNode*)(((uint8_t*)data) - sizeof(BlockNode::next_block) - sizeof(BlockNode::magic_number));

			if (temp->magic_number != kDeadBeef)  //Check magic_number
			{
				//c2::util::crash_assert();
				simple_crash();
				return;
			}

			if (PlacementNew) // dtor / ctor  호출할지는정함.
				temp->data.~Type();

			temp->next_block = this->free_block_header;
			this->free_block_header = temp;
			++this->free_block_count;

			return;
		}


		// 현재 확보 된 블럭 개수를 얻는다. (메모리풀 내부의 전체 개수)
		[[nodiscard]] const size_t capactity(void) const noexcept
		{
			return Capacity;
		}

		[[nodiscard]] size_t get_free_block_count(void) const noexcept
		{
			return this->free_block_count;
		}
	};
}

#endif

