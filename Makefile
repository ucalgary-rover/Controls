.RECIPEPREFIX = >
SRC_FILES := $(wildcard prj/*.cpp)
SRC_FILES += $(wildcard prj/Base/*.cpp)

SRC_FILES := $(filter-out prj/websocket.cpp, $(SRC_FILES))

BOOST_ROOT = "C:/Program Files/boost/boost_1_87_0"
SDL_ROOT = tools/SDL
PHIDGETS_ROOT = "tools/Phidgets/Phidget22"

CMPL_BOOST = -I $(BOOST_ROOT)
CMPL_SDL = -I $(SDL_ROOT)/include/SDL2 -L $(SDL_ROOT)/lib -lmingw32 -lSDL2main -lSDL2
CMPL_PHIDETS = -I $(PHIDGETS_ROOT) -L tools/Phidgets/Phidget22/x64/ -lphidget22 

NULL_DEVICE := $(if $(filter Windows_NT,$(OS)),NUL,/dev/null)

run_test:
> cmake -S ./test -B test/build
> cmake --build test/build
> cd test/build && ctest

compile:
> g++  -I"prj" prj/main.cpp -o main

run: compile
> ./main.exe

controller_test:
> g++ -I"prj" prj/Base/ControllerTesting.cpp -o ControllerTesting -I"C:/mingw-dev-lib/include/SDL2" -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2
> ./ControllerTesting.exe

base:
> g++ -I"prj" $(SRC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(CMPL_PHIDETS) -o main

run_base: base
> ./main.exe