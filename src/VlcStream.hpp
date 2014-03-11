/*
 * VlcStream.h
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#ifndef VLCCAPTURE_H_
#define VLCCAPTURE_H_


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
	VlcStream(std::string sout, int fps, int width, int height);
	virtual ~VlcStream();


	//void open();

	//bool grab();

	//bool retrieve();

	bool write(cv::Mat &image);

	void start();

	void stop();


	pthread_mutex_t imagemutex;

//	int width,height;
//
	cv::Mat imagebuf;
//	uint8_t* buffer;
//	unsigned int buffersize;
//
//	bool imageAvailable;

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
