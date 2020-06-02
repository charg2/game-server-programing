//
//
//
//#include <iostream>
//#include <thread>
//#include <vector>
//#include <mutex>
//
//#include "ConcurrentStack.h"
//#include "../util/Profiler.h"
//#include "../util/dump.h"
//
//struct Some16
//{
//	uint64_t a;
//	uint64_t b;
//
//	static thread_local int n;
//};
//
//
//thread_local int Some16::n = 0;
//
//constexpr size_t TEST_CASE = 1000000; // ¹ó¸¸
//constexpr size_t TEST_THREAD_COUNT = 12; //
//
//ConcurrentStackBase<Some16*> stack1;
//ConcurrentStack<Some16*, TEST_CASE* TEST_THREAD_COUNT> stack2;
//ConcurrentStack2<Some16*, TEST_CASE* TEST_THREAD_COUNT> stack3;
//
//std::mutex mtx;
//int64_t ttas_pop_cnt = 0;
//int64_t ttas_sleep_pop_cnt = 0;
//int64_t pop_cnt = 0;
//
//
//void simulate_concurrent_stack()
//{
//	LProfiler = new Profiler;
//
//	{
//		profile_func_t("");
//
//		for (uint64_t n{}; n < TEST_CASE; ++n)
//		{
//			stack1.push(new Some16{ n, n });
//		}
//	}
//
//	LProfiler->print_console("");
//
//}
//
//void simulate_concurrent_stack_using_back_off()
//{
//	LProfiler = new Profiler;
//
//	{
//		profile_func_t("");
//
//		for (uint64_t n{}; n < TEST_CASE; ++n)
//		{
//			stack2.push(new Some16{ n, n });
//		}
//	}
//
//	LProfiler->print_console("");
//}
//
//void simulate_concurrent_stack_using_back_off_sleep()
//{
//	LProfiler = new Profiler;
//
//	{
//		profile_func_t("");
//
//		for (uint64_t n{}; n < TEST_CASE; ++n)
//		{
//			stack3.push(new Some16{ n, n });
//		}
//	}
//
//	LProfiler->print_console("");
//}
//
//void pop_simulate_concurrent_stack()
//{
//	LProfiler = new Profiler;
//
//	int n = 0;
//	{
//		profile_func_t("");
//
//		Some16* p;
//		while (stack1.try_pop(p))
//		{
//			n += 1;
//		}
//	}
//
//	LProfiler->print_console("");
//
//	InterlockedAdd64(&pop_cnt, n);
//	std::cout << GetCurrentThreadId() << "ÀÇ pop È½¼ö :" << n << std::endl;
//}
//
//void pop_simulate_concurrent_stack_using_back_off()
//{
//	LProfiler = new Profiler;
//
//	int n = 0;
//	{
//		profile_func_t("");
//
//		Some16* p;
//		while (stack2.try_pop(p))
//		{
//			n += 1;
//		}
//	}
//
//	LProfiler->print_console("");
//	InterlockedAdd64(&ttas_pop_cnt, n);
//	std::cout << GetCurrentThreadId() << "ÀÇ ttas_backoff pop È½¼ö :" << n << std::endl;
//}
//
//void pop_simulate_concurrent_stack_using_back_off_sleep()
//{
//	LProfiler = new Profiler;
//
//	int n = 0;
//	{
//		profile_func_t("");
//
//		Some16* p;
//		while (stack2.try_pop(p))
//		{
//			n += 1;
//		}
//	}
//
//	LProfiler->print_console("");
//	InterlockedAdd64(&ttas_sleep_pop_cnt, n);
//	std::cout << GetCurrentThreadId() << "ÀÇ ttas_backoff_sleep pop È½¼ö :" << n << std::endl;
//}
//
//
//void main()
//{
//	std::cout << __FILE__ << std::endl;
//
//	LProfiler = new Profiler;
//
//	std::vector<std::thread*> threads1; // push
//	std::vector<std::thread*> threads2; // pop 
//
//	for (int n{}; n < TEST_THREAD_COUNT; ++n)
//	{
//		threads1.push_back(new std::thread{ simulate_concurrent_stack });
//	}
//
//	for (auto th : threads1)
//	{
//		th->join();
//	}
//
//	threads1.clear();
//
//	for (int n{}; n < TEST_THREAD_COUNT; ++n)
//	{
//		threads1.push_back(new std::thread{ simulate_concurrent_stack_using_back_off });
//	}
//
//	for (auto th : threads1)
//	{
//		th->join();
//	}
//
//	threads1.clear();
//
//
//
//	for (int n{}; n < TEST_THREAD_COUNT; ++n)
//	{
//		threads1.push_back(new std::thread{ simulate_concurrent_stack_using_back_off_sleep });
//	}
//
//	for (auto th : threads1)
//	{
//		th->join();
//	}
//
//	threads1.clear();
//
//
//	//for (int n{}; n < TEST_THREAD_COUNT; ++n)
//	//{
//	//	threads2.push_back(new std::thread{ pop_simulate_concurrent_stack_using_back_off });
//	//}
//
//	//for (int n{}; n < TEST_THREAD_COUNT; ++n)
//	//{
//	//	threads2.push_back(new std::thread{ pop_simulate_concurrent_stack });
//	//}
//
//	//for (auto th : threads2)
//	//{
//	//	th->join();
//	//}
//
//
//	std::cout << GetCurrentThreadId() << "ÀÇ pop È½¼ö :" << pop_cnt << std::endl;
//	std::cout << GetCurrentThreadId() << "ÀÇ ttas_backoff pop È½¼ö :" << ttas_pop_cnt << std::endl;
//
//	//Some16* p;
//	//int n1 = 0;
//
//	//while (stack1.try_pop(p))
//	//{
//	//	delete p;
//	//	++n1;
//	//}
//	//std::cout << n1 << std::endl;
//	//
//	//int n2 = 0;
//	//while (stack2.try_pop(p))
//	//{
//	//	delete p;
//	//	++n2;
//	//}
//	//std::cout << n2 << std::endl;
//
//	int n;
//	std::cin >> n;
//}
//
//
//
