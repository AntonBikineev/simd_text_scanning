#ifndef HW_SCALAR_H_
#define HW_SCALAR_H_

#include <benchmark/benchmark.h>

__attribute__((noinline)) void ScanTextScalar(const char *&start,
                                              const char *end) {
  auto match = [&](auto *start) __attribute__((always_inline)) {
    return *start == '<' || *start == '&' || *start == '\r' || *start == '\0';
  };

  for (; start < end; ++start) {
    if (match(start)) {
      return;
    }
  }
}

static void BenchmarkScalar(benchmark::State &state) {
  const size_t len = state.range(0);
  auto string = generate_alpha_num_string(len);
  for (auto _ : state) {
    const char *start = string.data();
    ScanTextScalar(start, start + len);
    benchmark::DoNotOptimize(start);
  }
  state.SetBytesProcessed(int64_t(state.iterations()) *
                          int64_t(state.range(0)));
}
BENCHMARK(BenchmarkScalar)->Arg(16)->Arg(1024)->Arg(1 << 20);

#endif // HW_SCALAR_H_
