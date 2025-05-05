# Introduction
This is a simple PC club simulator! <br />
An EventReader class reads and parses contents form file and then EventFactory class creates events accroding to their type.
All created events are added to the object of Club class, then the club executes them.
# Requirements
**C++20** compiler,
**CMake**,
build system (eg. **make** or **ninja**)
# Installation
Run the following commands:
```
cd PCClub
mkdir build
cd build
cmake ..
make 
```
# Use
./club_simulator ../tests/test.txt
