CXX=g++
CXXFLAGS=-O4 -std=c++14 -Wall
ELPA_SOURCES=elpa/memory.cpp elpa/shell.cpp elpa/stream.cpp elpa/tests.cpp
KCON_SOURCES=kcon/console.cpp  kcon/interpreter.cpp  kcon/shell.cpp  kcon/tests.cpp
OHNO_SOURCES=ohno/console.cpp  ohno/shell.cpp  ohno/tests.cpp

all: bin/kcon bin/ohno

bin/kcon:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(ELPA_SOURCES) $(KCON_SOURCES) kcon.cpp -o bin/kcon

bin/ohno:
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(ELPA_SOURCES) $(KCON_SOURCES) $(OHNO_SOURCES) ohno.cpp -o bin/ohno

clean:
	rm -rf bin