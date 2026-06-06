.RECIPEPREFIX = >
BASE_SRC_FILES := $(wildcard prj/*.cpp)
BASE_SRC_FILES += $(wildcard prj/Base/*.cpp)
BASE_SRC_FILES += $(wildcard prj/Base/Models/*.cpp)
BASE_SRC_FILES += $(wildcard prj/RoverState/*.cpp)
BASE_SRC_FILES += $(wildcard prj/RoverState/StateManagers/*.cpp)
BASE_SRC_FILES += $(wildcard prj/Controller/*.cpp)

ROVER_SRC_FILES := $(wildcard prj/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Systems/Drive.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Systems/Arm.cpp)
ROVER_SRC_FILES += $(wildcard prj/RoverState/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/RoverState/StateManagers/*.cpp)
ROVER_SRC_FILES += $(wildcard prj/Rover/Handlers/*.cpp)

BASE_INC_FILES := -I prj
BASE_INC_FILES += -I prj/Base
BASE_INC_FILES  += -I prj/RoverState
BASE_INC_FILES  += -I prj/RoverState/StateManagers
BASE_INC_FILES  += -I prj/Controller

ROVER_INC_FILES  += -I prj/RoverState
ROVER_INC_FILES  += -I prj/RoverState/StateManagers

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
> g++ $(CPP_STD) $(CMPL_SDL) test/controllerTest.cpp -o out/ControllerTest
> ./out/ControllerTest

build_dir:
> mkdir -p out
> mkdir -p logs

base: build_dir
> g++ $(CPP_STD) -DSIDE_TO_BUILD=1 $(BASE_INC_FILES) $(BASE_SRC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(CMPL_IK) $(CMPL_PHIDETS) $(CMPL_PTHREAD) -o out/base

rover: build_dir
> g++ $(CPP_STD) -DSIDE_TO_BUILD=2 -I"prj" $(ROVER_SRC_FILES) $(ROVER_INC_FILES) $(CMPL_BOOST) $(CMPL_SDL) $(CMPL_PHIDETS) $(CMPL_JSON) $(CMPL_PTHREAD) -o out/rover

run_base: base
> ./base 

run_rover: rover
> ./rover 

clean:
> rm -f out/ -r
> rm -f logs/*.log

all: build_dir base rover
