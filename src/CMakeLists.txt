rock_library(video_streamer_vlc
    SOURCES VlcStream.cpp
    HEADERS VlcStream.hpp
    DEPS_PKGCONFIG opencv
    LIBS vlc)

rock_executable(video_streamer_vlc_bin Main.cpp
    DEPS video_streamer_vlc)
