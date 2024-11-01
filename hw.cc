#include <benchmark/benchmark.h>
#include <hwy/highway.h>
#include <iostream>
#include <type_traits>

namespace hw = hwy::HWY_NAMESPACE;

std::string generate_alpha_num_string(size_t len);

#if defined(SCALAR)
#include "hw_scalar.h"
#endif // defined(SCALAR)

#if defined(SIMD_COMPARISONS)
#include "hw_comparisons.h"
#endif // defined(SIMD_COMPARISONS)

#if defined(SIMD_TABLES)
#include "hw_tables.h"
#endif // defined(SIMD_TABLES)

#if defined(SIMD_SINGLE_TABLE)
#include "hw_single_table.h"
#endif // defined(SIMD_SINGLE_TABLE)

#if defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED2)
#include "hw_single_table_better_utilized2.h"
#endif // defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED)

#if defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED4)
#include "hw_single_table_better_utilized4.h"
#endif // defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED)

#if defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED8)
#include "hw_single_table_better_utilized8.h"
#endif // defined(SIMD_SINGLE_TABLE_BETTER_UTILIZED8)

BENCHMARK_MAIN();
