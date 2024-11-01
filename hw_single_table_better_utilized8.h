#ifndef HW_SINGLE_TABLE_BETTER_UTILIZED_H_
#define HW_SINGLE_TABLE_BETTER_UTILIZED_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <benchmark/benchmark.h>
#include <hwy/highway.h>

#include "hw_scalar.h"

__attribute__((noinline)) void
ScanStringSingleTableLookupBetterUtilized8(const char *&start,
                                           const char *end) {
  using LaneType = std::make_unsigned_t<char>;
  hw::FixedTag<LaneType, 16> d;
  static constexpr auto stride = 8 * hw::MaxLanes(d);

  if (std::distance(start, end) < stride) {
    ScanTextScalar(start, end);
    return;
  }

  const auto low_nibble_table = hw::Dup128VecFromValues(
      d, '\0', 0, 0, 0, 0, 0, '&', 0, 0, 0, 0, 0, '<', '\r', 0, 0);
  const auto low_nib_and_mask = hw::Set(d, 0xf);

  auto match = [&](auto *start) HWY_ATTR __attribute__((always_inline))->int {
    auto input1 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start));
    auto input2 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 16));
    auto input3 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 32));
    auto input4 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 48));
    auto input5 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 64));
    auto input6 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 80));
    auto input7 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 96));
    auto input8 = hw::LoadU(d, reinterpret_cast<const LaneType *>(start + 112));
    // Get the low nibbles.
    const auto nib_lo1 = input1 & low_nib_and_mask;
    const auto nib_lo2 = input2 & low_nib_and_mask;
    const auto nib_lo3 = input3 & low_nib_and_mask;
    const auto nib_lo4 = input4 & low_nib_and_mask;
    const auto nib_lo5 = input5 & low_nib_and_mask;
    const auto nib_lo6 = input6 & low_nib_and_mask;
    const auto nib_lo7 = input7 & low_nib_and_mask;
    const auto nib_lo8 = input8 & low_nib_and_mask;
    // Lookup the values in the table using the nibbles as offsets into the
    // table.
    const auto shuf_lo1 = hw::TableLookupBytes(low_nibble_table, nib_lo1);
    const auto shuf_lo2 = hw::TableLookupBytes(low_nibble_table, nib_lo2);
    const auto shuf_lo3 = hw::TableLookupBytes(low_nibble_table, nib_lo3);
    const auto shuf_lo4 = hw::TableLookupBytes(low_nibble_table, nib_lo4);
    const auto shuf_lo5 = hw::TableLookupBytes(low_nibble_table, nib_lo5);
    const auto shuf_lo6 = hw::TableLookupBytes(low_nibble_table, nib_lo6);
    const auto shuf_lo7 = hw::TableLookupBytes(low_nibble_table, nib_lo7);
    const auto shuf_lo8 = hw::TableLookupBytes(low_nibble_table, nib_lo8);
    // The values in the tables correspond to the interesting symbols. Just
    // compare them with the input vector.
    const auto result1 = shuf_lo1 == input1;
    const auto result2 = shuf_lo2 == input2;
    const auto result3 = shuf_lo3 == input3;
    const auto result4 = shuf_lo4 == input4;
    const auto result5 = shuf_lo5 == input5;
    const auto result6 = shuf_lo6 == input6;
    const auto result7 = shuf_lo7 == input7;
    const auto result8 = shuf_lo8 == input8;

    const auto result_combined =
        hw::VecFromMask(d, result1) | hw::VecFromMask(d, result2) |
        hw::VecFromMask(d, result3) | hw::VecFromMask(d, result4) |
        hw::VecFromMask(d, result5) | hw::VecFromMask(d, result6) |
        hw::VecFromMask(d, result7) | hw::VecFromMask(d, result8);

    if (!hw::AllFalse(d, hw::MaskFromVec(result_combined))) {
      if (const auto index = hw::FindFirstTrue(d, result1); index != 1) {
        return index;
      } else if (const auto index = hw::FindFirstTrue(d, result2); index != 1) {
        return index + 16;
      } else if (const auto index = hw::FindFirstTrue(d, result3); index != 1) {
        return index + 32;
      } else if (const auto index = hw::FindFirstTrue(d, result4); index != 1) {
        return index + 48;
      } else if (const auto index = hw::FindFirstTrue(d, result5); index != 1) {
        return index + 64;
      } else if (const auto index = hw::FindFirstTrue(d, result6); index != 1) {
        return index + 80;
      } else if (const auto index = hw::FindFirstTrue(d, result7); index != 1) {
        return index + 96;
      }
      return hw::FindKnownFirstTrue(d, result4) + 112;
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

static void BenchmarkSingleTableBetterUtilized8(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanStringSingleTableLookupBetterUtilized8(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkSingleTableBetterUtilized8)
    ->Arg(16)
    ->Arg(1024)
    ->Arg(1 << 20);

#endif // HW_SINGLE_TABLE_BETTER_UTILIZED_H_
