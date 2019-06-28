CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

project(HExample)

include_directories(../HBase)
include_directories(../Module)

# Assets
file(GLOB_RECURSE SRCS "Assets/*.cpp" "Assets/*.h")
add_executable("Assets" ${SRCS})
target_link_libraries("Assets" HBase)
FUNC_OUTPUT("Assets" ${PATH_BIN})
set_target_properties(Assets PROPERTIES FOLDER HExample)

# Audio
file(GLOB_RECURSE SRCS "Audio/*.cpp" "Audio/*.h")
add_executable("Audio" ${SRCS})
target_link_libraries("Audio" HBase)
FUNC_OUTPUT("Audio" ${PATH_BIN})
set_target_properties(Audio PROPERTIES FOLDER HExample)

# DemoRunner
file(GLOB_RECURSE SRCS "DemoRunner/*.cpp" "DemoRunner/*.h")
add_executable("DemoRunner" ${SRCS})
target_link_libraries("DemoRunner" HBase)
FUNC_OUTPUT("DemoRunner" ${PATH_BIN})
set_target_properties(DemoRunner PROPERTIES FOLDER HExample)

# DSP
file(GLOB_RECURSE SRCS "DSP/*.cpp" "DSP/*.h")
add_executable("DSP" ${SRCS})
target_link_libraries("DSP" HBase)
FUNC_OUTPUT("DSP" ${PATH_BIN})
set_target_properties(DSP PROPERTIES FOLDER HExample)

# GUI
file(GLOB_RECURSE SRCS "GUI/*.cpp" "GUI/*.h")
add_executable("GUI" ${SRCS})
target_link_libraries("GUI" HBase)
FUNC_OUTPUT("GUI" ${PATH_BIN})
set_target_properties(GUI PROPERTIES FOLDER HExample)

# Plugins
file(GLOB_RECURSE SRCS "Plugins/*.cpp" "Plugins/*.h")
add_executable("Plugins" ${SRCS})
target_link_libraries("Plugins" HBase)
FUNC_OUTPUT("Plugins" ${PATH_BIN})
set_target_properties(Plugins PROPERTIES FOLDER HExample)

# Utilities
file(GLOB_RECURSE SRCS "Utilities/*.cpp" "Utilities/*.h")
add_executable("Utilities" ${SRCS})
target_link_libraries("Utilities" HBase)
FUNC_OUTPUT("Utilities" ${PATH_BIN})
set_target_properties(Utilities PROPERTIES FOLDER HExample)


