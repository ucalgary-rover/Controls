.RECIPEPREFIX = >
SRC_FILES := $(wildcard prj/*.cpp)
SRC_FILES += $(wildcard prj/Base/*.cpp)

# SRC_FILES := $(filter-out prj/websocket.cpp, $(SRC_FILES))

BOOST_ROOT = "C:\Program Files\boost\boost_1_87_0"
SDL_ROOT = "C:/mingw-dev-lib/include/SDL2"
PHIDGETS_ROOT = "C:/Program Files/Phidgets/Phidget22"

CMPL_BOOST = -I $(BOOST_ROOT)
CMPL_SDL = -I $(SDL_ROOT) -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2
CMPL_PHIDETS = -I $(PHIDGETS_ROOT) -L"C:/Program Files/Phidgets/Phidget22/x64" -lphidget22 

compile:
> g++  -I"prj" prj/main.cpp -o main

run: compile
> ./main.exe

controller_test:
> g++ -I"prj" prj/Base/ControllerTesting.cpp -o ControllerTesting -I"C:/mingw-dev-lib/include/SDL2" -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2
> ./ControllerTesting.exe

base:
> g++ -I"prj" $(SRC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(PHIDGETS_ROOT) -o main

run_base: base
> ./main.exe