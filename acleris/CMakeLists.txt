find_package(SDL2 REQUIRED)

add_library(Acleris STATIC
        Acleris.h
        Acleris.cpp
        util/Format.h
        util/StaticFor.h
        util/NArray.h
        )

if (!${SDL2_FOUND})
    message(FATAL "Could not find SDL2")
endif()

set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -O2")

message(STATUS "Found SDL at ${SDL2_INCLUDE_DIR}")

target_include_directories(Acleris PUBLIC ${SDL2_INCLUDE_DIR})

target_link_libraries(Acleris PRIVATE ${SDL2_LIBRARY})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # copy over SDL2.dll, does not happen automatically for me for some reason
    message("Copying SDL2.dll to ${CMAKE_BINARY_DIR}")
    file(COPY ${CMAKE_SOURCE_DIR}/files/SDL2.dll
            DESTINATION ${CMAKE_BINARY_DIR})
endif()