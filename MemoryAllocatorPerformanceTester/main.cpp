
#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <thread>

using namespace std;
using namespace chrono;

template<class func>
__int64 time_elapsed_function(func &&f)
{
	high_resolution_clock::time_point start_tick = high_resolution_clock::now();
	f();
	high_resolution_clock::time_point end_tick = high_resolution_clock::now();
	return duration_cast<microseconds>(end_tick - start_tick).count();
}

class MemoryTester
{
public:
	const static int COUNT_ALLOCATION = 100;
	const static int MIN_ALLOCATION_SIZE = 8;
	const static int MAX_ALLOCATION_SIZE = 36 * 1024;

protected:
	typedef vector<void *> PTRVECTOR;

protected:
	PTRVECTOR _ptr_vector;

public:
	__int64 allocation_sequencial(void)
	{
		return time_elapsed_function([&] {
			for (int i = 0; i < COUNT_ALLOCATION; ++i)
			{
				size_t alloc_size = MIN_ALLOCATION_SIZE + (rand() % (MAX_ALLOCATION_SIZE - MIN_ALLOCATION_SIZE));
				void *ptr = _allocator(alloc_size);
				_ptr_vector.push_back(ptr);
			}
		});
	}

public:
	void run(void)
	{
		wcout << _name.c_str() << endl;

		thread t([&] {
			__int64 elapsed_time = allocation_sequencial();
			wcout << "elpased time malloc " << elapsed_time << endl;
		});
		
		
		
		

		/*
		ThreadMemoryTester* pTester = new ThreadMemoryTester[_count_thread];
		thread* pThread = new thread[_count_thread];
		TestResult* pResult = new TestResult[_count_thread];

		for (int i = 0; i < _count_thread; ++i)
		{
			pThread[i] = thread([&] {
				pResult[i]._elapsed_alloc = pTester[i].allocation_sequencial(_allocator);
				pResult[i]._elapsed_dealloc = pTester[i].deallocation_sequencial(_deallocator);
			});
		}

		for (int i = 0; i < _count_thread; ++i)
		{
			pThread[i].join();
		}

		wcout << _name.c_str() << endl;

		for (int i = 0; i < _count_thread; ++i)
		{
			wcout << "allocation, " << pResult[i]._elapsed_alloc << endl;
			wcout << "deallocation, " << pResult[i]._elapsed_dealloc << endl;
		}
		*/
	}

	void report(void)
	{
		wcout << _name.c_str() << endl;
		//wcout << "allocation, " << _elapsed_alloc << endl;
		//wcout << "deallocation, " << _elapsed_dealloc << endl;
	}

public:
	const wstring get_name(void) const { return _name; }

protected:
	class ThreadMemoryTester
	{
	public:
		template<class func>
		__int64 allocation_sequencial(func &&f)
		{
			return time_elapsed_function([&] {
				for (int i = 0; i < COUNT_ALLOCATION; ++i)
				{
					size_t alloc_size = MIN_ALLOCATION_SIZE + (rand() % (MAX_ALLOCATION_SIZE - MIN_ALLOCATION_SIZE));

					void *ptr = f(alloc_size);
					_ptr_vector.push_back(ptr);
				}
			});
		}

		template<class func>
		__int64 deallocation_sequencial(func &&f)
		{
			return time_elapsed_function([&] {
				for (int i = 0; i < COUNT_ALLOCATION; ++i)
				{
					void *ptr = _ptr_vector.back();
					_ptr_vector.pop_back();
					f(ptr);
				}
			});
		}

	protected:
		typedef vector<void *> PTRVECTOR;

	protected:
		PTRVECTOR _ptr_vector;

	public:
		ThreadMemoryTester(void)
		{
			_ptr_vector.reserve(COUNT_ALLOCATION);
		}
	};

	class TestResult
	{
	public:
		const __int64 get_elapsed_alloc(void) const { return _elapsed_alloc; }
		const __int64 get_elapsed_dealloc(void) const { return _elapsed_dealloc; }

	public:
		__int64 _elapsed_alloc;
		__int64 _elapsed_dealloc;
	};

protected:
	const function<void*(size_t)> _allocator;
	const function<void(void *)> _deallocator;
	const wstring _name;
	const int _count_thread;

public:
	MemoryTester(function<void*(size_t)> allocator, function<void(void*)> deallocator, const wstring& name, const int count_thread)
		:_allocator(allocator)
		, _deallocator(deallocator)
		, _name(name)
		, _count_thread(count_thread)
	{
	}
};

