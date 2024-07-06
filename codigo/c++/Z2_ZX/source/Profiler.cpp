#include "Profiler.hpp"

//Cuando se invoque al constructor con el nombre de un ambito
Profiler::Profiler( const char* name )
{
    
    //Ponemos el nombre de este ambito
    this->scopeName = std::string( name );

    //Tomamos la primera medida de tiempo
    this->start_point = std::chrono::high_resolution_clock::now();
}

//Cuando se invoque al destructor, se añadirá la medida de tiempo a su ambito correspondiente
Profiler::~Profiler()
{
    //Esta es la cantidad de nanosegundos medida
    uint64_t ns = 0;

    //Tomamos el último punto de medida de tiempo
    std::chrono::high_resolution_clock::time_point end_point = std::chrono::high_resolution_clock::now();

    //Calculamos la diferencia de tiempos entre el comienzo y el final
    ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_point - this->start_point).count();

    //Buscamos en el mapa de rendimiento el ambito en el que estamos
    std::map<std::string, std::vector<uint64_t>>::iterator currentScope = performanceMap.find(this->scopeName);

    //Si el ambito no existe
    if(currentScope == performanceMap.end()) {

        //Declaramos un vector de tiempos medidos
        std::vector<uint64_t> meassured_times;

        //Generamos un par con el ambito y el vector
        std::pair<std::string, std::vector<uint64_t>> newScope(this->scopeName, meassured_times);

        //Lo añadimos al mapa de rendimiento y obtenemos de paso su posición en el mismo
        currentScope = performanceMap.insert(newScope).first;
    }

    //Añadimos el tiempo medido al mapa de rendimiento
    currentScope->second.push_back(ns);

}

// Generamos el reporte de rendimiento
Profiler::Profiler()
{

    //Para cada elemento del mapa de rendimiento
    for(std::map <std::string, std::vector<uint64_t>>::iterator iter = performanceMap.begin(); iter != performanceMap.end(); ++iter){

        //Sumamos todos los tiempos del vector
        uint64_t tiempo_total = std::accumulate(iter->second.begin(), iter->second.end(), (uint64_t)0);
        uint64_t llamadas = iter->second.size();

        //Calculamos la media
        double media = (double)tiempo_total/llamadas;

        //Calculamos la desviacion tipica con funciones lambda
        double standard_deviation = std::sqrt((double)std::accumulate(iter->second.begin(), iter->second.end(), 0.0, 
                                                    [media] (double acumulated, uint64_t ns) -> double{ return acumulated + std::pow(((double)ns - media), 2); })/llamadas);

        
        //Escribimos el reporte de la función de forma ordenada
        std::cout << std::setw(20) << iter->first << " ----- Llamadas ----- Media (ns) ----- Desviacion Tipica (ns)" << std::endl;
        std::cout << std::setw(27) << "" << std::setw(8) << std::to_string(llamadas) << std::setw(17) << std::setprecision(3) << std::to_string(media) << std::setw(29) << std::setprecision(3) << std::to_string(standard_deviation) << std::endl << std::endl;
    }
}