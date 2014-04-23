/*
 * VlcStream.cpp
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#include "VlcStream.hpp"


VlcStream::VlcStream(std::string sout, int fps, int width, int height) {

	pthread_mutex_init(&imagemutex,NULL);// = PTHREAD_MUTEX_INITIALIZER;

	sprintf(str_imem_get, "--imem-get=%lld", (long long int)vlc_imem_get_callback);
	sprintf(str_imem_release, "--imem-release=%lld", (long long int)vlc_imem_release_callback);
	sprintf(str_imem_data, "--imem-data=%lld", (long long int)this);
	sprintf(str_imem_fps, "%i", fps);
	sprintf(str_imem_width, "%i", width);
	sprintf(str_imem_height, "%i", height);


	if (sout == ""){
		//sout = "#transcode{vcodec=MJPG, vb=500}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg}";
		sout = "#transcode{vcodec=h264, vb=4096}:std{access=file, mux=mp4, dst=video.mp4}";
	}

	sprintf(smem_options,"%s",sout.c_str());



	const char * const vlc_args[] = {
	"-I","dummy",
	        "--ignore-config",
	        "--network-caching=100",
          "--live-caching=100",
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
	start();
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
	parent->imagebuf.copyTo(parent->pixbuf);

    *output=parent->pixbuf.data;

    if (pts)
        *pts = 1;//4*(1/30.0)*1000;
    if (dts)
        *dts = 1;//4*(1/30.0)*1000;
    //  *size=(size_t)300;
    *size=(size_t)(parent->pixbuf.rows * parent->pixbuf.cols * 3);

    pthread_mutex_unlock(&parent->imagemutex);
    return 0;

}

/**
 * image buffer full, unlock at end
 */
void vlc_imem_release_callback(void* data, const char* cookie,
		size_t size, void* unknown){

	VlcStream* parent = (VlcStream*)data;
	//unlock

	//printf("encode done\n");

}
