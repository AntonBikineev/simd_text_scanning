#ifndef HW_TABLES_H_
#define HW_TABLES_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <benchmark/benchmark.h>
#include <hwy/highway.h>

#include "hw_scalar.h"

__attribute__((noinline)) void ScanStringTwoTablesLookup(const char *&start,
                                                         const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto low_nibble_mask = hw::Dup128VecFromValues(
      d, 0b0001, 0, 0, 0, 0, 0, 0b0100, 0, 0, 0, 0, 0, 0b0010, 0b1000, 0, 0);
  const auto high_nibble_mask = hw::Dup128VecFromValues(
      d, 0b1001, 0, 0b0100, 0b0010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  const auto low_nib_and_mask = hw::Set(d, 0xf);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline)) {
    auto input = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    const auto nib_lo = input & low_nib_and_mask;
    const auto nib_hi = hw::ShiftRight<4>(input);

    const auto shuf_lo = hw::TableLookupBytes(low_nibble_mask, nib_lo);
    const auto shuf_hi = hw::TableLookupBytes(high_nibble_mask, nib_hi);

    const auto classified = shuf_lo & shuf_hi;

    return hw::FindFirstTrue(d, classified != hw::Zero(d));
  };

  for (; start + (stride - 1) < end; start += stride) {
    const intptr_t result = match(start);
    if (result != -1) {
      start += result;
      return;
    }
  }
  // Tail.
  if (start < end) {
    const intptr_t result = match(end - stride);
    if (result != -1) {
      start = end - stride + result;
      return;
    }
    start = end;
  }
}

static void BenchmarkTables(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringTwoTablesLookup(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkTables)->Arg(16)->Arg(1024)->Arg(1 << 20);

#endif // HW_TABLES_H_
