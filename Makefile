.RECIPEPREFIX = >
SRC_FILES := $(wildcard prj/*.cpp)
SRC_FILES += $(wildcard prj/Base/*.cpp)

SRC_FILES := $(filter-out prj/websocket.cpp, $(SRC_FILES))

compile:
> g++  -I"prj" prj/main.cpp -o main

run: compile
> ./main.exe

controller_test:
> g++ -I"prj" prj/Base/ControllerTesting.cpp -o ControllerTesting -I"C:/mingw-dev-lib/include/SDL2" -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2
> ./ControllerTesting.exe

base:
> g++ -I"prj" $(SRC_FILES) -I"C:/mingw-dev-lib/include/SDL2" -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2 -I"C:/Program Files/Phidgets/Phidget22" -L"C:/Program Files/Phidgets/Phidget22/x64" -lphidget22 -o main

run_base: base
> ./main.exe