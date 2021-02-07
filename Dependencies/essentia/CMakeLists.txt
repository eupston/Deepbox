project(Essentia)
cmake_minimum_required(VERSION 3.11)

set(ESSENTIA_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

#file(GLOB_RECURSE ESSENTIA_HEADERS_FILES ${ESSENTIA_INC_DIR}/*.h)

add_library(${PROJECT_NAME} INTERFACE)
#set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#TODO create these static libs via cmake or use CPM and my own fork of essentia with cmake
target_link_libraries(${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/lib/clang/static/libfftw3f.a
        ${CMAKE_CURRENT_LIST_DIR}/lib/clang/static/libessentia.a
)

target_include_directories(${PROJECT_NAME} INTERFACE ${ESSENTIA_INC_DIR})
