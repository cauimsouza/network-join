# Project settings

HEADER_DIR := include
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
BIN_NAME := decide_later # name of the final file

# Compiler settings

CXX := g++ 
CXX_FLAGS := -std=c++11 -I $(HEADER_DIR)

# Gather filenames

SOURCES = $(shell find $(SRC_PATH) -name '*.cpp') # every cpp file in src
OBJECTS = $(SOURCES:$(SRC_PATH)/%.cpp=$(BUILD_PATH)/%.o) # regex treatment to switch .cpp for .o

# Recipes

all: $(BIN_DIR)/$(BIN_NAME) #compile main executable and create a shortcut in root directory
	@$(RM) $(BIN_NAME)
	@ln -s $(BIN_DIR)/$(BIN_NAME) $(BIN_NAME) 

$(BIN_DIR)/$(BIN_NAME): $(OBJECTS) # main executable
	$(CXX) $(CXX_FLAGS) $^ -o $@

test_join: tests/join/test_join.cpp $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -o $@	

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp # source files
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

clean:
	@$(RM) -r $(BUILD_DIR)
	@$(RM) $(BIN_NAME)
	@$(RM) -r $(BIN_DIR)
