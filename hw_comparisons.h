#ifndef HW_COMPARISONS_H_
#define HW_COMPARISONS_H_

#include <type_traits>

#include <benchmark/benchmark.h>
#include <hwy/highway.h>

#include "hw_scalar.h"

#if defined(SIMD_COMPARISONS_REDUCE_MIN)
__attribute__((noinline)) void
ScanStringComparisonsReduceWithMin(const char *&start, const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto quote_mask = hw::Set(d, static_cast<LaneType>('<'));
  const auto escape_mask = hw::Set(d, static_cast<LaneType>('&'));
  const auto newline_mask = hw::Set(d, static_cast<LaneType>('\r'));
  const auto zero_mask = hw::Set(d, static_cast<LaneType>('\0'));

  const auto bit_mask = hw::Dup128VecFromValues(d, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                                10, 11, 12, 13, 14, 15);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline))->long {
    auto input = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    auto quotes = input == quote_mask;
    auto escapes = input == escape_mask;
    auto newlines = input == newline_mask;
    auto zeros = input == zero_mask;
    auto mask = hw::VecFromMask(d, zeros) | hw::VecFromMask(d, quotes) |
                hw::VecFromMask(d, escapes) | hw::VecFromMask(d, newlines);

    auto matches = vornq_u8(bit_mask.raw, mask.raw);
    uint64_t min = vminvq_u8(matches);
    if (min != 0xff) {
      return min;
    }
    return -1;
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
#endif // defined(SIMD_COMPARISONS_REDUCE_MIN)

#if defined(SIMD_COMPARISONS_REDUCE_SHRN)
__attribute__((noinline)) void
ScanStringComparisonsReduceWithShrn(const char *&start, const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto quote_mask = hw::Set(d, static_cast<LaneType>('<'));
  const auto escape_mask = hw::Set(d, static_cast<LaneType>('&'));
  const auto newline_mask = hw::Set(d, static_cast<LaneType>('\r'));
  const auto zero_mask = hw::Set(d, static_cast<LaneType>('\0'));

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline)) {
    auto input = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    auto quotes = input == quote_mask;
    auto escapes = input == escape_mask;
    auto newlines = input == newline_mask;
    auto zeros = input == zero_mask;
    auto mask = hw::VecFromMask(d, zeros) | hw::VecFromMask(d, quotes) |
                hw::VecFromMask(d, escapes) | hw::VecFromMask(d, newlines);
    return hw::FindFirstTrue(d, hw::MaskFromVec(mask));
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
#endif // defined(SIMD_COMPARISONS_REDUCE_SHRN)

#if defined(SIMD_COMPARISONS_REDUCE_MIN)
static void BenchmarkComparisonsReduceWithMin(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringComparisonsReduceWithMin(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkComparisonsReduceWithMin)->Arg(16)->Arg(1024)->Arg(1 << 20);
#endif // defined(SIMD_COMPARISONS_REDUCE_MIN)

#if defined(SIMD_COMPARISONS_REDUCE_SHRN)
static void BenchmarkComparisonsReduceWithShrn(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringComparisonsReduceWithShrn(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkComparisonsReduceWithShrn)->Arg(16)->Arg(1024)->Arg(1 << 20);
#endif // defined (SIMD_COMPARISONS_REDUCE_SHRN)

#endif // HW_COMPARISONS_H_
