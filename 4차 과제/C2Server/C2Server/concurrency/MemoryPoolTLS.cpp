//#include "MemoryPoolTls.h"
//
//
////#include <iostream>
//#include <thread>
//#include <vector>
////
//#include "../util/Profiler.h"
//#include "../util/exception.h"
//
//#include "../util/dump.h"
//
////#include "ThreadCallHistory.h"
//
//#include "../util/CallHistory.h"
//
//using namespace c2::concurrency;
//
//namespace c2
//{
//	thread_local int	lThreadId = -1;
//}
//
//constexpr size_t countOfThread = 12;
//constexpr size_t countOfTest = 100;
//constexpr size_t 억 = 1000'0000'0;
////constexpr size_t numberOfTarget = 억 / 5;
//constexpr size_t numberOfTarget = 1'000'000;
//constexpr size_t kCheckSymbol = 0x0000000055555555;
//
//struct test
//{
//	size_t data = kCheckSymbol;
//	size_t count = 0;
//	size_t da1ta = kCheckSymbol;
//	size_t co1unt = 0;
//	//size_t dd;
//	//size_t dd1;
//	//size_t dd2;
//	//size_t dd3;
//
//};
//
//
//std::vector<std::thread> th_v;
//std::vector<std::thread> th_v2;
//
//test* alloc_table[countOfThread][numberOfTarget];
//ThreadLocalMemoryPool<test>* objPool;
//
//void native_new_delete(int idx)
//{
//	LProfiler.reset(new Profiler);
//
//	for (int k = 0; k < countOfTest; ++k)
//	{
//		{
//			PROFILE_FUNC_T("_native_new");
//			for (int n = 0; n < numberOfTarget; ++n)
//			{
//				alloc_table[idx][n] = new test;
//			}
//		}
//
//		{
//			PROFILE_FUNC_T("_native_delete");
//			
//			for (int n = 0; n < numberOfTarget; ++n)
//			{
//				delete alloc_table[idx][n];
//			}
//		}
//	}
//	LProfiler->print("native_new_delete 100'00000개 ");
//	LProfiler->output("native_new_delete 100'00000개 ");
//}
//
//
//void mem_pool_tls_alloc_free(int idx)
//{
//	LProfiler.reset(new Profiler);
//
//	for (int k = 0; k < countOfTest; ++k)
//	{
//		{
//			PROFILE_FUNC_T("_mempool_alloc");
//			
//			for (int n = 0; n < numberOfTarget; ++n)
//			{
//				alloc_table[idx][n] = objPool->alloc();
//			}
//		}
//
//		{
//			PROFILE_FUNC_T("_mempool_free");
//			
//			for (int n = 0; n < numberOfTarget; ++n)
//			{
//				objPool->release(alloc_table[idx][n]);
//			}
//		}
//	}
//	LProfiler->print("_mempool_alloc_free 100'00000개 ");
//	LProfiler->output("_mempool_alloc_free 100'00000개 ");
//}
//
//
//void main()
//{
//	std::cout << __FILE__ << std::endl;
//	SetUnhandledExceptionFilter(ExceptionFilter);
//	objPool = new ThreadLocalMemoryPool<test>;
//	int k = 0;
//
//	std::cout << "defualt heap new / delete " << std::endl;
//
//	for (int n = 0; n < 4; ++n)
//		th_v.push_back( std::thread( native_new_delete, k++));
//
//	for (auto&& th : th_v)
//		th.join();
//
//	std::cout << "private heap mempool_tls alloc / release " << std::endl;
//
//	for (int n = 0; n < 4; ++n)
//		th_v2.push_back(std::thread(mem_pool_tls_alloc_free, k++));
//
//	for (auto&& th : th_v2)
//		th.join();
//}
//
