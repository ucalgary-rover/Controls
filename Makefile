.RECIPEPREFIX = >
BASE_SRC_FILES := $(wildcard prj/*.cpp)
BASE_SRC_FILES += $(wildcard prj/Base/*.cpp)
BASE_SRC_FILES += $(wildcard prj/Websocket/*.cpp)

BASE_SRC_FILES := $(filter-out prj/Websocket/Client.cpp, $(BASE_SRC_FILES))
BASE_SRC_FILES := $(filter-out prj/Websocket/Server.cpp, $(BASE_SRC_FILES))

ROVER_SRC_FILES := $(wildcard prj/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Systems/Drive.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Systems/Arm.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Handlers/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Websocket/*.cpp)

ROVER_SRC_FILES := $(filter-out prj/Websocket/Client.cpp, $(ROVER_SRC_FILES))
ROVER_SRC_FILES := $(filter-out prj/Websocket/Server.cpp, $(ROVER_SRC_FILES))

BOOST_ROOT = /usr/include
SDL_ROOT = tools/SDL
IK_root = tools/IK

CPP_STD = -std=c++17

CMPL_BOOST = -I $(BOOST_ROOT)
CMPL_SDL = -I$(SDL_ROOT)/include -L$(SDL_ROOT)/lib -lSDL2
CMPL_PHIDETS = -lphidget22
CMPL_JSON = -ljsoncpp
CMPL_PTHREAD = -lpthread
CMPL_IK = -I$(IK_root)/include

NULL_DEVICE := $(if $(filter Windows_NT,$(OS)),NUL,/dev/null)

run_test:
> cmake -S ./test -B test/build
> cmake --build test/build
> cd test/build && ctest

controller_test:
> g++ -I"prj" prj/Base/ControllerTesting.cpp -o ControllerTesting -I"C:/mingw-dev-lib/include/SDL2" -L"C:/mingw-dev-lib/lib" -lmingw32 -lSDL2main -lSDL2
> ./ControllerTesting

build_dir:
> mkdir -p out
> mkdir -p logs

base: build_dir
> g++ $(CPP_STD) -DSIDE_TO_BUILD=1 -I"prj" $(BASE_SRC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(CMPL_PHIDETS) $(CMPL_PTHREAD) -o out/base

rover: build_dir
> g++ $(CPP_STD) -DSIDE_TO_BUILD=2 -I"prj" $(CMPL_IK) $(ROVER_SRC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(CMPL_PHIDETS) $(CMPL_JSON) $(CMPL_PTHREAD) -o out/rover

run_base: base
> ./base 

run_rover: rover
> ./rover 

clean:
> rm -f out/ -r
> rm -f logs/*.log

all: build_dir base rover
