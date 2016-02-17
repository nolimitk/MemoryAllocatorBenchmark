@echo off
echo MMemory Allocator Benchmark
echo 02.13.2016 nolimitk
echo GPL

echo ...
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark malloc
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark new
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark concurrency
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark tbb
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark tc
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark malloc -concurrent
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark new -concurrent
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark concurrency -concurrent
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark tbb -concurrent
for /L %%G IN (1,1,10 ) do MemoryAllocatorBenchmark tc -concurrent

echo done.
pause