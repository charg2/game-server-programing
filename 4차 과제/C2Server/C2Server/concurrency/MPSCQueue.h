
namespace c2::concurrency
{
#include <Windows.h>

	template<class T>
	class MPSCQueue
	{
	private:
		struct Node
		{
			Node* next;
			T	  data;
		};

	public:
		MPSCQueue() : tail{ }, head{}
		{
			head = new Node();
			tail = head;
			head->next = nullptr;
		}
		~MPSCQueue()
		{}

		void push(T src)
		{
			Node* new_node = new Node{ nullptr , src };

			Node* prev_node = (Node*)InterlockedExchangePointer((void* volatile*)&tail, new_node);

			prev_node->next = new_node;
		}

		bool try_pop(T& dest)
		{
			Node* dummy_next = head->next;  // [] - []  

			if (nullptr == dummy_next)
			{
				return false;
			}
			else
			{
				dest = dummy_next->data;

				Node* head_ptr = head;

				head = dummy_next;

				delete head_ptr;

				return true;
			}
		}


		const bool empty() const
		{
			return head->next == nullptr;
		}

	private:
		alignas(64) Node* head;
		alignas(64) Node* volatile	tail;
	};
}