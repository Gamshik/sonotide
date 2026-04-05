cmake_minimum_required(VERSION 3.24)

project(SonotideSmokeConsumer LANGUAGES CXX)

find_package(Sonotide CONFIG REQUIRED)

add_executable(sonotide_smoke_consumer main.cpp)
target_link_libraries(sonotide_smoke_consumer PRIVATE Sonotide::sonotide)
target_compile_features(sonotide_smoke_consumer PRIVATE cxx_std_20)
