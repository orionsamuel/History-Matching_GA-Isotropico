#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <random>
#include <iomanip>
#include <vector>
#include <fstream>
#include "utils.hpp"

using namespace std;

class functions{
    public:

    void simulation(int idIteration);
    double rand_double(double min, double max);
    double max(double num1, double num2);
    double min(double num1, double num2);
    const vector<string> split(const string& s, const char& c);

    static bool compare(const individual n, const individual m){
        return n.error_rank < m.error_rank;
    }

};