SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

#主工程
FILE(GLOB SRC "*.h" "*.cpp")
INCLUDE_DIRECTORIES(.)

#Juce库 不加入生成
FILE(GLOB_RECURSE juce_audio_basics "juce_audio_basics/*.*")
FILE(GLOB_RECURSE juce_audio_devices "juce_audio_devices/*.*")
FILE(GLOB_RECURSE juce_audio_formats "juce_audio_formats/*.*")
FILE(GLOB_RECURSE juce_audio_plugin_client "juce_audio_plugin_client/*.*")
FILE(GLOB_RECURSE juce_audio_processors "juce_audio_processors/*.*")
FILE(GLOB_RECURSE juce_audio_utils "juce_audio_utils/*.*")
FILE(GLOB_RECURSE juce_box2d "juce_box2d/*.*")
FILE(GLOB_RECURSE juce_core "juce_core/*.*")
FILE(GLOB_RECURSE juce_cryptography "juce_cryptography/*.*")
FILE(GLOB_RECURSE juce_data_structures "juce_data_structures/*.*")
FILE(GLOB_RECURSE juce_events "juce_events/*.*")
FILE(GLOB_RECURSE juce_graphics "juce_graphics/*.*")
FILE(GLOB_RECURSE juce_gui_basics "juce_gui_basics/*.*")
FILE(GLOB_RECURSE juce_gui_extra "juce_gui_extra/*.*")
FILE(GLOB_RECURSE juce_opengl "juce_opengl/*.*")
FILE(GLOB_RECURSE juce_osc "juce_osc/*.*")
FILE(GLOB_RECURSE juce_tracktion_marketplace "juce_tracktion_marketplace/*.*")
FILE(GLOB_RECURSE juce_video "juce_video/*.*")

FILE(GLOB JuceFiles ${juce_audio_basics}
${juce_audio_devices}
${juce_audio_formats}
${juce_audio_plugin_clien}
${juce_audio_processors}
${juce_audio_utils}
${juce_box2d}
${juce_core}
${juce_cryptography}
${juce_data_structures}
${juce_events}
${juce_graphics}
${juce_gui_basics}
${juce_gui_extra}
${juce_opengl}
${juce_osc}
${juce_tracktion_marketplace}
${juce_video})

set_source_files_properties(${JuceFiles} PROPERTIES 
  HEADER_FILE_ONLY TRUE) 

ADD_DEFINITIONS(-DJUCE_DLL_BUILD)
ADD_LIBRARY(AltQBase SHARED ${SRC} ${JuceFiles})

SOURCE_GROUP("juce_audio_basics" FILES ${juce_audio_basics})
SOURCE_GROUP("juce_audio_devices" FILES ${juce_audio_devices})
SOURCE_GROUP("juce_audio_formats" FILES ${juce_audio_formats})
SOURCE_GROUP("juce_audio_plugin_clien" FILES ${juce_audio_plugin_clien})
SOURCE_GROUP("juce_audio_processors" FILES ${juce_audio_processors})
SOURCE_GROUP("juce_audio_utils" FILES ${juce_audio_utils})
SOURCE_GROUP("juce_box2d" FILES ${juce_box2d})
SOURCE_GROUP("juce_core" FILES ${juce_core})
SOURCE_GROUP("juce_cryptography" FILES ${juce_cryptography})
SOURCE_GROUP("juce_data_structures" FILES ${juce_data_structures})
SOURCE_GROUP("juce_events" FILES ${juce_events})
SOURCE_GROUP("juce_graphics" FILES ${juce_graphics})
SOURCE_GROUP("juce_gui_basics" FILES ${juce_gui_basics})
SOURCE_GROUP("juce_gui_extra" FILES ${juce_gui_extra})
SOURCE_GROUP("juce_opengl" FILES ${juce_opengl})
SOURCE_GROUP("juce_osc" FILES ${juce_osc})
SOURCE_GROUP("juce_tracktion_marketplace" FILES ${juce_tracktion_marketplace})
SOURCE_GROUP("juce_video" FILES ${juce_video})