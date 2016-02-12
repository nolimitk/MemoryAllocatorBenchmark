
#include <iostream>
using namespace std;

#include "NKTester.hpp"

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

void main(void)
{
	wcout << L"Memory allocator performance tester" << endl;
	wcout << L"01.30.2016 nolimitk GPL" << endl;

	srand(static_cast<unsigned int>(time(nullptr)));

	//NKTester::Tester t(malloc, free);
	//NKTester::Tester t(func_malloc, func_free);
	NKTester::Tester t(func_new, func_delete);
	t.run_thread();

	wchar_t pause;
	wcin >> pause;
}


