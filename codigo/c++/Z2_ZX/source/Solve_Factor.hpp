#ifndef SOLVE_FACTOR_HPP
#define SOLVE_FACTOR_HPP

#include <gmp.h>
#include "Z2_linear_solver.hpp"

#define U_TYPE uint64_t

//Pre: True
//Post: Devuelve, de proveerle del grado target adecuado uno de los factores del numero, de encontrarlo
void solve_factor(std::vector<Z2_poly<U_TYPE>>& factores, uint32_t grado_target, const mpz_t N, mpz_t& factor);

//Pre: True
//Post: Devuelve uno de los factores del numero, de encontrarlo
void solve_factor_blind(std::vector<Z2_poly<U_TYPE>>& factores, const mpz_t N, mpz_t& factor);

#endif