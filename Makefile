CXX=g++
CXXFLAGS = -std=c++17 -g -ggdb

.DEFAULT_GOAL := all
all: out_file

out_file: raytracer1a.o image_gen.o options.o view_window.o
	$(CXX) $(CXXFLAGS) raytracer1a.o image_gen.o options.o view_window.o -o out_file 

raytracer1a.o: raytracer1a.cc
	$(CXX) $(CXXFLAGS) -c raytracer1a.cc -o raytracer1a.o

image_gen.o: image_gen.cc
	$(CXX) $(CXXFLAGS) -c image_gen.cc -o image_gen.o

options.o: options.cc
	$(CXX) $(CXXFLAGS) -c options.cc -o options.o

view_window.o: view_window.cc
	$(CXX) $(CXXFLAGS) -c view_window.cc -o view_window.o


clean:
	rm -f *.o out_file

.PHONY: all clean