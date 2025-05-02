flags = -std=c++23

DEBUG:
	flags += -ggdb

# Determine object files from cpp files, incase they have not been generated
cpp_files = $(wildcard src/*.cpp)
obj_files = $(patsubst src/%.cpp, obj/%.o, $(cpp_files))

RayTracer: $(obj_files)
	g++ $(flags) $(obj_files) -o RayTracer -fopenmp -lgdi32 -lmingw32

obj/main.o: src/main.cpp
	g++ $(flags) -c src/main.cpp -o obj/main.o

obj/aligned_array.o: src/aligned_array.cpp src/aligned_array.h
	g++ $(flags) -c src/aligned_array.cpp -o obj/aligned_array.o

obj/camera.o: src/camera.cpp src/camera.h
	g++ $(flags) -c src/camera.cpp -o obj/camera.o

obj/simd_camera.o: src/simd_camera.cpp src/simd_camera.h src/simd_sse_math.h src/simd_sse_fma_math.h src/hardware_support.h
	g++ $(flags) -msse -msse2 -mfma -mavx -mavx2 -c src/simd_camera.cpp -o obj/simd_camera.o

obj/hardware_support.o: src/hardware_support.cpp src/hardware_support.h
	g++ $(flags) -c src/simd_camera.cpp -o obj/simd_camera.o
