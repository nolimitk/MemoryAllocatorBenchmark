#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <thread>

#include "NKTime.h"

namespace NKTester
{
	using std::thread;
	using std::vector;
	using std::function;
	using std::endl;

	class TestConfig
	{
	public:
		wstring _log_name;

	public:
		int _count_thread;
		int _count_test;
		int _count_allocation;
		int _min_allocation_size;
		int _max_allocation_size;
		function<void*(size_t)> _allocator;
		function<void(void*)> _deallocator;

	public:
		TestConfig &operator = (const TestConfig &other)
		{
			_log_name = other._log_name;
			_count_thread = other._count_thread;
			_count_test = other._count_test;
			_count_allocation = other._count_allocation;
			_min_allocation_size = other._min_allocation_size;
			_max_allocation_size = other._max_allocation_size;
			_allocator = other._allocator;
			_deallocator = other._deallocator;
			return *this;
		}
	};

	class Tester
	{
	protected:
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
				ptr_vector.reserve(_config._count_allocation);

				for (int i = 0; i < _config._count_test; ++i)
				{
					ptr_vector.clear();

					__int64 elapsed_time = NK::time_elapsed_function([&] {
						for (int i = 0; i < _config._count_allocation; ++i)
						{
							size_t alloc_size = _config._min_allocation_size + (rand() % (_config._max_allocation_size - _config._min_allocation_size));
							void *ptr = _config._allocator(alloc_size);
							ptr_vector.push_back(ptr);
						}
					});

					_r._elapsed_allocation_vector.push_back(elapsed_time);

					elapsed_time = NK::time_elapsed_function([&] {
						for (int i = 0; i < _config._count_allocation; ++i)
						{
							void *ptr = ptr_vector.back();
							ptr_vector.pop_back();
							_config._deallocator(ptr);
						}
					});

					_r._elapsed_deallocation_vector.push_back(elapsed_time);
				}

				adjust();
			}

			void adjust(void)
			{
				for (auto iter = _r._elapsed_allocation_vector.begin(); iter != _r._elapsed_allocation_vector.end(); ++iter)
				{
					_r._total_elapsed_allocation += *iter;
				}

				_r._average_elapsed_allocation = _r._total_elapsed_allocation / _r._elapsed_allocation_vector.size();

				for (auto iter = _r._elapsed_deallocation_vector.begin(); iter != _r._elapsed_deallocation_vector.end(); ++iter)
				{
					_r._total_elapsed_deallocation += *iter;
				}

				_r._average_elapsed_deallocation = _r._total_elapsed_deallocation / _r._elapsed_deallocation_vector.size();
			}

			void report(std::wostream& os)
			{
				for (auto iter = _r._elapsed_allocation_vector.begin(); iter != _r._elapsed_allocation_vector.end(); ++iter)
				{
					os << L"thread : " << _r._thread_id << L" alloc : " << *iter << endl;
				}

				os << L"thread : " << _r._thread_id << L" alloc, total : " << _r._total_elapsed_allocation << ", average : " << _r._average_elapsed_allocation << endl;

				for (auto iter = _r._elapsed_deallocation_vector.begin(); iter != _r._elapsed_deallocation_vector.end(); ++iter)
				{
					os << L"thread : " << _r._thread_id << L" dealloc : " << *iter << endl;
				}

				os << L"thread : " << _r._thread_id << L" dealloc, total : " << _r._total_elapsed_deallocation << ", average : " << _r._average_elapsed_deallocation << endl;
			}

			void run(function<void*(size_t)> allocator, function<void(void*)> deallocator)
			{
				_config._allocator = allocator;
				_config._deallocator = deallocator;

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

		public:
			void set_config(const TestConfig& config)
			{
				_config = config;
			}

		protected:
			typedef vector<void *> PTRVECTOR;

		protected:
			Result _r;
			thread _t;

		protected:
			TestConfig _config;

		public:
			ThreadTester(const TestConfig& config)
				:_config(config)
			{
			}

			ThreadTester(void)
			{
			}
		};

	public:
		void run_thread(void)
		{
			_pThread_tester = new ThreadTester[_config._count_thread];

			for (int i = 0; i < _config._count_thread; i++)
			{
				_pThread_tester[i].set_config(_config);
				_pThread_tester[i].run(_config._allocator, _config._deallocator);
			}

			for (int i = 0; i < _config._count_thread; i++)
			{
				_pThread_tester[i].join();
			}	
		}

		void report(std::wostream& os, bool detail = false)
		{
			if (detail)
			{
				for (int i = 0; i < _config._count_thread; i++)
				{
					_pThread_tester[i].report(os);
				}
			}

			__int64 total_time_allocation = 0;
			__int64 total_time_deallocation = 0;

			for (int i = 0; i < _config._count_thread; i++)
			{
				const Result r = _pThread_tester[i].get_result();
				total_time_allocation += r._total_elapsed_allocation;
				total_time_deallocation += r._total_elapsed_deallocation;
			}

			os << _config._log_name.c_str()
				<< L", thread:" << _config._count_thread
				<< L", allocation:" << _config._count_allocation
				<< L", alloc_time:" << total_time_allocation << "us"
				<< L", dealloc_time:" << total_time_deallocation << "us" << endl;
		}

	protected:
		ThreadTester *_pThread_tester;
		const TestConfig _config;

	public:
		Tester(const TestConfig& config)
			:_config(config)
			, _pThread_tester(nullptr)
		{
		}

		~Tester(void)
		{
			if (_pThread_tester != nullptr)
			{
				delete[] _pThread_tester;
			}
		}
	};
};