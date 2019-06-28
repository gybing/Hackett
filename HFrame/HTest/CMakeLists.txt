CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

include_directories(../HBase)

# HFrameTest
add_executable("HFrameTest" "HBaseTest.cpp")
set_property(TARGET HFrameTest PROPERTY FOLDER HTest)
target_link_libraries("HFrameTest" HBase)
FUNC_OUTPUT("HFrameTest" ${PATH_BIN})

# HFrameUnitTest
add_executable("HFrameUnitTest" "HBaseUnitTestMain.cpp")
set_property(TARGET HFrameUnitTest PROPERTY FOLDER HTest)
target_link_libraries("HFrameUnitTest" HBase)
FUNC_OUTPUT("HFrameUnitTest" ${PATH_BIN})