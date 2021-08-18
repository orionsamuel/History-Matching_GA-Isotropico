#include "../include/genetic.hpp"

genetic_algorithm::genetic_algorithm()
{
    srand(time(0));
}

genetic_algorithm::~genetic_algorithm()
{
}

void genetic_algorithm::init()
{
    DIR *dp = opendir("../Output/");

    if (dp == NULL)
    {
        system("mkdir ../Output/");
    }
    else
    {
        system("rm -r -f ../Output/*");
    }

    ifstream result_water("../Input/resultadoVazaoAgua.dat", ios::in);
    ifstream result_oil("../Input/resultadoVazaoOleo.dat", ios::in);

    string line, line2, content, content2;

    while (!result_water.eof() && !result_oil.eof())
    {
        getline(result_water, line);
        getline(result_oil, line2);
        content += line;
        content += " ";
        content2 += line2;
        content2 += " ";
    }

    result_water.close();
    result_oil.close();

    vector<string> v{split(content, ' ')};
    vector<string> v2{split(content2, ' ')};

    real_results = new result[v.size()];

    for (int i = 0; i < v.size(); i++)
    {
        this->real_results[i].water = stod(v[i]);
        this->real_results[i].oil = stod(v2[i]);
    }

    firstPopulation();
    int count = 1;
    while (count < N_GENERATIONS)
    {
        otherPopulations(count);
        count++;
    }
}

