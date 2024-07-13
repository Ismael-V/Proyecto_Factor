
opt_flags= -march=native -O3

hamming_equality_attack: hamming_equality_attack.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 hamming_equality_attack.cpp objects/Solve_Factor.o objects/Z2_polynomial.o objects/Z2_guided_decarrier.o objects/Profiler.o -lgmp -o bin/hamming_equality_attack

prueba_g_dec: prueba_g_dec.cpp objects/Z2_guided_decarrier.o
	g++ $(opt_flags) -std=c++20 prueba_g_dec.cpp objects/Z2_guided_decarrier.o -o bin/prueba_g_dec

generic_method: generic_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_mpi_decarrier.o objects/Profiler.o
	g++ $(opt_flags) -std=c++20 generic_method.cpp objects/Solve_Factor.o  objects/Z2_polynomial.o objects/Z2_guided_mpi_decarrier.o objects/Profiler.o -lgmp -o bin/generic_method

objects/Solve_Factor.o: source/Solve_Factor.cpp 
	g++ $(opt_flags) -std=c++20 -c source/Solve_Factor.cpp -o objects/Solve_Factor.o

objects/Z2_polynomial.o: source/Z2_polynomial.cpp
	g++ $(opt_flags) -std=c++20 -c source/Z2_polynomial.cpp -o objects/Z2_polynomial.o

objects/Z2_guided_decarrier.o: source/Z2_guided_decarrier.cpp
	g++ $(opt_flags) -std=c++20 -c source/Z2_guided_decarrier.cpp -o objects/Z2_guided_decarrier.o

objects/Z2_guided_mpi_decarrier.o: source/Z2_guided_mpi_decarrier.cpp
	g++ $(opt_flags) -std=c++20 -c source/Z2_guided_mpi_decarrier.cpp -o objects/Z2_guided_mpi_decarrier.o

objects/Profiler.o: source/Profiler.cpp
	g++ $(opt_flags) -std=c++20 -c source/Profiler.cpp -o objects/Profiler.o

clean: 
	rm objects/*