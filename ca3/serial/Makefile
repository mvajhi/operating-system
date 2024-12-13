CXX = g++
CXXFLAGS = -lsndfile -Wall -g -I. -I../shared -std=c++11


SRC_DIR = .
OBJ_DIR = out
SHARED_DIR = ../shared
SHARED_OBJ_DIR = $(SHARED_DIR)/out

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

SHARED_SRCS = $(wildcard $(SHARED_DIR)/*.cpp)
SHARED_OBJS = $(patsubst $(SHARED_DIR)/%.cpp, $(SHARED_OBJ_DIR)/%.o, $(SHARED_SRCS))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

out/VoiceFilter.out: $(OBJS) $(SHARED_OBJS)
	mkdir -p out
	$(CXX) $^ -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) out/VoiceFilter.out

.PHONY: all clean
