/*
 * VlcStream.h
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#ifndef VLCSTREAM_H_
#define VLCSTREAM_H_


//https://wiki.videolan.org/Stream_to_memory_%28smem%29_tutorial/

#include <stdio.h>
#include <stdint.h>
#include <string>

#include <vlc/vlc.h>

#include <pthread.h>
#include <opencv2/opencv.hpp>


/**
 * lock and set the pointer to image buffer
 */
int vlc_imem_get_callback(void* data, const char* cookie,
		int64_t* dts, int64_t* pts, unsigned * flags, size_t* size,
		void** output);

/**
 * image buffer full, unlock at end
 */
void vlc_imem_release_callback(void* data, const char* cookie,
		size_t size, void* unknown);


class VlcStream {
public:

	/**
	 * Constructor of the streamer class
	 * @param sout sout string as defined for vlc, see following links for documentation:
	 * https://wiki.videolan.org/Transcode/
	 * http://www.videolan.org/doc/vlc-user-guide/en/ch04.html
	 * http://www.videolan.org/doc/videolan-howto/en/ch09.html
	 * http://www.videolan.org/doc/streaming-howto/en/ch04.html
	 * @param fps frames per second of the input from opencv
	 * @param width width of the input in pixels
	 * @param height height of the input in pixels
	 */

	VlcStream(std::string sout, int fps, int width, int height);
	virtual ~VlcStream();

	/**
	 * write the next image to the stream
	 */
	bool write(cv::Mat &image);

	/**
	 * start streaming thread (vlc instance "play")
	 * automatically called by the constructor
	 */
	void start();

	/**
	 * stop streaming  (vlc instance "stop")
	 */
	void stop();


	pthread_mutex_t imagemutex;

	cv::Mat imagebuf;
	cv::Mat pixbuf;

private:

    //options
    char smem_options[256];

    //callback pointer addess string
    char str_imem_get[100], str_imem_release[100],str_imem_data[100],str_imem_fps[100],str_imem_width[100],str_imem_height[100];


    libvlc_instance_t *vlc;
    libvlc_media_player_t *vlcmp;
    libvlc_media_t *vlcm;

};

#endif /* VLCTOMEMORY_H_ */