void genetic_algorithm::firstPopulation()
{
    srand((unsigned)time(0));

    DIR *dp = opendir("../Output/0");

    if (dp == NULL)
    {
        string command = "mkdir ../Output/0";
        string command2 = "mkdir ../Output/0/agua";
        string command3 = "mkdir ../Output/0/oleo";
        const char *file = (char *)command.c_str();
        const char *file2 = (char *)command2.c_str();
        const char *file3 = (char *)command3.c_str();
        system(file);
        system(file2);
        system(file3);
    }
    else
    {
        string command = "rm -f ../Output/0/*";
        const char *file = (char *)command.c_str();
        system(file);
    }

    //Leitura do dataset e criação do arquivo da população 0
    ifstream read_input("../Dataset/history_matching.csv", ios::in);
    ofstream write_output("../Output/0/0.csv", ios::out);

    string line;

    //Passando os valores contidos no arquivo para um vector do tipo individuo
    int count = 0;
    while (!read_input.eof())
    {
        getline(read_input, line);
        if (count == 0)
        {
            write_output << line << endl;
        }
        else if (count < 301)
        {
            vector<string> v{split(line, ',')};
            dataset.push_back({{stod(v[0]), stod(v[4]), stod(v[8]), stod(v[12]), stod(v[16])},
                               {stod(v[1]), stod(v[5]), stod(v[9]), stod(v[13]), stod(v[17])},
                               {stod(v[2]), stod(v[6]), stod(v[10]), stod(v[14]), stod(v[18])},
                               {stod(v[3]), stod(v[7]), stod(v[11]), stod(v[15]), stod(v[19])},
                               stod(v[20])});
        }
        count++;
    }

    read_input.close();

    sort(begin(dataset), end(dataset), compare);

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        write_output << scientific << dataset[i].porosity[0] << ","
                     << dataset[i].permeability_1[0] << "," << dataset[i].permeability_2[0]
                     << "," << dataset[i].permeability_3[0] << "," << dataset[i].porosity[1]
                     << "," << dataset[i].permeability_1[1] << "," << dataset[i].permeability_2[1]
                     << "," << dataset[i].permeability_3[1] << "," << dataset[i].porosity[2] << ","
                     << dataset[i].permeability_1[2] << "," << dataset[i].permeability_2[2]
                     << "," << dataset[i].permeability_3[2] << "," << dataset[i].porosity[3] << ","
                     << dataset[i].permeability_1[3] << "," << dataset[i].permeability_2[3]
                     << "," << dataset[i].permeability_3[3] << "," << dataset[i].porosity[4] << ","
                     << dataset[i].permeability_1[4] << "," << dataset[i].permeability_2[4]
                     << "," << dataset[i].permeability_3[4] << "," << dataset[i].error_rank << endl;
    }

    write_output.close();

    ifstream read_csv("../Output/0/0.csv", ios::in);

    line = "";

    //Pega a casa decimal da permeabilidade da menor taxa de erro, para gerar os valores mínimos e máximos de permeabilidade
    count = 0;
    while (!read_csv.eof())
    {
        getline(read_csv, line);
        if (count == 1)
        {
            vector<string> v{split(line, ',')};
            decimal[0] = v[1][10];
            decimal[0] += v[1][11];
            decimal[1] += v[9][10];
            decimal[1] += v[9][11];
            cout << endl;
        }

        count++;
    }

    read_csv.close();

    //Pegando os valores dos expoentes de cada célula para achar os valores mínimos e máximos, fazendo a conversão para mD

    string cell15 = "1e-" + decimal[0];
    string cell34 = "1e-" + decimal[1];

    double cell_15 = stod(cell15);
    double cell_34 = stod(cell34);

    cell_15 = (cell_15 * 1) / 10e-15;
    cell_34 = (cell_34 * 1) / 10e-15;

    if (stod(decimal[0]) < 14)
    {
        this->min_permeabilityI[0] = cell_15 - (cell_15 * 0.2);
        this->max_permeabilityI[0] = cell_15 + (cell_15 * 0.2);
    }
    else
    {
        this->min_permeabilityD[0] = cell_15 - (cell_15 * 0.2);
        this->max_permeabilityD[0] = cell_15 + (cell_15 * 0.2);
    }

    if (stod(decimal[1]) < 14)
    {
        this->min_permeabilityI[1] = cell_34 - (cell_34 * 0.2);
        this->max_permeabilityI[1] = cell_34 + (cell_34 * 0.2);
    }
    else
    {
        this->min_permeabilityD[1] = cell_34 - (cell_34 * 0.2);
        this->max_permeabilityD[1] = cell_34 + (cell_34 * 0.2);
    }

    system("rm ../Output/0/0.csv");

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        string command = "cp ../Input/inputDS.dat ../Output/0/inputDS_" + to_string(i) + ".dat";
        const char *file = (char *)command.c_str();
        system(file);
    }

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        population.push_back(dataset[i]);
        population[i].error_rank = 0;
    }

    //Gerando os valores randômicos da primeira população
    //Se o decimal for menor que 15, então os valores gerados serão inteiros, senão, serão doubles
    //Também arredondando os valores double para duas casas decimais
    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        this->population[i].porosity[0] = rand_double(MIN_POROSITY, MAX_POROSITY);
        this->population[i].porosity[0] = floor(this->population[i].porosity[0] * 100) / 100;
        this->population[i].porosity[1] = dataset[i].porosity[1];
        this->population[i].porosity[2] = rand_double(MIN_POROSITY, MAX_POROSITY);
        this->population[i].porosity[2] = floor(this->population[i].porosity[2] * 100) / 100;
        this->population[i].porosity[3] = rand_double(MIN_POROSITY, MAX_POROSITY);
        this->population[i].porosity[3] = floor(this->population[i].porosity[3] * 100) / 100;
        this->population[i].porosity[4] = rand_double(MIN_POROSITY, MAX_POROSITY);
        this->population[i].porosity[4] = floor(this->population[i].porosity[4] * 100) / 100;
        if (stod(decimal[0]) < 14)
        {
            int permeabilityI0 = rand() % (this->max_permeabilityI[0] - this->min_permeabilityI[0] + 1) + this->min_permeabilityI[0];
            int permeabilityI4 = rand() % (this->max_permeabilityI[0] - this->min_permeabilityI[0] + 1) + this->min_permeabilityI[0];
            this->population[i].permeability_1[0] = permeabilityI0;
            this->population[i].permeability_1[4] = permeabilityI4;
            this->population[i].permeability_2[0] = permeabilityI0;
            this->population[i].permeability_2[4] = permeabilityI4;
            this->population[i].permeability_3[0] = permeabilityI0;
            this->population[i].permeability_3[4] = permeabilityI4;
        }
        else
        {

            double permeabilityD0 = rand_double(this->min_permeabilityD[0], this->max_permeabilityD[0]);
            double permeabilityD4 = rand_double(this->min_permeabilityD[0], this->max_permeabilityD[0]);
            this->population[i].permeability_1[0] = permeabilityD0;
            this->population[i].permeability_1[0] = floor(this->population[i].permeability_1[0] * 100) / 100;
            this->population[i].permeability_1[4] = permeabilityD4;
            this->population[i].permeability_1[4] = floor(this->population[i].permeability_1[4] * 100) / 100;
            this->population[i].permeability_2[0] = permeabilityD0;
            this->population[i].permeability_2[0] = floor(this->population[i].permeability_2[0] * 100) / 100;
            this->population[i].permeability_2[4] = permeabilityD4;
            this->population[i].permeability_2[4] = floor(this->population[i].permeability_2[4] * 100) / 100;
            this->population[i].permeability_3[0] = permeabilityD0;
            this->population[i].permeability_3[0] = floor(this->population[i].permeability_3[0] * 100) / 100;
            this->population[i].permeability_3[4] = permeabilityD4;
            this->population[i].permeability_3[4] = floor(this->population[i].permeability_3[4] * 100) / 100;
        }
        if (stod(decimal[1]) < 14)
        {
            int permeabilityI2 = rand() % (this->max_permeabilityI[1] - this->min_permeabilityI[1] + 1) + this->min_permeabilityI[1];
            int permeabilityI3 = rand() % (this->max_permeabilityI[1] - this->min_permeabilityI[1] + 1) + this->min_permeabilityI[1];
            this->population[i].permeability_1[2] = permeabilityI2;
            this->population[i].permeability_1[3] = permeabilityI3;
            this->population[i].permeability_2[2] = permeabilityI2;
            this->population[i].permeability_2[3] = permeabilityI3;
            this->population[i].permeability_3[2] = permeabilityI2;
            this->population[i].permeability_3[3] = permeabilityI3;
        }
        else
        {
            double permeabilityD2 = rand_double(this->min_permeabilityD[1], this->max_permeabilityD[1]);
            double permeabilityD3 = rand_double(this->min_permeabilityD[1], this->max_permeabilityD[1]);
            this->population[i].permeability_1[2] = permeabilityD2;
            this->population[i].permeability_1[2] = floor(this->population[i].permeability_1[2] * 100) / 100;
            this->population[i].permeability_1[3] = permeabilityD3;
            this->population[i].permeability_1[3] = floor(this->population[i].permeability_1[3] * 100) / 100;
            this->population[i].permeability_2[2] = permeabilityD2;
            this->population[i].permeability_2[2] = floor(this->population[i].permeability_2[2] * 100) / 100;
            this->population[i].permeability_2[3] = permeabilityD3;
            this->population[i].permeability_2[3] = floor(this->population[i].permeability_2[3] * 100) / 100;
            this->population[i].permeability_3[2] = permeabilityD2;
            this->population[i].permeability_3[2] = floor(this->population[i].permeability_3[2] * 100) / 100;
            this->population[i].permeability_3[3] = permeabilityD3;
            this->population[i].permeability_3[3] = floor(this->population[i].permeability_3[3] * 100) / 100;
        }
        this->population[i].permeability_1[1] = dataset[i].permeability_1[1];
        this->population[i].permeability_2[1] = dataset[i].permeability_2[1];
        this->population[i].permeability_3[1] = dataset[i].permeability_3[1];
    }

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        ifstream read_input("../Output/0/inputDS_" + to_string(i) + ".dat", ios::in);
        ofstream write_input("../inputDS_" + to_string(i) + ".dat", ios::out);
        int count = 0;
        line = "";
        while (!read_input.eof())
        {
            getline(read_input, line);
            if (count == 142)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[0] << "   " << (this->population[i].permeability_1[0] * 10e-15) << "   " << (this->population[i].permeability_2[0] * 10e-15) << "   " << (this->population[i].permeability_3[0] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 143)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[1] << "   " << this->population[i].permeability_1[1] << "   " << this->population[i].permeability_2[1] << "   " << this->population[i].permeability_3[1] << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 144)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[2] << "   " << (this->population[i].permeability_1[2] * 10e-15) << "   " << (this->population[i].permeability_2[2] * 10e-15) << "   " << (this->population[i].permeability_3[2] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 145)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[3] << "   " << (this->population[i].permeability_1[3] * 10e-15) << "   " << (this->population[i].permeability_2[3] * 10e-15) << "   " << (this->population[i].permeability_3[3] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 146)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[4] << "   " << (this->population[i].permeability_1[4] * 10e-15) << "   " << (this->population[i].permeability_2[4] * 10e-15) << "   " << (this->population[i].permeability_3[4] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else
            {
                write_input << line << endl;
            }
            count++;
        }

        read_input.close();
        write_input.close();

        string command = "mv ../inputDS_" + to_string(i) + ".dat ../Output/0/";
        const char *file = (char *)command.c_str();
        system(file);
    }

    simulation(0);
    fitness(0);
    sort(begin(this->population), end(this->population), compare);

    ofstream write_error("../Output/0/error.txt", ios::out);

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        write_error << scientific << this->population[i].error_rank << endl;
    }

    write_error.close();

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        ifstream read_input("../Output/0/inputDS_" + to_string(i) + ".dat", ios::in);
        ofstream write_input("../inputDS_" + to_string(i) + ".dat", ios::out);
        int count = 0;
        line = "";
        while (!read_input.eof())
        {
            getline(read_input, line);
            if (count == 142)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[0] << "   " << (this->population[i].permeability_1[0] * 10e-15) << "   " << (this->population[i].permeability_2[0] * 10e-15) << "   " << (this->population[i].permeability_3[0] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 143)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[1] << "   " << this->population[i].permeability_1[1] << "   " << this->population[i].permeability_2[1] << "   " << this->population[i].permeability_3[1] << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 144)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[2] << "   " << (this->population[i].permeability_1[2] * 10e-15) << "   " << (this->population[i].permeability_2[2] * 10e-15) << "   " << (this->population[i].permeability_3[2] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 145)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[3] << "   " << (this->population[i].permeability_1[3] * 10e-15) << "   " << (this->population[i].permeability_2[3] * 10e-15) << "   " << (this->population[i].permeability_3[3] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 146)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[4] << "   " << (this->population[i].permeability_1[4] * 10e-15) << "   " << (this->population[i].permeability_2[4] * 10e-15) << "   " << (this->population[i].permeability_3[4] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else
            {
                write_input << line << endl;
            }
            count++;
        }

        read_input.close();
        write_input.close();

        string command = "mv ../inputDS_" + to_string(i) + ".dat ../Output/0/";
        const char *file = (char *)command.c_str();
        system(file);
    }
}

