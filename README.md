to install:
1. git clone
2. go to visits_module
3. mkdir build
4. cd build
5. cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE -DCMAKE_BUILD_TYPE=Release ../src/
6. make
command to launch the planner:
../../popf-tif/popf3-clp -x dom1.pddl prob1.pddl ../visits_module/build/libVisits.so region.txt
