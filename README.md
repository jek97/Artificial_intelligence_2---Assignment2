This is the repository of an assignment II of Artificial Intelligence II course at University of Genoa, Italy.
In order to run the code, follow the following steps:

1. Git clone the repository
```
git clone https://github.com/jek97/Artificial_intelligence_2---Assignment2.git
```
2. Go to visits_module
3. Build the module
```
mkdir build
cd build
```
```
cmake -DCMAKE_VERBOSE_MAKEFILE=TRUE -DCMAKE_BUILD_TYPE=Release ../src/
```
```
make
```
4. Command to launch the planner:
```
../../popf-tif/popf3-clp -x dom1.pddl prob1.pddl ../visits_module/build/libVisits.so region.txt
```
