CXX = g++
CXXFLAGS = --std=c++17 -g

LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt

SOURCES = src/main.cpp src/cpu.cpp src/graphics.cpp src/sound.cpp
OBJECTS = $(SOURCES:.cpp=.o)

chip8: $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o chip8

.PHONY: clean