#include <random>
#pragma comment(lib, "Winmm")


#if _M_AMD64
extern "C" int __stdcall spin_wait(uint64_t limit);
#endif
namespace c2::concurrency
{
	struct BackOff
	{
	public:
		void do_backoff();

	public:
		int limit;

		static constexpr size_t min_delay = 1000;
		static constexpr size_t max_delay = 10000;
	};

}