void genetic_algorithm::otherPopulations(int idIteration)
{
    crossover();

    string command = "../Output/" + to_string(idIteration);
    const char *file = (char *)command.c_str();
    DIR *dp = opendir(file);

    if (dp == NULL)
    {
        command = "mkdir ../Output/" + to_string(idIteration);
        string command2 = "mkdir ../Output/" + to_string(idIteration) + "/agua";
        string command3 = "mkdir ../Output/" + to_string(idIteration) + "/oleo";
        file = (char *)command.c_str();
        const char *file2 = (char *)command2.c_str();
        const char *file3 = (char *)command3.c_str();
        system(file);
        system(file2);
        system(file3);
    }
    else
    {
        command = "rm -f ../Output/" + to_string(idIteration) + "/*";
        file = (char *)command.c_str();
        system(file);
    }

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        string command = "cp ../Output/" + to_string(idIteration - 1) + "/inputDS_" + to_string(i) + ".dat ../Output/" + to_string(idIteration) + "/inputDS_" + to_string(i) + ".dat";
        const char *file = (char *)command.c_str();
        system(file);
    }

    for (int i = SIZE_POPULATION; i < (SIZE_POPULATION + this->crossover_rate); i++)
    {
        string command = "cp ../Input/inputDS.dat ../Output/" + to_string(idIteration) + "/inputDS_" + to_string(i) + ".dat";
        const char *file = (char *)command.c_str();
        system(file);
    }

    for (int i = SIZE_POPULATION; i < (SIZE_POPULATION + this->crossover_rate); i++)
    {
        ifstream read_input("../Output/" + to_string(idIteration) + "/inputDS_" + to_string(i) + ".dat", ios::in);
        ofstream write_input("../inputDS_" + to_string(i) + ".dat", ios::out);
        int count = 0;
        string line;
        while (!read_input.eof())
        {
            getline(read_input, line);
            if (count == 142)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[0] << "   " << (this->population[i].permeability_1[0] * 10e-15) << "   " << (this->population[i].permeability_2[0] * 10e-15) << "   " << (this->population[i].permeability_3[0] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 143)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[1] << "   " << this->population[i].permeability_1[1] << "   " << this->population[i].permeability_2[1] << "   " << this->population[i].permeability_3[1] << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 144)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[2] << "   " << (this->population[i].permeability_1[2] * 10e-15) << "   " << (this->population[i].permeability_2[2] * 10e-15) << "   " << (this->population[i].permeability_3[2] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 145)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[3] << "   " << (this->population[i].permeability_1[3] * 10e-15) << "   " << (this->population[i].permeability_2[3] * 10e-15) << "   " << (this->population[i].permeability_3[3] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 146)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[4] << "   " << (this->population[i].permeability_1[4] * 10e-15) << "   " << (this->population[i].permeability_2[4] * 10e-15) << "   " << (this->population[i].permeability_3[4] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else
            {
                write_input << line << endl;
            }
            count++;
        }

        read_input.close();
        write_input.close();

        string command = "mv ../inputDS_" + to_string(i) + ".dat ../Output/" + to_string(idIteration) + "/";
        const char *file = (char *)command.c_str();
        system(file);
    }

    simulation(idIteration);
    fitness(idIteration);
    sort(begin(this->population), end(this->population), compare);

    ofstream write_error("../Output/" + to_string(idIteration) + "/error.txt", ios::out);

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        write_error << scientific << this->population[i].error_rank << endl;
    }

    write_error.close();

    string removeDat = "rm -f ../Output/" + to_string(idIteration) + "/*.dat";
    const char *remDat = (char *)removeDat.c_str();
    system(remDat);

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        string command = "cp ../Input/inputDS.dat ../Output/" + to_string(idIteration) + "/inputDS_" + to_string(i) + ".dat";
        const char *file = (char *)command.c_str();
        system(file);
    }

    for (int i = SIZE_POPULATION; i < (SIZE_POPULATION + this->crossover_rate); i++)
    {
        this->population.pop_back();
    }

    for (int i = 0; i < SIZE_POPULATION; i++)
    {
        ifstream read_input("../Output/" + to_string(idIteration) + "/inputDS_" + to_string(i) + ".dat", ios::in);
        ofstream write_input("../inputDS_" + to_string(i) + ".dat", ios::out);
        int count = 0;
        string line;
        while (!read_input.eof())
        {
            getline(read_input, line);
            if (count == 142)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[0] << "   " << (this->population[i].permeability_1[0] * 10e-15) << "   " << (this->population[i].permeability_2[0] * 10e-15) << "   " << (this->population[i].permeability_3[0] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 143)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[1] << "   " << this->population[i].permeability_1[1] << "   " << this->population[i].permeability_2[1] << "   " << this->population[i].permeability_3[1] << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 144)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[2] << "   " << (this->population[i].permeability_1[2] * 10e-15) << "   " << (this->population[i].permeability_2[2] * 10e-15) << "   " << (this->population[i].permeability_3[2] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 145)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[3] << "   " << (this->population[i].permeability_1[3] * 10e-15) << "   " << (this->population[i].permeability_2[3] * 10e-15) << "   " << (this->population[i].permeability_3[3] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else if (count == 146)
            {
                vector<string> v{split(line, ' ')};
                write_input << "         " << v[0] << "   " << scientific << this->population[i].porosity[4] << "   " << (this->population[i].permeability_1[4] * 10e-15) << "   " << (this->population[i].permeability_2[4] * 10e-15) << "   " << (this->population[i].permeability_3[4] * 10e-15) << "   " << v[5] << "   " << v[6] << "   " << v[7] << "   " << v[8] << "   " << v[9] << endl;
            }
            else
            {
                write_input << line << endl;
            }
            count++;
        }

        read_input.close();
        write_input.close();

        string command = "mv ../inputDS_" + to_string(i) + ".dat ../Output/" + to_string(idIteration) + "/";
        const char *file = (char *)command.c_str();
        system(file);
    }
}

