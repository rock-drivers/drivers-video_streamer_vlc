rock_library(video_streamer_vlc
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(video_streamer_vlc_bin Main.cpp
    DEPS video_streamer_vlc)

