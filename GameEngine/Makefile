CXX			:= g++
CXX_FLAGS 	:= -g --std=gnu++14 

BIN			:= bin
SRC			:= src
INCLUDES	:= -Iincludes

ifeq ($(UNAME), Darwin)

else

LIB			:= libs
LIBRARIES	:= -lglfw3 -lgdi32 -lbass -lentityx -lstdc++fs -static-libgcc -static-libstdc++ 

endif

EXECUTABLE	:= main


build: $(SRC)/*.cpp $(SRC)/*.c
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -L$(LIB) $^ -o engine $(LIBRARIES)
