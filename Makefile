CXX=g++
CXXFLAGS = -std=c++17 -g -ggdb
out_file = raytracer1c

.DEFAULT_GOAL := all
all: $(out_file)

$(out_file): main.o image_gen.o options.o view_window.o 3d_object.o light_source.o
	$(CXX) $(CXXFLAGS) main.o image_gen.o options.o view_window.o 3d_object.o light_source.o -o $(out_file) 

main.o: main.cc
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o

image_gen.o: image_gen.cc
	$(CXX) $(CXXFLAGS) -c image_gen.cc -o image_gen.o

options.o: options.cc
	$(CXX) $(CXXFLAGS) -c options.cc -o options.o

view_window.o: view_window.cc
	$(CXX) $(CXXFLAGS) -c view_window.cc -o view_window.o

3d_object.o: 3d_object.cc
	$(CXX) $(CXXFLAGS) -c 3d_object.cc -o 3d_object.o

light_source.o: light_source.cc
	$(CXX) $(CXXFLAGS) -c light_source.cc -o light_source.o


clean:
	rm -f *.o $(out_file)

.PHONY: all clean