void genetic_algorithm::fitness(int idIteration)
{
    if (idIteration == 0)
    {
        for (int i = 0; i < SIZE_POPULATION; i++)
        {
            double rank;

            ifstream result_water("../Output/" + to_string(idIteration) + "/agua/vazaoAgua_" + to_string(i) + ".dat", ios::in);
            ifstream result_oil("../Output/" + to_string(idIteration) + "/oleo/vazaoOleo_" + to_string(i) + ".dat", ios::in);

            string line, line2, content, content2;

            while (!result_water.eof() && !result_oil.eof())
            {
                getline(result_water, line);
                getline(result_oil, line2);

                content += line;
                content += " ";
                content2 += line2;
                content2 += " ";
            }

            result_water.close();
            result_oil.close();

            vector<string> v{split(content, ' ')};
            vector<string> v2{split(content2, ' ')};

            result simulate_results[v.size()];

            for (int j = 0; j < v.size(); j++)
            {
                if(stod(v[j]) == 0.0)
                {
                    simulate_results[j].water = -1;
                }
                else
                {
                    simulate_results[j].water = stod(v[j]);
                }

                if(stod(v2[j]) == 0.0)
                {
                    simulate_results[j].oil = -1;
                }
                else
                {
                    simulate_results[j].oil = stod(v2[j]);
                }
            }

            for (int j = 0; j < N_METRICS; j++)
            {
                if (j == 0)
                {
                    for (int k = 0; k < v.size(); k++)
                    {
                        rank += pow((this->real_results[k].water - simulate_results[k].water), 2);
                    }
                    rank *= WATER_WEIGHT;
                }
                else if (j == 1)
                {
                    for (int k = 0; k < v.size(); k++)
                    {
                        rank += pow((this->real_results[k].oil - simulate_results[k].oil), 2);
                    }
                    rank *= OIL_WEIGHT;
                }
            }

            rank = sqrt((rank / (v.size() * 2)));

            this->population[i].error_rank = rank;
        }
    }
    else
    {
        for (int i = SIZE_POPULATION; i < (SIZE_POPULATION + this->crossover_rate); i++)
        {
            double rank;

            ifstream result_water("../Output/" + to_string(idIteration) + "/agua/vazaoAgua_" + to_string(i) + ".dat", ios::in);
            ifstream result_oil("../Output/" + to_string(idIteration) + "/oleo/vazaoOleo_" + to_string(i) + ".dat", ios::in);

            string line, line2, content, content2;

            while (!result_water.eof() && !result_oil.eof())
            {
                getline(result_water, line);
                getline(result_oil, line2);

                content += line;
                content += " ";
                content2 += line2;
                content2 += " ";
            }

            result_water.close();
            result_oil.close();

            vector<string> v{split(content, ' ')};
            vector<string> v2{split(content2, ' ')};

            result simulate_results[v.size()];

            for (int j = 0; j < v.size(); j++)
            {
                if(stod(v[j]) == 0.0)
                {
                    simulate_results[j].water = -1;
                }
                else
                {
                    simulate_results[j].water = stod(v[j]);
                }

                if(stod(v2[j]) == 0.0)
                {
                    simulate_results[j].oil = -1;
                }
                else
                {
                    simulate_results[j].oil = stod(v2[j]);
                }
            }

            for (int j = 0; j < N_METRICS; j++)
            {
                if (j == 0)
                {
                    for (int k = 0; k < v.size(); k++)
                    {
                        rank += pow((this->real_results[k].water - simulate_results[k].water), 2);
                    }
                    rank *= WATER_WEIGHT;
                }
                else if (j == 1)
                {
                    for (int k = 0; k < v.size(); k++)
                    {
                        rank += pow((this->real_results[k].oil - simulate_results[k].oil), 2);
                    }
                    rank *= OIL_WEIGHT;
                }
            }

            rank = sqrt((rank / (v.size() * 2)));

            this->population[i].error_rank = rank;
        }
    }
}

