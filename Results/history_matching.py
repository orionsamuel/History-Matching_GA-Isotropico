import os
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
from sklearn.metrics import r2_score
from sklearn.metrics import mean_absolute_percentage_error

if(not os.path.exists("Matchs")):
    os.system("mkdir Matchs")
else:
    os.system("rm -f Matchs/*")

for i in range(10): 
    water = []
    oil = []
    real_water = []
    real_oil = []

    inputFile_Water = open("Output_Simulation/agua/vazaoAgua_"+str(i)+".dat", "r")
    inputFile_Oil = open("Output_Simulation/oleo/vazaoOleo_"+str(i)+".dat", "r")

    for line in inputFile_Water:
        found = line.split(" ")
        water.append(float(found[3]))
        
    inputFile_Water.close()

    for line in inputFile_Oil:
        found = line.split(" ")
        oil.append(float(found[3]))   

    inputFile_Oil.close()

    inputFile_RealWater = open("../Input/resultadoVazaoAgua.dat", "r")
    inputFile_RealOil = open("../Input/resultadoVazaoOleo.dat", "r")

    for line in inputFile_RealWater:
        found = line.split(" ")
        real_water.append(float(found[3]))

    inputFile_Water.close()

    for line in inputFile_RealOil:
        found = line.split(" ")
        real_oil.append(float(found[3]))

    inputFile_Oil.close()

    print(str(i)+" - MAPE Error Water: ", mean_absolute_percentage_error(real_water, water))
    print(str(i)+" - R² Error Water: ", r2_score(real_water, water))
    print(str(i)+" - MAPE Error Oil: ", mean_absolute_percentage_error(real_oil, oil))
    print(str(i)+" - R² Error Water: ", r2_score(real_oil, oil))
    print("")

    output = open("Matchs/resultado_"+str(i)+".txt", "w")

    output.write("MAPE Error Water: "+str(mean_absolute_percentage_error(real_water, water))+"\n")
    output.write("R² Error Water: "+str(r2_score(real_water, water))+"\n")
    output.write("MAPE Error Oil: "+str(mean_absolute_percentage_error(real_oil, oil))+"\n")
    output.write("R² Error Water: "+str(r2_score(real_oil, oil))+"\n")

    output.close()

    values = [[real_water, water],[real_oil, oil]]
    
    count = 0
    for value in values:
        instances = []
        
        for j in range(len(real_water)):
            instances.append(j)

        if(count == 0):
            plt.title("Water Flow")
            plt.ylabel("Error Rate (Log Scale)")
            plt.xlabel("Time")
            plt.yscale('log')
            plt.plot(instances, values[0][0], color='red', label ='Real')
            plt.plot(instances, values[0][1], color='purple', label ='Predicted')
            plt.legend(loc = 'upper right')
            plt.savefig("Matchs/Matching Water_"+str(i)+" - Linhas.png")
        else:
            plt.title("Oil Flow")
            plt.ylabel("Error Rate (Log Scale)")
            plt.xlabel("Time")
            plt.yscale('log')
            plt.plot(instances, values[1][0], color='red', label ='Real')
            plt.plot(instances, values[1][1], color='purple', label ='Predicted')
            plt.legend(loc = 'upper right')
            plt.savefig("Matchs/Matching Oil_"+str(i)+" - Linhas.png")
        
        plt.clf()

        count = count + 1
