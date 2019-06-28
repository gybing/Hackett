CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

project(HModule)

include_directories(../HBase)

# analytics
file(GLOB_RECURSE SRCS "analytics/*.cpp" "analytics/*.h")
add_executable("analytics" ${SRCS})
target_link_libraries("analytics" HBase)
FUNC_OUTPUT("analytics" ${PATH_BIN})
set_target_properties(analytics PROPERTIES FOLDER HModule)

# audio_basics
file(GLOB_RECURSE SRCS "audio_basics/*.cpp" "audio_basics/*.h")
add_executable("audio_basics" ${SRCS})
target_link_libraries("audio_basics" HBase)
FUNC_OUTPUT("audio_basics" ${PATH_BIN})
set_target_properties(audio_basics PROPERTIES FOLDER HModule)

# audio_devices
file(GLOB_RECURSE SRCS "audio_devices/*.cpp" "audio_devices/*.h")
add_executable("audio_devices" ${SRCS})
target_link_libraries("audio_devices" HBase)
FUNC_OUTPUT("audio_devices" ${PATH_BIN})
set_target_properties(audio_devices PROPERTIES FOLDER HModule)

# audio_formats
file(GLOB_RECURSE SRCS "audio_formats/*.cpp" "audio_formats/*.h")
add_executable("audio_formats" ${SRCS})
target_link_libraries("audio_formats" HBase)
FUNC_OUTPUT("audio_formats" ${PATH_BIN})
set_target_properties(audio_formats PROPERTIES FOLDER HModule)

# audio_plugin_client
file(GLOB_RECURSE SRCS "audio_plugin_client/*.cpp" "audio_plugin_client/*.h")
add_executable("audio_plugin_client" ${SRCS})
target_link_libraries("audio_plugin_client" HBase)
FUNC_OUTPUT("audio_plugin_client" ${PATH_BIN})
set_target_properties(audio_plugin_client PROPERTIES FOLDER HModule)

# audio_processors
file(GLOB_RECURSE SRCS "audio_processors/*.cpp" "audio_processors/*.h")
add_executable("audio_processors" ${SRCS})
target_link_libraries("audio_processors" HBase)
FUNC_OUTPUT("audio_processors" ${PATH_BIN})
set_target_properties(audio_processors PROPERTIES FOLDER HModule)

# audio_utils
file(GLOB_RECURSE SRCS "audio_utils/*.cpp" "audio_utils/*.h")
add_executable("audio_utils" ${SRCS})
target_link_libraries("audio_utils" HBase)
FUNC_OUTPUT("audio_utils" ${PATH_BIN})
set_target_properties(audio_utils PROPERTIES FOLDER HModule)

# blocks_basics
file(GLOB_RECURSE SRCS "blocks_basics/*.cpp" "blocks_basics/*.h")
add_executable("blocks_basics" ${SRCS})
target_link_libraries("blocks_basics" HBase)
FUNC_OUTPUT("blocks_basics" ${PATH_BIN})
set_target_properties(blocks_basics PROPERTIES FOLDER HModule)

# box2d
file(GLOB_RECURSE SRCS "box2d/*.cpp" "box2d/*.h")
add_executable("box2d" ${SRCS})
target_link_libraries("box2d" HBase)
FUNC_OUTPUT("box2d" ${PATH_BIN})
set_target_properties(box2d PROPERTIES FOLDER HModule)

# cryptography
file(GLOB_RECURSE SRCS "cryptography/*.cpp" "cryptography/*.h")
add_executable("cryptography" ${SRCS})
target_link_libraries("cryptography" HBase)
FUNC_OUTPUT("cryptography" ${PATH_BIN})
set_target_properties(cryptography PROPERTIES FOLDER HModule)

# data_structures
file(GLOB_RECURSE SRCS "data_structures/*.cpp" "data_structures/*.h")
add_executable("data_structures" ${SRCS})
target_link_libraries("data_structures" HBase)
FUNC_OUTPUT("data_structures" ${PATH_BIN})
set_target_properties(data_structures PROPERTIES FOLDER HModule)

# dsp
file(GLOB_RECURSE SRCS "dsp/*.cpp" "dsp/*.h")
add_executable("dsp" ${SRCS})
target_link_libraries("dsp" HBase)
FUNC_OUTPUT("dsp" ${PATH_BIN})
set_target_properties(dsp PROPERTIES FOLDER HModule)

# events
file(GLOB_RECURSE SRCS "events/*.cpp" "events/*.h")
add_executable("events" ${SRCS})
target_link_libraries("events" HBase)
FUNC_OUTPUT("events" ${PATH_BIN})
set_target_properties(events PROPERTIES FOLDER HModule)

# graphics
file(GLOB_RECURSE SRCS "graphics/*.cpp" "graphics/*.h")
add_executable("graphics" ${SRCS})
target_link_libraries("graphics" HBase)
FUNC_OUTPUT("graphics" ${PATH_BIN})
set_target_properties(graphics PROPERTIES FOLDER HModule)

# gui_basics
file(GLOB_RECURSE SRCS "gui_basics/*.cpp" "gui_basics/*.h")
add_executable("gui_basics" ${SRCS})
target_link_libraries("gui_basics" HBase)
FUNC_OUTPUT("gui_basics" ${PATH_BIN})
set_target_properties(gui_basics PROPERTIES FOLDER HModule)

# gui_extra
file(GLOB_RECURSE SRCS "gui_extra/*.cpp" "gui_extra/*.h")
add_executable("gui_extra" ${SRCS})
target_link_libraries("gui_extra" HBase)
FUNC_OUTPUT("gui_extra" ${PATH_BIN})
set_target_properties(gui_extra PROPERTIES FOLDER HModule)

# opengl
file(GLOB_RECURSE SRCS "opengl/*.cpp" "opengl/*.h")
add_executable("opengl" ${SRCS})
target_link_libraries("opengl" HBase)
FUNC_OUTPUT("opengl" ${PATH_BIN})
set_target_properties(opengl PROPERTIES FOLDER HModule)

# osc
file(GLOB_RECURSE SRCS "osc/*.cpp" "osc/*.h")
add_executable("osc" ${SRCS})
target_link_libraries("osc" HBase)
FUNC_OUTPUT("osc" ${PATH_BIN})
set_target_properties(osc PROPERTIES FOLDER HModule)

# product_unlocking
file(GLOB_RECURSE SRCS "product_unlocking/*.cpp" "product_unlocking/*.h")
add_executable("product_unlocking" ${SRCS})
target_link_libraries("product_unlocking" HBase)
FUNC_OUTPUT("product_unlocking" ${PATH_BIN})
set_target_properties(product_unlocking PROPERTIES FOLDER HModule)

# video
file(GLOB_RECURSE SRCS "video/*.cpp" "video/*.h")
add_executable("video" ${SRCS})
target_link_libraries("video" HBase)
FUNC_OUTPUT("video" ${PATH_BIN})
set_target_properties(video PROPERTIES FOLDER HModule)
