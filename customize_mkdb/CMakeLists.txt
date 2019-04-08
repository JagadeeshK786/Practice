cmake_minimum_required(VERSION 3.3)
project(mkdb C CXX)

include_directories(/Library/Java/JavaVirtualMachines/jdk1.8.0_201.jdk/Contents/Home/include)
link_directories(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
    mkdb_Storage.h
    mkdb_Storage_IWalk.h
    mkdb_Storage_KeyIterator.h
    mkdb_Storage.cpp
    mkdb_Storage_IWalk.cpp
    mkdb_Storage_KeyIterator.cpp)

add_library(db SHARED ${SOURCE_FILES})
set_target_properties(db PROPERTIES OUTPUT_NAME "db_x86_64")
target_link_libraries(db hiredis)