

find_package(Qt5 COMPONENTS Core REQUIRED)

file(GLOB QDLT645_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

add_library(qdlt645 SHARED ${QDLT645_SRCS})
target_include_directories(qdlt645 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(qdlt645 Qt5::Core)
target_compile_definitions(qdlt645 PUBLIC -DQDLT645_LIB)
