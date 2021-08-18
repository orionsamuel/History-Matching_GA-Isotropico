import os

def init(n):
    simulation(n)

def simulation(n):
    if(not os.path.exists("Output_Simulation")):
        os.system("mkdir Output_Simulation")
        os.system("mkdir Output_Simulation/agua")
        os.system("mkdir Output_Simulation/oleo")
    else:
        os.system("rm -r -f Output_Simulation/*")
        os.system("mkdir Output_Simulation/agua")
        os.system("mkdir Output_Simulation/oleo")

    os.system("rm -f output_simulation")
    for i in range(n):
        print("Executando a simulação: "+str(i))
        os.system("cp ../Output/9/inputDS_"+str(i)+".dat "
                  +"../../Codigo_Bifasico_Slab/simulacoes/dev/inputDS.dat")
        os.system("./../../Codigo_Bifasico_Slab"
                  +"/rodarSimulador.sh >> output_simulation 2>>"
                  +"output_simulation")
        os.system("cp ../../Codigo_Bifasico_Slab/simulacoes/dev/out/"
                  +"resultadoVazaoAgua.dat Output_Simulation/agua/"
                  +"vazaoAgua_"+str(i)+".dat")
        os.system("cp ../../Codigo_Bifasico_Slab/simulacoes/dev/out/"
                  +"resultadoVazaoOleo.dat Output_Simulation/oleo/"
                  +"vazaoOleo_"+str(i)+".dat")


if __name__ == '__main__':
    init(10)




