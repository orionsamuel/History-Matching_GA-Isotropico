GCC = g++
GCC_FLAGS = -I include -std=c++11 -g
EXECUTABLE = exec

all: clean $(EXECUTABLE)

$(EXECUTABLE): functions.o genetic.o main.o
	$(GCC) $(GCC_FLAGS) -o build/$(EXECUTABLE) main.o genetic.o functions.o 

main.o: src/main.cpp 
	$(GCC) $(GCC_FLAGS) -c src/main.cpp 

genetic.o: src/genetic.cpp include/genetic.hpp 
	$(GCC) $(GCC_FLAGS) -c src/genetic.cpp 

functions.o: src/functions.cpp include/functions.hpp include/utils.hpp
	$(GCC) $(GCC_FLAGS) -c src/functions.cpp

clean:
	rm -f *.o
	rm -f build/$(EXECUTABLE)
	rm -f build/output_simulation
