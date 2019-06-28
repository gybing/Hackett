CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

project(HExtras)

include_directories(../HBase)

# AudioPerformanceTest
file(GLOB_RECURSE SRCS "AudioPerformanceTest/*.cpp" "AudioPerformanceTest/*.h")
add_executable("AudioPerformanceTest" ${SRCS})
target_link_libraries("AudioPerformanceTest" HBase)
FUNC_OUTPUT("AudioPerformanceTest" ${PATH_BIN})
set_target_properties(AudioPerformanceTest PROPERTIES FOLDER HExtras)

# AudioPluginHost
file(GLOB_RECURSE SRCS "AudioPluginHost/*.cpp" "AudioPluginHost/*.h")
add_executable("AudioPluginHost" ${SRCS})
target_link_libraries("AudioPluginHost" HBase)
FUNC_OUTPUT("AudioPluginHost" ${PATH_BIN})
set_target_properties(AudioPluginHost PROPERTIES FOLDER HExtras)

# BinaryBuilder
file(GLOB_RECURSE SRCS "BinaryBuilder/*.cpp" "BinaryBuilder/*.h")
add_executable("BinaryBuilder" ${SRCS})
target_link_libraries("BinaryBuilder" HBase)
FUNC_OUTPUT("BinaryBuilder" ${PATH_BIN})
set_target_properties(BinaryBuilder PROPERTIES FOLDER HExtras)

# BLOCKS
file(GLOB_RECURSE SRCS "BLOCKS/*.cpp" "BLOCKS/*.h")
add_executable("BLOCKS" ${SRCS})
target_link_libraries("BLOCKS" HBase)
FUNC_OUTPUT("BLOCKS" ${PATH_BIN})
set_target_properties(BLOCKS PROPERTIES FOLDER HExtras)

# NetworkGraphicsDemo
file(GLOB_RECURSE SRCS "NetworkGraphicsDemo/*.cpp" "NetworkGraphicsDemo/*.h")
add_executable("NetworkGraphicsDemo" ${SRCS})
target_link_libraries("NetworkGraphicsDemo" HBase)
FUNC_OUTPUT("NetworkGraphicsDemo" ${PATH_BIN})
set_target_properties(NetworkGraphicsDemo PROPERTIES FOLDER HExtras)

# Projector
file(GLOB_RECURSE SRCS "Projector/*.cpp" "Projector/*.h")
add_executable("Projector" ${SRCS})
target_link_libraries("Projector" HBase)
FUNC_OUTPUT("Projector" ${PATH_BIN})
set_target_properties(Projector PROPERTIES FOLDER HExtras)

# UnitTestRunner
file(GLOB_RECURSE SRCS "UnitTestRunner/*.cpp" "UnitTestRunner/*.h")
add_executable("UnitTestRunner" ${SRCS})
target_link_libraries("UnitTestRunner" HBase)
FUNC_OUTPUT("UnitTestRunner" ${PATH_BIN})
set_target_properties(UnitTestRunner PROPERTIES FOLDER HExtras)

# WindowsDLL
file(GLOB_RECURSE SRCS "WindowsDLL/*.cpp" "WindowsDLL/*.h")
add_executable("WindowsDLL" ${SRCS})
target_link_libraries("WindowsDLL" HBase)
FUNC_OUTPUT("WindowsDLL" ${PATH_BIN})
set_target_properties(WindowsDLL PROPERTIES FOLDER HExtras)