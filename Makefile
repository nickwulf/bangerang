
PROJ_NAME = bangerang

INCLUDE_FLAG = -I"./include"
LIB_FLAG = -L"./lib"

# Names of tools to use when building
CC = g++

C_FLAGS = -std=c++17 $(INCLUDE_FLAG)
L_FLAGS = -lfreeglut -lopengl32 -lglu32 -lstdc++ $(LIB_FLAG)

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=obj/%.o)

.PHONY: all clean

all: bin/$(PROJ_NAME).exe
	
clean:
	@if exist obj\*		del /Q obj\*
	@if exist obj			rmdir obj
	@if exist bin\*		del /Q bin\*
	@if exist bin			rmdir bin
	
obj:
	@if not exist "$@" mkdir "$@"
	
bin:
	@if not exist "$@" mkdir "$@"
	
bin/$(PROJ_NAME).exe: ${OBJS} | bin
	$(CC) -o "$@" ${OBJS} ${L_FLAGS}

obj/%.o: src/%.cpp | obj
	$(CC) -c $< -o $@ $(C_FLAGS)



