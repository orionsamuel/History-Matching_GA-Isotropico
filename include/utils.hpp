struct individual{
    double porosity[5];
    double permeability_1[5];
    double permeability_2[5];
    double permeability_3[5];
    double error_rank;
};

struct result{
    double water;
    double oil;
};

#define SIZE_POPULATION 100
#define N_GENERATIONS 10

#define CROSSOVER_RATE 80
#define MUTATION_RATE 50

#define MIN_POROSITY 1.0000000E-01
#define MAX_POROSITY 3.0000000E-01

#define WATER_WEIGHT 0.6
#define OIL_WEIGHT 0.4

#define N_METRICS 2
