# Memory Allocator Performance Tester

* You can measure the performance of variety memory allocators.
 * Windows LFH(malloc/free,new/delete)
 * Microsoft Concurrency runtime - https://msdn.microsoft.com/en-us/library/ff829272.aspx
 * Intel TBB malloc - https://www.threadingbuildingblocks.org/
 * Google tcmalloc - https://github.com/gperftools/gperftools

* You can test your custom or another memory allocator easily.

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

* To measure clock it uses high_resolution_clock of C++11.

### Enhancement Requests
* support more memory allocators
* support more Operating Systems
* support more IDEs
