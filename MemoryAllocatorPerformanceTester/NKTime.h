#pragma once

#include <chrono>

namespace NK
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;

	template<class func>
	__int64 time_elapsed_function(func &&f)
	{
		high_resolution_clock::time_point start_tick = high_resolution_clock::now();
		f();
		high_resolution_clock::time_point end_tick = high_resolution_clock::now();
		return duration_cast<microseconds>(end_tick - start_tick).count();
	}
};