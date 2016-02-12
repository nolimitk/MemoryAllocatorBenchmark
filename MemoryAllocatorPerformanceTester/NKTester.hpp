#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <thread>

#include "NKTime.h"

namespace NKTester
{
	using std::thread;
	using std::wcout;
	using std::vector;
	using std::function;
	using std::endl;

	class Tester
	{
	public:
		const static int COUNT_ALLOCATION = 10000;
		const static int MIN_ALLOCATION_SIZE = 8;
		const static int MAX_ALLOCATION_SIZE = 36 * 1024;
		const static int COUNT_THREAD = 4;
		const static int COUNT_TEST = 5;

	public:
		class Result
		{
		public:
			thread::id _thread_id;
			vector<__int64> _elapsed_allocation_vector;
			vector<__int64> _elapsed_deallocation_vector;
			__int64 _total_elapsed_allocation;
			__int64 _average_elapsed_allocation;
			__int64 _total_elapsed_deallocation;
			__int64 _average_elapsed_deallocation;

		public:
			Result(void)
				:_total_elapsed_allocation(0)
				, _total_elapsed_deallocation(0)
				, _average_elapsed_allocation(0)
				, _average_elapsed_deallocation(0)
			{
			}
		};

		class ThreadTester
		{
		public:
			void run_sequentially(void)
			{
				PTRVECTOR ptr_vector;
				ptr_vector.reserve(COUNT_ALLOCATION);

				for (int i = 0; i < COUNT_TEST; ++i)
				{
					ptr_vector.clear();

					__int64 elapsed_time = NK::time_elapsed_function([&] {
						for (int i = 0; i < COUNT_ALLOCATION; ++i)
						{
							size_t alloc_size = MIN_ALLOCATION_SIZE + (rand() % (MAX_ALLOCATION_SIZE - MIN_ALLOCATION_SIZE));
							void *ptr = _allocator(alloc_size);
							ptr_vector.push_back(ptr);
						}
					});

					_r._elapsed_allocation_vector.push_back(elapsed_time);

					elapsed_time = NK::time_elapsed_function([&] {
						for (int i = 0; i < COUNT_ALLOCATION; ++i)
						{
							void *ptr = ptr_vector.back();
							ptr_vector.pop_back();
							_deallocator(ptr);
						}
					});

					_r._elapsed_deallocation_vector.push_back(elapsed_time);
				}
			}

			void report(void)
			{
				for (auto iter = _r._elapsed_allocation_vector.begin(); iter != _r._elapsed_allocation_vector.end(); ++iter)
				{
					_r._total_elapsed_allocation += *iter;
					wcout << L"thread : " << _r._thread_id << L" alloc : " << *iter << endl;
				}

				_r._average_elapsed_allocation = _r._total_elapsed_allocation / _r._elapsed_allocation_vector.size();

				wcout << L"thread : " << _r._thread_id << L" alloc, total : " << _r._total_elapsed_allocation << ", average : " << _r._average_elapsed_allocation << endl;

				for (auto iter = _r._elapsed_deallocation_vector.begin(); iter != _r._elapsed_deallocation_vector.end(); ++iter)
				{
					_r._total_elapsed_deallocation += *iter;
					wcout << L"thread : " << _r._thread_id << L" dealloc : " << *iter << endl;
				}

				_r._average_elapsed_deallocation = _r._total_elapsed_deallocation / _r._elapsed_deallocation_vector.size();

				wcout << L"thread : " << _r._thread_id << L" dealloc, total : " << _r._total_elapsed_deallocation << ", average : " << _r._average_elapsed_deallocation << endl;
			}

			void run(function<void*(size_t)> allocator, function<void(void*)> deallocator)
			{
				_allocator = allocator;
				_deallocator = deallocator;

				_t = thread([&] {
					run_sequentially();
				});

				_r._thread_id = _t.get_id();
			}

			void join(void)
			{
				_t.join();
			}

			const Result& get_result(void)
			{
				return _r;
			}

		protected:
			Result _r;
			thread _t;

		protected:
			function<void*(size_t)> _allocator;
			function<void(void*)> _deallocator;

		public:
			ThreadTester(void)
			{
			}
		};

	public:
		void run_thread(void)
		{
			ThreadTester *pThread_tester = new ThreadTester[COUNT_THREAD];

			for (int i = 0; i < COUNT_THREAD; i++)
			{
				pThread_tester[i].run(_allocator, _deallocator);
			}

			for (int i = 0; i < COUNT_THREAD; i++)
			{
				pThread_tester[i].join();
			}

			for (int i = 0; i < COUNT_THREAD; i++)
			{
				pThread_tester[i].report();
			}

			__int64 total_time_allocation = 0;
			__int64 total_time_deallocation = 0;
			for (int i = 0; i < COUNT_THREAD; i++)
			{
				const Result r = pThread_tester[i].get_result();
				total_time_allocation += r._total_elapsed_allocation;
				total_time_deallocation += r._total_elapsed_deallocation;
			}

			wcout << L"total alloction : " << total_time_allocation << endl;
			wcout << L"total dealloction : " << total_time_deallocation << endl;
		}

	protected:
		const function<void*(size_t)> _allocator;
		const function<void(void*)> _deallocator;

	protected:
		typedef vector<void *> PTRVECTOR;

	public:
		Tester(function<void*(size_t)> allocator, function<void(void*)> deallocator)
			:_allocator(allocator)
			, _deallocator(deallocator)
		{
		}
	};
};