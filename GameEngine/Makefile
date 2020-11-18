CXX			:= g++
CXX_FLAGS 	:= -g --std=gnu++14 

BIN			:= bin
SRC			:= src
TESTS		:= src/Tests
INCLUDES	:= -Iincludes
TEST_SRC	:= $(filter-out $(SRC)/main.cpp, $(SRC)/*.cpp)

ifeq ($(UNAME), Darwin)

else

LIB			:= libs
LIBRARIES	:= -lglfw3 -lgdi32 -lbass -lentityx -lgtest -lstdc++fs -static-libgcc -static-libstdc++ 

endif

EXECUTABLE	:= main


build: $(SRC)/*.cpp $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -L$(LIB) $^ -o engine $(LIBRARIES)

tests: $(TESTS)/*.cpp $(SRC)/*.c $(TEST_SRC)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -L$(LIB) $^ -o runtests $(LIBRARIES) && ./runtests > test_results.txt
