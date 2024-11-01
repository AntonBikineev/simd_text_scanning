#ifndef HW_SINGLE_TABLE_BETTER_UTILIZED2_H_
#define HW_SINGLE_TABLE_BETTER_UTILIZED2_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <benchmark/benchmark.h>
#include <hwy/highway.h>

#include "hw_scalar.h"
#include "hw_single_table.h"

__attribute__((noinline)) void
ScanStringSingleTableLookupBetterUtilized2Regs(const char *&start, const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = 2 * hw::MaxLanes(d);

  if (std::distance(start, end) < 16) {
    ScanTextScalar(start, end);
    return;
  }

  if (std::distance(start, end) < stride) {
    ScanStringSingleTableLookupAlwaysInline(start, end);
    return;
  }

  const auto low_nibble_table = hw::Dup128VecFromValues(
      d, '\0', 0, 0, 0, 0, 0, '&', 0, 0, 0, 0, 0, '<', '\r', 0, 0);
  const auto low_nib_and_mask = hw::Set(d, 0xf);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline))->int {
    auto input1 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    auto input2 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 16));
    // Get the low nibbles.
    const auto nib_lo1 = input1 & low_nib_and_mask;
    const auto nib_lo2 = input2 & low_nib_and_mask;
    // Lookup the values in the table using the nibbles as offsets into the
    // table.
    const auto shuf_lo1 = hw::TableLookupBytes(low_nibble_table, nib_lo1);
    const auto shuf_lo2 = hw::TableLookupBytes(low_nibble_table, nib_lo2);
    // The values in the tables correspond to the interesting symbols. Just
    // compare them with the input vector.
    const auto result1 = shuf_lo1 == input1;
    const auto result2 = shuf_lo2 == input2;

    const auto result_combined =
        hw::VecFromMask(d, result1) | hw::VecFromMask(d, result2);

    if (!hw::AllFalse(d, hw::MaskFromVec(result_combined))) {
      if (const auto index = hw::FindFirstTrue(d, result1); index != 1) {
        return index;
      }
      return hw::FindKnownFirstTrue(d, result2) + 16;
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

static void BenchmarkSingleTableBetterUtilized2Regs(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringSingleTableLookupBetterUtilized2Regs(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkSingleTableBetterUtilized2Regs)->Arg(16)->Arg(1024)->Arg(1 << 20);

#endif // HW_SINGLE_TABLE_BETTER_UTILIZED2_H_
