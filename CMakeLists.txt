cmake_minimum_required(VERSION 3.16)

project(LIVE_CLIENT)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

# set(CMAKE_C_FLAGS "-Wall -fPIC -O0 -g")
# set(CMAKE_CXX_FLAGS "-Wall -fPIC -O0 -g")

set(CMAKE_C_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-g")

set(MPP_INCLUDE_PATH "/usr/local/include/rockchip/")
set(MPP_LIB "/usr/lib")

if(NOT DEFINED ENV{LIVE555_INCLUDE})
    set(LIVE555_INCLUDE "/usr/local/live555/include/")
else()
    set(LIVE555_INCLUDE $ENV{LIVE555_INCLUDE})
endif()

if(NOT DEFINED ENV{LIVE555_LIB})
    set(LIVE555_LIB "/usr/local/live555/lib/")
else()
    set(LIVE555_LIB $ENV{LIVE555_LIB})
endif()

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

message(STATUS "live555 include :${LIVE555_INCLUDE}")
message(STATUS "live555 lib :${LIVE555_LIB}")

include_directories(
    ./include/
    ${MPP_INCLUDE_PATH}
    ${LIVE555_INCLUDE}/BasicUsageEnvironment/
    ${LIVE555_INCLUDE}/groupsock/
    ${LIVE555_INCLUDE}/liveMedia/
    ${LIVE555_INCLUDE}/UsageEnvironment/
)

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(CPP_IDX "1")
message(STATUS "the all cpp files listed as below:")
foreach(item  ${CPP_FILES})
    message(STATUS "\t [${CPP_IDX}] ${item}")
    math(EXPR CPP_IDX "${CPP_IDX} + 1")
endforeach()

link_directories(
    ${MPP_LIB}
    ${LIVE555_LIB}
)

list(REMOVE_ITEM CPP_FILES ${PROJECT_SOURCE_DIR}/src/drm_test.cpp ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp ${CPP_FILES})

target_link_libraries(main -lrockchip_mpp -lpthread -ldrm -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment -lssl -lcrypto ${OpenCV_LIBS})

add_executable(drmTest ${PROJECT_SOURCE_DIR}/src/drm_test.cpp)

target_link_libraries(drmTest -lpthread -ldrm)