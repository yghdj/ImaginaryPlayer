cmake_minimum_required(VERSION 3.15...3.25)

project(ImaginaryPlayer
    VERSION
        1.0
    DESCRIPTION
        "A simple music player"
    LANGUAGES
        CXX
)

add_executable(iplayer)

target_include_directories(iplayer PUBLIC ../include)
target_compile_features(iplayer PUBLIC c_std_17)
target_sources(iplayer PUBLIC Codec.cpp MusicPlayer.cpp Shell.cpp Track.cpp Utils.cpp)

