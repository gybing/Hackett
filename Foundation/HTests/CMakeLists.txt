CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

set(TARGET HBaseTest)
project(${TARGET})

#生成可执行的文件
add_executable(${TARGET} "HBaseTest.cpp")
target_link_libraries(${TARGET} HBase)
FUNC_OUTPUT(${TARGET} ${PATH_BIN})