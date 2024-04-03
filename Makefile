CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude -Iinclude/physics
TARGET = build/mhac.so
SOURCES = src/physics/SA.cpp src/bindings.cpp

all: lib-build

lib-build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)