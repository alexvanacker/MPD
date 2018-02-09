cmake_minimum_required(VERSION 2.8)
#SET(CMAKE_C_FLAGS     "-w -g -O3 -fbounds-checking")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

project(MPD)
file(                  GLOB_RECURSE source_files sources/*)
add_executable(        mpdProgram ${source_files})
FIND_LIBRARY(          M_LIB m)
SET(                   LIBRARIES ${M_LIB} "-llapacke" "-fopenmp")
target_link_libraries( mpdProgram ${LIBRARIES})
INSTALL(               TARGETS mpdProgram RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/bin")


 



