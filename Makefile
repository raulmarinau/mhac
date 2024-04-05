CXX = c++
CXXFLAGS = -g -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude
TARGET = build/mhac.so

SOURCES_BINDINGS = src/bindings.cpp
SOURCES_LOGGER = src/logger/logger.cpp
SOURCES_RANDOM = src/random/random.cpp
SOURCES_ALG_PHYSICS = src/physics/SA.cpp
SOURCES = $(SOURCES_BINDINGS) $(SOURCES_LOGGER) $(SOURCES_ALG_PHYSICS) $(SOURCES_RANDOM)

all: lib-build

lib-build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)