
opt_flags= -pedantic -Wall -Wextra -O3

hamming_equality_attack: hamming_equality_attack.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 hamming_equality_attack.cpp objects/Solve_Factor.o objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o -lgmp -o bin/hamming_equality_attack

generic_method: generic_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 generic_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o -lgmp -o bin/generic_method

specific_method: specific_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 specific_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o -lgmp -o bin/specific_method

prueba_g_dec: prueba_g_dec.cpp objects/Z2_guided_decarrier.o
	g++ $(opt_flags) -std=c++20 prueba_g_dec.cpp objects/Z2_guided_decarrier.o -o bin/prueba_g_dec

objects/Solve_Factor.o: source/Solve_Factor.cpp 
	g++ $(opt_flags) -std=c++20 -c source/Solve_Factor.cpp -o objects/Solve_Factor.o

objects/Z2_polynomial.o: source/Z2_polynomial.cpp
	g++ $(opt_flags) -std=c++20 -c source/Z2_polynomial.cpp -o objects/Z2_polynomial.o

objects/Z2_guided_decarrier.o: source/Z2_guided_decarrier.cpp
	g++ $(opt_flags) -std=c++20 -c source/Z2_guided_decarrier.cpp -o objects/Z2_guided_decarrier.o

objects/Profiler.o: source/Profiler.cpp
	g++ $(opt_flags) -std=c++20 -c source/Profiler.cpp -o objects/Profiler.o

generic_method_mpi: generic_method_mpi.cpp objects/Solve_Factor_mpi.o  objects/Z2_polynomial_mpi.o objects/Z2_guided_mpi_decarrier.o objects/Profiler_mpi.o
	mpic++ $(opt_flags) -std=c++20 generic_method_mpi.cpp objects/Solve_Factor_mpi.o  objects/Z2_polynomial_mpi.o objects/Z2_guided_mpi_decarrier.o objects/Profiler_mpi.o -lgmp -o bin/generic_method_mpi

objects/Solve_Factor_mpi.o: source/Solve_Factor.cpp 
	mpic++ $(opt_flags) -std=c++20 -c source/Solve_Factor.cpp -o objects/Solve_Factor_mpi.o

objects/Z2_polynomial_mpi.o: source/Z2_polynomial.cpp
	mpic++ $(opt_flags) -std=c++20 -c source/Z2_polynomial.cpp -o objects/Z2_polynomial_mpi.o

objects/Z2_guided_mpi_decarrier.o: source/Z2_guided_mpi_decarrier.cpp
	mpic++ $(opt_flags) -std=c++20 -c source/Z2_guided_mpi_decarrier.cpp -o objects/Z2_guided_mpi_decarrier.o

objects/Profiler_mpi.o: source/Profiler.cpp
	mpic++ $(opt_flags) -std=c++20 -c source/Profiler.cpp -o objects/Profiler_mpi.o

factorizacion_impar_mpi: factorizacion_impar_mpi.cpp
	mpic++ $(opt_flags) -std=c++20 factorizacion_impar_mpi.cpp -o bin/factorizacion_impar_mpi -lgmp

generic_method_many_trials: generic_method_many_trials.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 generic_method_many_trials.cpp objects/Solve_Factor.o objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o -lgmp -o bin/generic_method_many_trials

weighted_semiprime_gen: weighted_semiprime_gen.cpp
	 g++ $(opt_flags) -std=c++20 weighted_semiprime_gen.cpp -o bin/weighted_semiprime_gen

clean: 
	rm objects/*
