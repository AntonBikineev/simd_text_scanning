#ifndef HW_SINGLE_TABLE_H_
#define HW_SINGLE_TABLE_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <benchmark/benchmark.h>
#include <hwy/highway.h>

#include "hw_scalar.h"

__attribute__((noinline)) void ScanStringSingleTableLookup(const char *&start,
                                                           const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto low_nibble_table = hw::Dup128VecFromValues(
      d, '\0', 0, 0, 0, 0, 0, '&', 0, 0, 0, 0, 0, '<', '\r', 0, 0);
  const auto low_nib_and_mask = hw::Set(d, 0xf);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline)) {
    auto input = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    // Get the low nibbles.
    const auto nib_lo = input & low_nib_and_mask;
    // Lookup the values in the table using the nibbles as offsets into the
    // table.
    const auto shuf_lo = hw::TableLookupBytes(low_nibble_table, nib_lo);
    // The values in the tables correspond to the interesting symbols. Just
    // compare them with the input vector.
    const auto result = hw::VecFromMask(d, shuf_lo == input);

    return hw::FindFirstTrue(d, result != hw::Zero(d));
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

static void BenchmarkSingleTable(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringSingleTableLookup(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkSingleTable)->Arg(16)->Arg(1024)->Arg(1 << 20);

__attribute__((always_inline)) void
ScanStringSingleTableLookupAlwaysInline(const char *&start, const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto low_nibble_table = hw::Dup128VecFromValues(
      d, '\0', 0, 0, 0, 0, 0, '&', 0, 0, 0, 0, 0, '<', '\r', 0, 0);
  const auto low_nib_and_mask = hw::Set(d, 0xf);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline)) {
    auto input = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    // Get the low nibbles.
    const auto nib_lo = input & low_nib_and_mask;
    // Lookup the values in the table using the nibbles as offsets into the
    // table.
    const auto shuf_lo = hw::TableLookupBytes(low_nibble_table, nib_lo);
    // The values in the tables correspond to the interesting symbols. Just
    // compare them with the input vector.
    const auto result = hw::VecFromMask(d, shuf_lo == input);

    return hw::FindFirstTrue(d, result != hw::Zero(d));
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

#endif // HW_SINGLE_TABLE_H_
