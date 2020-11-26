CXX			:= g++
CXX_FLAGS 	:= -g --std=gnu++14 -Wl,--allow-multiple-definition -pthread

BIN			:= bin
SRC			:= src
TESTS		:= src/Tests
INCLUDES	:= -Iincludes
TEST_SRC	:= $(filter-out $(SRC)/main.cpp, $(wildcard $(SRC)/*.cpp))

ifeq ($(UNAME), Darwin)

else

LIB				:= libs
LIBRARIES		:= -lglfw3 -lgdi32 -lbass -lentityx -lstdc++fs -static-libgcc -static-libstdc++ 
TEST_LIBRARIES 	:= -lgtest 

endif

EXECUTABLE	:= main


build: $(SRC)/*.cpp $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -L$(LIB) $^ -o engine $(LIBRARIES)

tests: $(TEST_SRC) $(TESTS)/*.cpp $(SRC)/*.c 
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -L$(LIB) $^ -o runtests $(LIBRARIES) $(TEST_LIBRARIES) && ./runtests > test_results.txt
