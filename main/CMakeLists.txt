idf_component_register(
    SRC_DIRS "." "sys" "app" "app/gnuboy"
    INCLUDE_DIRS "." )

file(GLOB FAST_SRCS "app/gnuboy/*.c")
set_source_files_properties(${FAST_SRCS} PROPERTIES COMPILE_FLAGS "-O3")
