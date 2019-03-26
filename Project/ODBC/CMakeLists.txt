aux_source_directory(. SRCS)
file(GLOB_RECURSE INCS "*.h")

include_directories(.)
include_directories(${BASE_DIR})

add_library(odbc++ STATIC ${SRCS} ${INCS})
func_target_output(odbc++ ${LIB_DIR})

func_target_disablewarning_msvc(odbc++ 4290)

add_dependencies(odbc++ base)