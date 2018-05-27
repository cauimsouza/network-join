# Project settings

HEADER_DIR := include
SRC_DIR := src
BUILD_DIR := build
LIB_DIR := lib
BIN_DIR := bin
BIN_NAME := decide_later # name of the final file

# Create build directories
$(shell mkdir -p $(BUILD_DIR)) #create directories for building
$(shell mkdir -p $(BUILD_DIR)/common) #create directories for building
$(shell mkdir -p $(BUILD_DIR)/mpi) #create directories for building
$(shell mkdir -p $(BIN_DIR)) 

# Gather filenames

COMMON_SOURCES = $(shell find $(SRC_DIR)/common -name '*.cpp') # cpp files that are used both in sequential and mpi executions
COMMON_OBJECTS = $(COMMON_SOURCES:$(SRC_DIR)/common/%.cpp=$(BUILD_DIR)/common/%.o) # regex treatment to switch .cpp for .o
MPI_SOURCES = $(shell find $(SRC_DIR)/mpi -name '*.cpp') # cpp files that are used only in mpi executions
MPI_OBJECTS = $(MPI_SOURCES:$(SRC_DIR)/mpi/%.cpp=$(BUILD_DIR)/mpi/%.o)
LIB_FILES = $(shell find $(LIB_DIR) -name '*.*') 

# Compiler settings

CXX := g++ 
MPI_CXX := mpic++
CXX_FLAGS := -std=c++11 -I $(HEADER_DIR) 
SAVE_FLAGS := -save-temps=build/common/ioutil.o # flags for compilation optimization
MPI_CXX_FLAGS := -std=c++11 -I $(HEADER_DIR) -lboost_serialization -lboost_mpi # later switch for static lib?



# Recipes

#all: $(BIN_DIR)/$(BIN_NAME) #compile main executable and create a shortcut in root directory
#	@$(RM) $(BIN_NAME)
#	@ln -s $(BIN_DIR)/$(BIN_NAME) $(BIN_NAME) 

#$(BIN_DIR)/$(BIN_NAME): $(OBJECTS) # main executable
#	$(CXX) $(CXX_FLAGS) $^ -o $@

test_%: tests/**/test_%.cpp $(COMMON_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -o $(BIN_DIR)/$@	

mpi_test_%: tests/**/mpi_test_%.cpp $(COMMON_OBJECTS) $(MPI_OBJECTS)
	$(MPI_CXX) $(MPI_CXX_FLAGS) $^ -o $(BIN_DIR)/$@

$(BUILD_DIR)/common/%.o: $(SRC_DIR)/common/%.cpp # common source files
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

$(BUILD_DIR)/mpi/%.o: $(SRC_DIR)/mpi/%.cpp # mpi source files
	$(MPI_CXX) $(MPI_CXX_FLAGS) -c $^ -o $@

.PRECIOUS: $(BUILD_DIR)/common/%.o $(BUILD_DIR)/mpi/%.o # dont delete intermediary object files

clean:
	@$(RM) -r $(BUILD_DIR)
	@$(RM) $(BIN_NAME)
	@$(RM) -r $(BIN_DIR)