void genetic_algorithm::crossover()
{
    for (int i = 0; i < crossover_rate; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            this->children[i].porosity[j] = 0;
            this->children[i].permeability_1[j] = 0;
            this->children[i].permeability_2[j] = 0;
            this->children[i].permeability_3[j] = 0;
            this->children[i].error_rank = 0;
        }
    }

    //Escolhe o ponto de corte
    int count = 0;
    while (count < crossover_rate)
    {
        for (int i = 0; i < 5; i++)
        {
            this->children[count].porosity[i] = this->population[count].porosity[i];
            this->children[count].permeability_1[i] = this->population[count + 1].permeability_1[i];
            this->children[count].permeability_2[i] = this->population[count + 1].permeability_2[i];
            this->children[count].permeability_3[i] = this->population[count + 1].permeability_3[i];

            this->children[count + 1].porosity[i] = this->population[count + 1].porosity[i];
            this->children[count + 1].permeability_1[i] = this->population[count].permeability_1[i];
            this->children[count + 1].permeability_2[i] = this->population[count].permeability_2[i];
            this->children[count + 1].permeability_3[i] = this->population[count].permeability_3[i];
        }
        count = count + 2;
    }

    mutation();

    //Deixando os valores com apenas duas casas decimais
    for (int i = 0; i < crossover_rate; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (j == 1)
            {
                break;
            }
            else
            {
                this->children[i].porosity[j] = floor(this->children[i].porosity[j] * 100) / 100;
                this->children[i].permeability_1[j] = floor(this->children[i].permeability_1[j] * 100) / 100;
                this->children[i].permeability_2[j] = floor(this->children[i].permeability_2[j] * 100) / 100;
                this->children[i].permeability_3[j] = floor(this->children[i].permeability_3[j] * 100) / 100;
            }
        }
    }

    for (int i = 0; i < this->crossover_rate; i++)
    {
        this->population.push_back(children[i]);
    }
}

