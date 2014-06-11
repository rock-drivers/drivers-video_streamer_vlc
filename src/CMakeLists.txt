
if(WIN32)

project(video_streamer_vlc)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "small plugin that uses opencv to play movie on a osg texture for mars")
set(CMAKE_BUILD_TYPE Release)

        # this fixes the error 998 from the LibManager
        SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-auto-import")
        SET(CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-auto-import")


set(SOURCES 
	VlcStream.cpp
)
set(HEADERS
	VlcStream.hpp
)


set (prefix c:/dev_mars_cave/demos/experimental/20140205_MoviePlayer/install)
set (exec_prefix ${prefix})
set (OPENCV_LIBRARIES ${exec_prefix}/lib/libopencv_calib3d248.dll.a ${exec_prefix}/lib/libopencv_contrib248.dll.a ${exec_prefix}/lib/libopencv_core248.dll.a ${exec_prefix}/lib/libopencv_features2d248.dll.a ${exec_prefix}/lib/libopencv_flann248.dll.a ${exec_prefix}/lib/libopencv_gpu248.dll.a ${exec_prefix}/lib/libopencv_highgui248.dll.a ${exec_prefix}/lib/libopencv_imgproc248.dll.a ${exec_prefix}/lib/libopencv_legacy248.dll.a ${exec_prefix}/lib/libopencv_ml248.dll.a ${exec_prefix}/lib/libopencv_nonfree248.dll.a ${exec_prefix}/lib/libopencv_objdetect248.dll.a ${exec_prefix}/lib/libopencv_ocl248.dll.a ${exec_prefix}/lib/libopencv_photo248.dll.a ${exec_prefix}/lib/libopencv_stitching248.dll.a ${exec_prefix}/lib/libopencv_superres248.dll.a ${exec_prefix}/lib/libopencv_ts248.a ${exec_prefix}/bin/libopencv_video248.dll ${exec_prefix}/lib/libopencv_videostab248.dll.a)
set (VLC_LIBRARIES ${exec_prefix}/lib/libvlccore.dll.a ${exec_prefix}/lib/libvlc.dll.a )
cmake_minimum_required(VERSION 2.6)

include_directories(${exec_prefix}/include/)

include(FindPkgConfig)


  set(LIB_INSTALL_DIR bin) # .dll are in PATH, like executables
  set(WIN_LIBS -lwsock32 -lwinmm -lpthread)


set (TARGET_NAME ${PROJECT_NAME})

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME}
         ${OPENCV_LIBRARIES}
         ${VLC_LIBRARIES}
         pthread
)

set(_INSTALL_DESTINATIONS
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION lib
)

# Install the library into the lib folder
install(TARGETS ${PROJECT_NAME} ${_INSTALL_DESTINATIONS})

# Install headers into mars include directory
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

# Prepare and install necessary files to support finding of the library 
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

else(WIN32)

rock_library(video_streamer_vlc
    SOURCES VlcStream.cpp
    HEADERS VlcStream.hpp
    DEPS_PKGCONFIG opencv
    LIBS vlc vlccore)

rock_executable(video_streamer_vlc_bin Main.cpp
    DEPS video_streamer_vlc)

endif(WIN32)
