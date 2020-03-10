# Calculator Example using ZserioServiceGrpc library
cmake_minimum_required(VERSION 2.8.12.2)

project(Calculator)

find_package(Java COMPONENTS Runtime)

set(REPOSITORY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# generate Zserio Calculator Pub/Sub
get_filename_component(ZSERIO_JAR "${REPOSITORY_ROOT}/3rdparty/zserio.jar" ABSOLUTE)
get_filename_component(CALCULATOR_ZS_FULL "calculator.zs" ABSOLUTE)
get_filename_component(CALCULATOR_ZS "${CALCULATOR_ZS_FULL}" NAME)
get_filename_component(CALCULATOR_ZS_PATH "${CALCULATOR_ZS_FULL}" PATH)

set(CALCULATOR_GEN_SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/I32.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/I32.h"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/U64.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/U64.h"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/Double.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/Double.h"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/CalculatorClient.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/CalculatorClient.h"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/PowerOfTwoProvider.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/PowerOfTwoProvider.h"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/SquareRootOfProvider.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/calculator/SquareRootOfProvider.h"
)

add_custom_command(
    OUTPUT ${CALCULATOR_GEN_SOURCES}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ZSERIO_JAR}
    ARGS
        -src "${CALCULATOR_ZS_PATH}" ${CALCULATOR_ZS}
        -cpp "${CMAKE_CURRENT_BINARY_DIR}" -withoutSourcesAmalgamation
    DEPENDS "${CALCULATOR_ZS}"
)

# generated source (needed to generate ${CALCULATOR_GEN_SOURCES} only once)
add_library(CalculatorGen OBJECT ${CALCULATOR_GEN_SOURCES})

# client
add_executable(CalculatorClient
    CalculatorClient.cpp
    $<TARGET_OBJECTS:CalculatorGen>
)

# power of two provider
add_executable(PowerOfTwoProvider
    PowerOfTwoProvider.cpp
    $<TARGET_OBJECTS:CalculatorGen>
)

# square root of provider
add_executable(SquareRootOfProvider
    SquareRootOfProvider.cpp
    $<TARGET_OBJECTS:CalculatorGen>
)

# generated sources
target_include_directories(CalculatorGen PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(CalculatorClient PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(PowerOfTwoProvider PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_include_directories(SquareRootOfProvider PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# add ZserioPubsubMosquitto library
add_subdirectory("${REPOSITORY_ROOT}/src" "${CMAKE_CURRENT_BINARY_DIR}/ZserioPubsubMosquitto")
target_include_directories(CalculatorClient PRIVATE "${REPOSITORY_ROOT}/src")
target_include_directories(PowerOfTwoProvider PRIVATE "${REPOSITORY_ROOT}/src")
target_include_directories(SquareRootOfProvider PRIVATE "${REPOSITORY_ROOT}/src")
target_link_libraries(CalculatorClient ZserioPubsubMosquitto)
target_link_libraries(PowerOfTwoProvider ZserioPubsubMosquitto)
target_link_libraries(SquareRootOfProvider ZserioPubsubMosquitto)

# add ZserioCppRuntime library
add_subdirectory("${REPOSITORY_ROOT}/3rdparty/runtime" "${CMAKE_CURRENT_BINARY_DIR}/ZserioCppRuntime")
target_include_directories(CalculatorGen PRIVATE "${REPOSITORY_ROOT}/3rdparty/runtime")
target_link_libraries(CalculatorClient ZserioCppRuntime)
target_link_libraries(PowerOfTwoProvider ZserioCppRuntime)
target_link_libraries(SquareRootOfProvider ZserioCppRuntime)
