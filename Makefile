CPP = g++
CPPFLAGS = -pthread -lstdc++ -std=c++11 -g
OFLAGS = -o
OBJECTS = main.o

ifeq ($(OS), Windows_NT)
	BUILDEXE := rayffitica.exe
	RM := del
else 
	BUILDEXE := rayffitica
	RM := rm -rf
endif

.SUFFIXES: .o .cpp
.cpp.o:
	$(CPP) $(CPPFLAGS) -c $< $(OFLAGS) $@

.PHONY: all clean 

all: $(OBJECTS) build 

build: $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(OFLAGS) $(BUILDEXE) $(OBJECTS)

clean: 
	$(RM) *.o rayffitica
