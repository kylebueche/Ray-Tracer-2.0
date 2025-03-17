header_files = $(wildcard src/*.h)
cpp_files = $(wildcard src/*.cpp)
obj_files = $(patsubst src/%.cpp, obj_files/%.o, $(cpp_files))

output_executable = RayTracer

$(output_executable): $(obj_files)
	g++ $(obj_files) -ggdb -o $(output_executable) -fopenmp -lgdi32 -lmingw32

obj_files/%.o: src/%.cpp $(header_files)
	g++ -fopenmp -c -o $@ $<

