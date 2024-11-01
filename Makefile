all: bench_comparisons bench_comparisons_shrn bench_tables bench_single_table bench_single_table_2regs bench_single_table_4regs bench_all

clean:
	rm -f ./bench_comparisons ./bench_comparisons_shrn ./bench_tables ./bench_single_table ./bench_single_table_2regs ./bench_single_table_4regs ./bench_all

bench_comparisons : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_MIN

bench_comparisons_shrn : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_MIN -DSIMD_COMPARISONS_REDUCE_SHRN

bench_tables : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h hw_tables.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_SHRN -DSIMD_TABLES

bench_single_table : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h hw_tables.h hw_single_table.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_SHRN -DSIMD_TABLES -DSIMD_SINGLE_TABLE

bench_single_table_2regs : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h hw_tables.h hw_single_table.h hw_single_table_better_utilized2.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_SHRN -DSIMD_TABLES -DSIMD_SINGLE_TABLE -DSIMD_SINGLE_TABLE_BETTER_UTILIZED2

bench_single_table_4regs : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h hw_tables.h hw_single_table.h hw_single_table_better_utilized4.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_SHRN -DSIMD_TABLES -DSIMD_SINGLE_TABLE -DSIMD_SINGLE_TABLE_BETTER_UTILIZED2 -DSIMD_SINGLE_TABLE_BETTER_UTILIZED4

bench_all : hw.cc hw_string.cc hw_scalar.h hw_comparisons.h hw_tables.h hw_single_table.h hw_single_table_better_utilized4.h
	g++ -O3 -std=c++20 -Wall -o $@ hw.cc hw_string.cc -I. -lbenchmark -DSCALAR -DSIMD_COMPARISONS -DSIMD_COMPARISONS_REDUCE_MIN -DSIMD_COMPARISONS_REDUCE_SHRN -DSIMD_TABLES -DSIMD_SINGLE_TABLE -DSIMD_SINGLE_TABLE_BETTER_UTILIZED2 -DSIMD_SINGLE_TABLE_BETTER_UTILIZED4