void *func_new(size_t size)
{
	void *ptr = operator new(size);
	return ptr;
}

void func_delete(void *ptr)
{
	operator delete(ptr);
}

void *func_malloc(size_t size)
{
	void *ptr = malloc(size);
	return ptr;
}

void func_free(void *ptr)
{
	free(ptr);
}



void performance_test(void)
{
	srand(static_cast<unsigned int>(time(nullptr)));

	const static int COUNT_THREADS = 4;
	const static int NUMBER_TEST = 2;

	MemoryTester tester(func_malloc, func_free, L"malloc/free", 4);
	tester.run();

	//MemoryTester tester(func_malloc, func_free, L"malloc/free", 4);
	//tester.run();

	/*
	MemoryTester tester2(func_new, func_delete, L"new/delete");

	thread t[COUNT_THREADS];
	
	for (int j = 0; j < NUMBER_TEST; ++j)
	{
		for (int i = 0; i < _countof(t); ++i)
		{
			t[i] = thread([&] {
				tester[i] = MemoryTester(malloc, free, L"malloc/free");
				tester[i].run();
			});
		}

		for (int i = 0; i < _countof(t); ++i)
		{
			t[i].join();
		}

		for (int i = 0; i < _countof(tester); ++i)
		{
			tester[i].report();
		}
	}
	*/
}

/*
void main(void)
{
	wcout << L"Memory allocator performance tester" << endl;
	wcout << L"01.30.2016 nolimitk GPL" << endl;

	performance_test();

	wchar_t pause;
	wcin >> pause;
}
*/

// lambda & thread
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

				__int64 elapsed_time = time_elapsed_function([&] {
					for (int i = 0; i < COUNT_ALLOCATION; ++i)
					{
						size_t alloc_size = MIN_ALLOCATION_SIZE + (rand() % (MAX_ALLOCATION_SIZE - MIN_ALLOCATION_SIZE));
						void *ptr = _allocator(alloc_size);
						ptr_vector.push_back(ptr);
					}
				});

				_r._elapsed_allocation_vector.push_back(elapsed_time);

				elapsed_time = time_elapsed_function([&] {
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
	/*
	void run_sequentially(Result& r)
	{
		PTRVECTOR ptr_vector;

		__int64 elapsed_time = time_elapsed_function([&] {
			for (int i = 0; i < COUNT_ALLOCATION; ++i)
			{
				size_t alloc_size = MIN_ALLOCATION_SIZE + (rand() % (MAX_ALLOCATION_SIZE - MIN_ALLOCATION_SIZE));
				void *ptr = _allocator(alloc_size);
				ptr_vector.push_back(ptr);
			}
		});

		r._elapsed_allocation_vector.push_back(elapsed_time);

		elapsed_time = time_elapsed_function([&] {
			for (int i = 0; i < COUNT_ALLOCATION; ++i)
			{
				void *ptr = ptr_vector.back();
				ptr_vector.pop_back();
				_deallocator(ptr);
			}
		});

		r._elapsed_deallocation_vector.push_back(elapsed_time);
	}
	*/
	
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

		/*
		{
			__int64 total_elapsed = 0;
			__int64 average_elapsed = 0;
			for (auto iter = r._elapsed_allocation_vector.begin(); iter != r._elapsed_allocation_vector.end(); ++iter)
			{
				total_elapsed += *iter;
				wcout << L"thread : " << t.get_id() << L" alloc : " << *iter << endl;
			}

			average_elapsed = total_elapsed / r._elapsed_allocation_vector.size();

			wcout << L"thread : " << t.get_id() << L" alloc, total : " << total_elapsed << ", average : " << average_elapsed << endl;

			total_elapsed = 0;
			for (auto iter = r._elapsed_deallocation_vector.begin(); iter != r._elapsed_deallocation_vector.end(); ++iter)
			{
				total_elapsed += *iter;
				wcout << L"thread : " << t.get_id() << L" dealloc : " << *iter << endl;
			}

			average_elapsed = total_elapsed / r._elapsed_deallocation_vector.size();

			wcout << L"thread : " << t.get_id() << L" dealloc, total : " << total_elapsed << ", average : " << average_elapsed << endl;
		}
		*/
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

void main(void)
{
	wcout << L"Memory allocator performance tester" << endl;
	wcout << L"01.30.2016 nolimitk GPL" << endl;

	srand(static_cast<unsigned int>(time(nullptr)));

	Tester t(malloc, free);
	t.run_thread();

	wchar_t pause;
	wcin >> pause;
}


