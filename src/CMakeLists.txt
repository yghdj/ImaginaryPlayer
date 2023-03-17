add_executable(iplayer)

target_include_directories(iplayer PUBLIC ../include)
target_compile_features(iplayer PUBLIC cxx_std_17)
target_sources(iplayer PUBLIC Codec.cpp HelpMessages.cpp MusicPlayer.cpp Shell.cpp Track.cpp Utils.cpp)