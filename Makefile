# Where are you running the code?
SALLES_DINFO=true

# Project settings

HEADER_DIR = include
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Create build directories
$(shell mkdir -p $(BUILD_DIR)) #create directories for building
$(shell mkdir -p $(BIN_DIR)) 

# Gather filenames

SOURCES = $(shell find $(SRC_DIR) -name '*.cpp') # cpp files 
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) # regex treatment to switch .cpp for .o

# Compiler settings

CXX := mpic++ 
CXX_FLAGS := -std=c++11 -I $(HEADER_DIR) -lboost_mpi -lboost_serialization 
ifeq ($(SALLES_DINFO),true)
	CXX_FLAGS := -std=c++11 -I $(HEADER_DIR) -Llib -lboost_mpi -lboost_serialization # use provided lib files
endif

# Recipes

all: test_join test_triangles	

test_%: $(OBJECTS) tests/**/test_%.cpp 
	$(CXX) $(CXX_FLAGS) $^ -o $(BIN_DIR)/$@	

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp # source files
	$(CXX) $(CXX_FLAGS) -c $^ -o $@

.PRECIOUS: $(BUILD_DIR)/%.o  # dont delete intermediary object files

clean:
	@$(RM) -r $(BUILD_DIR)
	@$(RM) $(BIN_NAME)
	@$(RM) -r $(BIN_DIR)
