CXX = c++
COMMON_FLAGS = -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude

RELEASE_FLAGS = -O3 -DNDEBUG $(COMMON_FLAGS)
DEBUG_FLAGS = -g -O0 $(COMMON_FLAGS)

RELEASE_TARGET = build/release/mhac.so
DEBUG_TARGET = build/debug/mhac.so

SOURCES_BINDINGS = src/bindings.cpp
SOURCES_LOGGER = src/logger/logger.cpp
SOURCES_RANDOM = src/random/random.cpp

SOURCES_ALG_PHYSICS = src/physics/SA.cpp
SOURCES_ALG_MATH = src/math/TS.cpp
SOURCES_ALG_EVOLUTIONARY = src/evolutionary/GA.cpp
SOURCES_ALG_SWARM = src/swarm/ACO.cpp

SOURCES_PROBLEMS = src/problems/TSP.cpp src/problems/JSS.cpp
SOURCES = $(SOURCES_BINDINGS) $(SOURCES_LOGGER) $(SOURCES_PROBLEMS) $(SOURCES_ALG_PHYSICS) ${SOURCES_ALG_MATH} ${SOURCES_ALG_EVOLUTIONARY} ${SOURCES_ALG_SWARM} $(SOURCES_RANDOM)

all: release debug

release: $(RELEASE_TARGET)

$(RELEASE_TARGET): $(SOURCES)
	@mkdir -p $(@D)
	$(CXX) $(RELEASE_FLAGS) $(SOURCES) -o $(RELEASE_TARGET)

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SOURCES)
	@mkdir -p $(@D)
	$(CXX) $(DEBUG_FLAGS) $(SOURCES) -o $(DEBUG_TARGET)

clean:
	rm -f $(RELEASE_TARGET) $(DEBUG_TARGET)