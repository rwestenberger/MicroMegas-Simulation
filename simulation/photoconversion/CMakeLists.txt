# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(MM)

set(COG_EXEC cog.py)

set(ENV{PYTHONPATH} ..)

# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
	find_package(Geant4 REQUIRED)
endif()

# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# get root config
EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")
EXECUTE_PROCESS(COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_EXE_LINKER_FLAGS ${ROOT_LD_FLAGS})

# Locate sources and headers for this project
file(GLOB main_source ${PROJECT_SOURCE_DIR}/photoconversion.cpp)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB macros ${PROJECT_SOURCE_DIR}/*.mac)
file(GLOB cog_conf ${PROJECT_SOURCE_DIR}/../simulation.conf)

# Target for source file generation
add_custom_command(
	OUTPUT ${main_source} ${sources} ${headers} ${macros}
	COMMAND ${COG_EXEC} --verbosity=1 -r ${main_source} ${sources} ${headers} ${macros}
	DEPENDS ${cog_conf}
	COMMENT "Cogging source files..."
)

# Add the executable, and link it to the Geant4 libraries
add_executable(photoconversion photoconversion.cpp ${sources} ${headers})
target_link_libraries(photoconversion ${Geant4_LIBRARIES})

# Make sure that only one target uses the sources at once
add_custom_target(CogFiles DEPENDS ${main_source} ${sources} ${headers} COMMENT "Checking if re-cogging is required")
add_dependencies(photoconversion CogFiles)

# For internal Geant4 use - but has no effect if you build this standalone
add_custom_target(MM DEPENDS photoconversion)
