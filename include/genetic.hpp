#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <random>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include "functions.hpp"

using namespace std;

class genetic_algorithm: public functions{
    private:
    vector<individual> population;
    vector<individual> dataset;
    result* real_results;
    individual children[((SIZE_POPULATION * CROSSOVER_RATE) / 100)];
    int crossover_rate = ((SIZE_POPULATION * CROSSOVER_RATE) / 100);
    int mutation_rate = ((((SIZE_POPULATION * CROSSOVER_RATE) / 100) * MUTATION_RATE) / 100);
    double min_permeabilityD[2] = {0,0};
    double max_permeabilityD[2] = {0,0};
    int min_permeabilityI[2] = {0,0};
    int max_permeabilityI[2] = {0,0};
    string decimal[2];

    public:
    genetic_algorithm();
    ~genetic_algorithm();

    void init();
    void firstPopulation();
    void otherPopulations(int idIteration);
    void fitness(int idIteration);
    void crossover();
    void mutation(); 
};
