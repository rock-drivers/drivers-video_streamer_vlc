/*
 * VlcStream.cpp
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#include "VlcStream.hpp"


VlcStream::VlcStream(std::string sout, int fps, int width, int height) {

	pthread_mutex_init(&imagemutex,NULL);// = PTHREAD_MUTEX_INITIALIZER;



	sprintf(str_imem_get, "--imem-get=%p", &vlc_imem_get_callback);
	sprintf(str_imem_release, "--imem-release=%p", &vlc_imem_release_callback);
	sprintf(str_imem_data, "--imem-data=%p", this);
	sprintf(str_imem_fps, "%i", fps);
	sprintf(str_imem_width, "%i", width);
	sprintf(str_imem_height, "%i", height);


	if (sout == ""){
		sout = "#transcode{vcodec=MJPG, vb=500}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg}";
	}

	sprintf(smem_options,"%s",sout.c_str());



	const char * const vlc_args[] = {
	"-I","dummy",
	        "--ignore-config",
	        "--network-caching=10",
	        "--http-host=localhost",
	        "--demux","rawvideo",
	        "--rawvid-fps",str_imem_fps,
	        "--rawvid-width",str_imem_width,
	        "--rawvid-height",str_imem_height,
	        "--rawvid-chroma","RV24",

	        "--imem-channels=1",
	        "--imem-cat=4",
	        "--imem-fps",str_imem_fps,
	        "--imem-codec=none",

	        str_imem_get,
	        str_imem_release,
	        str_imem_data,

	        "--sout",smem_options
	};


	vlc = libvlc_new (sizeof (vlc_args) / sizeof (vlc_args[0]), vlc_args);
	vlcm = libvlc_media_new_location(vlc, "imem://");
	vlcmp = libvlc_media_player_new_from_media (vlcm);
	libvlc_media_release (vlcm);

}


VlcStream::~VlcStream() {
	libvlc_media_player_release (vlcmp);
	libvlc_release (vlc);
	//free(buffer);
}

bool VlcStream::write(cv::Mat& image) {
	pthread_mutex_lock(&imagemutex);
	//imagebuf.copyTo(image);

	image.copyTo(imagebuf);

//	if (image.rows != height || image.cols != width || image.type() != CV_8UC3){
//		image = cv::Mat(height,width,CV_8UC3);
//	}
//
//	if (imageAvailable){
//		memcpy(image.data,buffer,buffersize);
//		imageAvailable = false;
//		pthread_mutex_unlock(&imagemutex);
//		return true;
//	}
//
	pthread_mutex_unlock(&imagemutex);
	return false;
}

void VlcStream::start() {
	libvlc_media_player_play (vlcmp);
}

void VlcStream::stop() {
	libvlc_media_player_stop (vlcmp);
}

/**
 * lock and set the pointer to image buffer
 */
int vlc_imem_get_callback(void* data, const char* cookie,
		int64_t* dts, int64_t* pts, unsigned * flags, size_t* size,
		void** output){

	//printf("encode\n");
	VlcStream* parent = (VlcStream*)data;

	//lock
	pthread_mutex_lock(&parent->imagemutex);


	//parent->image = image2.clone();
    *output=parent->imagebuf.data;

    if (pts)
        *pts = 4*(1/30.0)*1000;
    if (dts)
        *dts = 4*(1/30.0)*1000;
    //  *size=(size_t)300;
    *size=(size_t)(parent->imagebuf.rows * parent->imagebuf.cols * 3);


    return 0;

}

/**
 * image buffer full, unlock at end
 */
void vlc_imem_release_callback(void* data, const char* cookie,
		size_t size, void* unknown){

	VlcStream* parent = (VlcStream*)data;
	//unlock
	pthread_mutex_unlock(&parent->imagemutex);
	//printf("encode done\n");

}
