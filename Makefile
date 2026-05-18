CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SRC = $(wildcard src/*.cpp)

TARGET = db_server

all:
	$(CXX) $(SRC) $(CXXFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)