
#include <iostream>
#include <fstream>
using namespace std;
#include <concrt.h>

#include "scalable_allocator.h"

#include "NKTester.hpp"

void *func_malloc(size_t size)
{
	void *ptr = malloc(size);
	return ptr;
}

void func_free(void *ptr)
{
	free(ptr);
}

void *func_new(size_t size)
{
	void *ptr = operator new(size);
	return ptr;
}

void func_delete(void *ptr)
{
	operator delete(ptr);
}

void *func_concurrency_alloc(size_t size)
{
	void *ptr = Concurrency::Alloc(size);
	return ptr;
}

void func_concurrency_free(void *ptr)
{
	Concurrency::Free(ptr);
}

void *func_tbb_alloc(size_t size)
{
	void *ptr = scalable_malloc(size);
	return ptr;
}

void func_tbb_free(void *ptr)
{
	scalable_free(ptr);
}

void print_help(const string& filename)
{
	wcout << filename.c_str() << L" memory_type [-standalone]" << endl;
	wcout << L"memory_type : malloc, new, concurrency, tbb" << endl;
}

void pause(void)
{
	wchar_t pause;
	wcin >> pause;
}

int main( int argc, char *argv[] )
{
	// main arguments
	enum MEMORYTYPE
	{
		MEMORYTYPE_NOTHING,
		MEMORYTYPE_MALLOCFREE,
		MEMORYTYPE_NEWDELETE,
		MEMORYTYPE_CONCURRENCY,
		MEMORYTYPE_TBBMALLOC,
	};
	
	bool standalone = false;
	bool help = false;
	bool concurrent = false;
	MEMORYTYPE memory_type = MEMORYTYPE_NOTHING;

	if (argc > 1)
	{
		if (strcmp(argv[1], "malloc") == 0)
		{
			memory_type = MEMORYTYPE_MALLOCFREE;
		}
		else if (strcmp(argv[1], "new") == 0)
		{
			memory_type = MEMORYTYPE_NEWDELETE;
		}
		else if (strcmp(argv[1], "concurrency") == 0)
		{
			memory_type = MEMORYTYPE_CONCURRENCY;
		}
		else if (strcmp(argv[1], "tbb") == 0)
		{
			memory_type = MEMORYTYPE_TBBMALLOC;
		}

		for (int i = 2; i < argc; ++i)
		{
			if (strcmp(argv[i], "-standalone") == 0)
			{
				standalone = true;
			}
			else if (strcmp(argv[i], "-concurrent") == 0)
			{
				concurrent = true;
			}
			else if (strcmp(argv[i], "-help") == 0)
			{
				help = true;
			}
		}
	}
	else
	{
		help = true;
		standalone = true;
	}
	///

	if (standalone)
	{
		wcout << L"Memory allocator performance tester" << endl;
		wcout << L"02.13.2016 nolimitk GPL" << endl;
	}

	if (memory_type == MEMORYTYPE_NOTHING)
	{
		help = true;
	}

	if (help)
	{
		print_help(argv[0]);
		pause();
		return -1;
	}

	//srand(static_cast<unsigned int>(time(nullptr)));

	// log file
	wofstream ofs;
	ofs.open(L"log.txt", wofstream::out | wofstream::app);
	///

	NKTester::TestConfig config;

	switch (memory_type)
	{
	case MEMORYTYPE_MALLOCFREE:
		{
			config._log_name = L"mallocfree";
			config._allocator = func_malloc;
			config._deallocator = func_free;
			config._count_thread = 4;
			config._count_test = 5;
			config._count_allocation = 10000;
			config._min_allocation_size = 8;
			config._max_allocation_size = 36 * 1024;
		}
		break;
	case MEMORYTYPE_NEWDELETE:
		{
			config._log_name = L"newdelete";
			config._allocator = func_new;
			config._deallocator = func_delete;
			config._count_thread = 4;
			config._count_test = 5;
			config._count_allocation = 10000;
			config._min_allocation_size = 8;
			config._max_allocation_size = 36 * 1024;
		}
		break;
	case MEMORYTYPE_CONCURRENCY:
		{
			config._log_name = L"concurrency";
			config._allocator = func_concurrency_alloc;
			config._deallocator = func_concurrency_free;
			config._count_thread = 4;
			config._count_test = 5;
			config._count_allocation = 10000;
			config._min_allocation_size = 8;
			config._max_allocation_size = 36 * 1024;
		}
		break;
	case MEMORYTYPE_TBBMALLOC:
		{
			config._log_name = L"tbbmalloc";
			config._allocator = func_tbb_alloc;
			config._deallocator = func_tbb_free;
			config._count_thread = 4;
			config._count_test = 5;
			config._count_allocation = 10000;
			config._min_allocation_size = 8;
			config._max_allocation_size = 36 * 1024;
		}
		break;
	default:
		{
			print_help(argv[0]);
		}
		break;
	}

	if (concurrent)
	{
		config._method_type = NKTester::TestConfig::METHODTYPE_CONCURRENT;
	}
	else
	{
		config._method_type = NKTester::TestConfig::METHODTYPE_SEQUENTIALLY;
	}

	NKTester::Tester t(config);
	t.run_thread();
	t.report(ofs);

	ofs.close();

	if (standalone)
	{
		pause();
		return 0;
	}

	return 0;
}


