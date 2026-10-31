cmake_minimum_required(VERSION 3.15)
project(SmartCareHospital CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Fetch Asio
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Asio headers")
include_directories(${ASIO_INCLUDE_DIR})

# 2. Fetch nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# 3. Fetch Crow (with internal asio search bypassed)
set(CROW_FEATURES "OFF" CACHE BOOL "" FORCE)
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

add_executable(smartcare_backend backend/main.cpp)

target_include_directories(smartcare_backend PRIVATE ${ASIO_INCLUDE_DIR})

target_link_libraries(smartcare_backend PRIVATE 
    Crow::Crow 
    nlohmann_json::nlohmann_json
    ws2_32
    mswsock
)

# Link SQLite3
find_package(sqlite3 REQUIRED)

