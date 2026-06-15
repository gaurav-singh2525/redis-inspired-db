CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SRC = $(wildcard src/*.cpp)

TARGET = bench_runner

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
