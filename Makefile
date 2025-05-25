.RECIPEPREFIX = >

run_test:
> cmake -S ./test -B test/build
> cmake --build test/build
> cd test/build && ctest

compile:
> g++  -I"prj" prj/main.cpp -o main

run: compile
> ./main.exe