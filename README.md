Benchmark comparing various vectorized versions of scanning a string.

```
----------------------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations UserCounters...
----------------------------------------------------------------------------------------------------------
BenchmarkScalar/16                                    10.4 ns         10.3 ns     67750022 bytes_per_second=1.44434G/s
BenchmarkScalar/1024                                   488 ns          481 ns      1459014 bytes_per_second=1.98133G/s
BenchmarkScalar/1048576                             486043 ns       480865 ns         1441 bytes_per_second=2.03084G/s
BenchmarkComparisonsReduceWithMin/16                  1.90 ns         1.88 ns    371701810 bytes_per_second=7.92384G/s
BenchmarkComparisonsReduceWithMin/1024                57.5 ns         56.5 ns     12285012 bytes_per_second=16.8891G/s
BenchmarkComparisonsReduceWithMin/1048576            57168 ns        56532 ns        12469 bytes_per_second=17.2746G/s
BenchmarkComparisonsReduceWithShrn/16                 1.68 ns         1.67 ns    411237354 bytes_per_second=8.9224G/s
BenchmarkComparisonsReduceWithShrn/1024               46.4 ns         45.9 ns     15299577 bytes_per_second=20.7997G/s
BenchmarkComparisonsReduceWithShrn/1048576           46865 ns        46288 ns        15115 bytes_per_second=21.0974G/s
BenchmarkTables/16                                    1.69 ns         1.67 ns    414387535 bytes_per_second=8.90071G/s
BenchmarkTables/1024                                  40.1 ns         39.5 ns     18058178 bytes_per_second=24.1667G/s
BenchmarkTables/1048576                              39708 ns        38956 ns        18034 bytes_per_second=25.0683G/s
BenchmarkSingleTable/16                               1.60 ns         1.58 ns    439731638 bytes_per_second=9.45258G/s
BenchmarkSingleTable/1024                             39.4 ns         39.0 ns     18210292 bytes_per_second=24.4608G/s
BenchmarkSingleTable/1048576                         33688 ns        33359 ns        20934 bytes_per_second=29.2742G/s
BenchmarkSingleTableBetterUtilized2Regs/16            1.59 ns         1.57 ns    444718334 bytes_per_second=9.50039G/s
BenchmarkSingleTableBetterUtilized2Regs/1024          25.1 ns         24.8 ns     28307304 bytes_per_second=38.4617G/s
BenchmarkSingleTableBetterUtilized2Regs/1048576      23453 ns        23193 ns        30139 bytes_per_second=42.1063G/s
BenchmarkSingleTableBetterUtilized4Regs/16            1.58 ns         1.57 ns    448258197 bytes_per_second=9.49717G/s
BenchmarkSingleTableBetterUtilized4Regs/1024          22.6 ns         22.3 ns     31422543 bytes_per_second=42.85G/s
BenchmarkSingleTableBetterUtilized4Regs/1048576      22135 ns        21916 ns        32114 bytes_per_second=44.5597G/s
```
