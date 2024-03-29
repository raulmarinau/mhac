CXX = c++
CXXFLAGS = -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude
TARGET = build/example.so
SOURCES = src/example_calculator.cpp src/bindings.cpp
DEPS = $(SOURCES:.cpp=.d)

SYSTEM_PATH := ./build
PYTHON_SCRIPT := test/test_example.py


all: lib-build

lib-build: $(TARGET)

# c++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -Iinclude src/example_calculator.cpp src/bindings.cpp -o build/example.so
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(DEPS)

run-python:
	PYTHONPATH=$(SYSTEM_PATH):$$PYTHONPATH python3 $(PYTHON_SCRIPT)
