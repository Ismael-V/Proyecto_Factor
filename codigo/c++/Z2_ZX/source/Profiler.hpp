#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <cinttypes>
#include <string>
#include <chrono>
#include <numeric>
#include <cmath>

//Este es el mapa que contiene todos los ambitos que han sido corridos durante el programa
static std::map <std::string, std::vector<uint64_t>> performanceMap;

class Profiler {
    private:

    //Este es el nombre del ambito
    std::string scopeName;

    //Esta es la primera medida de tiempo tomada
    std::chrono::high_resolution_clock::time_point start_point;

    public:
  
    //Cuando se invoque al constructor con el nombre de un ambito
    Profiler( const char* name );

    //Cuando se invoque al destructor, se añadirá la medida de tiempo a su ambito correspondiente
    ~Profiler();

    //El constructor vacio genera un reporte
    Profiler();

};

#endif