# shared/Makefile

CXX = g++
CXXFLAGS = -Wall -g -I.

SRC_DIR = .
OBJ_DIR = out

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all: $(OBJS)

clean:
	rm -rf $(OBJ_DIR)

.PHONY: all clean

