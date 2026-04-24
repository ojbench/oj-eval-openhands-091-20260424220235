CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
TARGET = code
SOURCES = main.cpp

all: $(TARGET)

$(TARGET): $(SOURCES) src.hpp
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
