# Memory Allocator Benchmark

* You can measure the performance of variety memory allocators.
 * Windows LFH(malloc/free,new/delete)
 * Microsoft Concurrency runtime - https://msdn.microsoft.com/en-us/library/ff829272.aspx
 * Intel TBB malloc - https://www.threadingbuildingblocks.org/
 * Google tcmalloc - https://github.com/gperftools/gperftools

* You can test your custom or another memory allocator easily.
* High-Performance multithreading applications( like a server application ) needs high-performance memory allocator.

### Usage

* open MemoryAllocatorBenchmark.sln
* build with x64 platform and Release.
* execute just batch file for benchmark.

$ bin\MemoryAllocatorBenchmark.bat

### Benchmark

* Because of memory allocators may has a memory pool, it is so different between first allocations and second allocations.
 * it tests to execute processes several times.
 * it tests several times allocations to measure the differences between first allocation and second allocation.
 
* Defaults
 * Variable block size : 8bytes ~ 32K bytes
 * Mulith-thread : 4 threads
 * count of allocations/deallocations : 10000
 * method of allocation : run sequentially, run concurrently

### Environment
* Windows
* C++
* Visual Studio 2015

### Extras

* it uses high_resolution_clock of C++11 to measure the performance.

### Enhancement Requests
* support more memory allocators
* support more Operating Systems
* support more IDEs
