.PHONY: DFA clean all
CXX = g++
CXXFLAGS = -std=c++14 
TASK = DFA
all: bin DFA

bin:
	mkdir -p bin
	
DFA: bin/DFA.o 
	$(CXX) $(CXXFLAGS) bin/DFA.o -o DFA

bin/DFA.o: $(TASK).cpp
	$(CXX) $(CXXFLAGS) -c $(TASK).cpp -o bin/DFA.o

clean:
	rm -rf bin DFA DFA.dot DFA.png minDFA.dot minDFA.png DFAinfo
