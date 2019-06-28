CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
include("../CMake/CMakeDefine.inc")

# DEBUG 模式
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

###
# 工程
project(HFrame)

# 包含目录
include_directories(${PROJECT_SOURCE_DIR})

# 公共目录定义
set(PATH_BIN ${PROJECT_SOURCE_DIR}/Bin)

# 项目
add_subdirectory(Core)
add_subdirectory(Source)