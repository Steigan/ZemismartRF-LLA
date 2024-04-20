cmake_minimum_required (VERSION 3.13)

project(ZRFAnalyzer)

add_definitions( -DLOGIC2 )

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
src/ZRFAnalyzer.cpp
src/ZRFAnalyzer.h
src/ZRFAnalyzerResults.cpp
src/ZRFAnalyzerResults.h
src/ZRFAnalyzerSettings.cpp
src/ZRFAnalyzerSettings.h
src/ZRFSimulationDataGenerator.cpp
src/ZRFSimulationDataGenerator.h
)

add_analyzer_plugin(${PROJECT_NAME} SOURCES ${SOURCES})