void genetic_algorithm::mutation()
{
    int valuePorosityI;
    int valuePermeabilityI;

    double valuePorosityD;
    double valuePermeabilityD;

    for (int i = 0; i < this->mutation_rate; i++)
    {
        int percent = rand() % 3;
        int tunning = rand() % 2;
        int gene = rand() % 2;

        if (percent == 0)
        {
            for (int j = 0; j < 5; j++)
            {
                if (j == 1)
                {
                    break;
                }
                else if ((j == 0) || (j == 4))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 5) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 5) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 5) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 5) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
                else if ((j == 2) || (j == 3))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 5) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 5) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 5) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 5) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
            }
        }
        else if (percent == 1)
        {
            for (int j = 0; j < 5; j++)
            {
                if (j == 1)
                {
                    break;
                }
                else if ((j == 0) || (j == 4))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 10) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 10) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 10) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 10) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
                else if ((j == 2) || (j == 3))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 10) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 10) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 10) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 10) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
            }
        }
        else if (percent == 2)
        {
            for (int j = 0; j < 5; j++)
            {
                if (j == 1)
                {
                    break;
                }
                else if ((j == 0) || (j == 4))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 20) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 20) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[0], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[0], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 20) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 20) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[0], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[0], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
                else if ((j == 2) || (j == 3))
                {
                    if (stod(decimal[0]) < 14)
                    {
                        valuePorosityI = ((this->children[i].porosity[j] * 20) / 100);
                        valuePermeabilityI = ((this->children[i].permeability_1[j] * 20) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityI));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_1[j] + valuePermeabilityI));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_2[j] + valuePermeabilityI));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityI[1], (this->children[i].permeability_3[j] + valuePermeabilityI));
                            }
                        }
                        else
                        {
                            if (gene == 0)
                            {  
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                            }
                            else{
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityI));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_1[j] - valuePermeabilityI));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_2[j] - valuePermeabilityI));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityI[1], (this->children[i].permeability_3[j] - valuePermeabilityI));
                            }
                        }
                    }
                    else
                    {
                        valuePorosityD = ((this->children[i].porosity[j] * 20) / 100);
                        valuePermeabilityD = ((this->children[i].permeability_1[j] * 20) / 100);
                        if (tunning == 0)
                        {
                            if (gene == 0)
                            {
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                            }
                            else{
                                this->children[i].porosity[j] = min(MAX_POROSITY, (this->children[i].porosity[j] + valuePorosityD));
                                this->children[i].permeability_1[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_1[j] + valuePermeabilityD));
                                this->children[i].permeability_2[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_2[j] + valuePermeabilityD));
                                this->children[i].permeability_3[j] = min(this->max_permeabilityD[1], (this->children[i].permeability_3[j] + valuePermeabilityD));
                            }
                        }
                        else
                        {
                            if (gene = 0)
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                            }
                            else
                            {
                                this->children[i].porosity[j] = max(MIN_POROSITY, (this->children[i].porosity[j] - valuePorosityD));
                                this->children[i].permeability_1[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_1[j] - valuePermeabilityD));
                                this->children[i].permeability_2[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_2[j] - valuePermeabilityD));
                                this->children[i].permeability_3[j] = max(this->min_permeabilityD[1], (this->children[i].permeability_3[j] - valuePermeabilityD));
                            }
                        }
                    }
                }
            }
        }
    }
}
