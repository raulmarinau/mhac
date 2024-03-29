CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude -Iinclude/physics
TARGET = build/mhac.so
SOURCES = src/physics/SA.cpp src/bindings.cpp

SYSTEM_PATH := ./build
PYTHON_SCRIPT := test/test_example.py


all: lib-build

lib-build: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

run-python:
	PYTHONPATH=$(SYSTEM_PATH):$$PYTHONPATH python3 $(PYTHON_SCRIPT)
