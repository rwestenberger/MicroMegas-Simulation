source /localscratch/simulation/installs/geant4/share/Geant4-10.1.2/geant4make/geant4make.sh
cmake -DGeant4_DIR=/localscratch/simulation/installs/geant4/lib/Geant4-10.1.2/ .
make -j 2 exampleB1
