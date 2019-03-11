/*
 * VlcStream.cpp
 *
 *  Created on: 03.03.2014
 *      Author: planthaber
 */

#include "VlcStream.hpp"
#include <vlc/libvlc_renderer_discoverer.h>


VlcStream::VlcStream(std::string sout, int fps, int width, int height) {

	pthread_mutex_init(&imagemutex,NULL);// = PTHREAD_MUTEX_INITIALIZER;

    char str_imem_get[100], str_imem_release[100],str_imem_data[100],
		str_imem_fps[100],str_imem_width[100],str_imem_height[100];
    char str_rawvideo_fps[100],str_rawvideo_width[100],str_rawvideo_height[100];

	sprintf(str_imem_get,     "imem-get=%lld", (long long int)vlc_imem_get_callback);
	sprintf(str_imem_release, "imem-release=%lld", (long long int)vlc_imem_release_callback);
	sprintf(str_imem_data,    "imem-data=%lld", (long long int)this);
	sprintf(str_imem_fps,     "imem-fps=%i", fps);
	sprintf(str_imem_width,   "imem-width=%i", width);
	sprintf(str_imem_height,  "imem-height=%i", height);
	sprintf(str_rawvideo_fps,     "rawvid-fps=%i", fps);
	sprintf(str_rawvideo_width,   "rawvid-width=%i", width);
	sprintf(str_rawvideo_height,  "rawvid-height=%i", height);

	if (sout == ""){
		sout = "#transcode{vcodec=h264, vb=4096}:std{access=file, mux=mp4, dst=video.mp4}";
	}

	const char* params[] = {
		"imem-channels=1",
		"imem-cat=4",
		str_imem_fps,
		"imem-codec=RV24",
		str_imem_width,
		str_imem_height,
		str_imem_get,
		str_imem_release,
		str_imem_data,
		"demux=rawvideo",
		str_rawvideo_fps,
		str_rawvideo_width,
		str_rawvideo_height,
		"rawvid-chroma=RV24"
	};

        // Load the VLC engine */
	char* args[] = { "--verbose=3" };
	vlc = libvlc_new (1, args);
	int ret = libvlc_vlm_add_broadcast(vlc, "netbroadcast",
		"imem://",
		sout.c_str(),
		sizeof(params) / sizeof(*params), params,
		1, 0);
	if (ret < 0)
		throw std::runtime_error("Failed to created broadcast");

	ret = libvlc_vlm_play_media(vlc, "netbroadcast");
	if (ret < 0)
		throw std::runtime_error("Failed to play media");
	vlcmp = libvlc_media_player_new(vlc);
}


VlcStream::~VlcStream() {
	libvlc_media_player_release (vlcmp);
	libvlc_release (vlc);
}

bool VlcStream::write(cv::Mat& image, int64_t ts) {
	pthread_mutex_lock(&imagemutex);

	image.copyTo(imagebuf);
	timestamp = ts; 

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

    VlcStream* parent = (VlcStream*)data;
    //lock
    pthread_mutex_lock(&parent->imagemutex);

    parent->imagebuf.copyTo(parent->pixbuf);

    *output=parent->pixbuf.data;

    if (pts)
		*pts = parent->timestamp;
    if (dts)
		*dts = parent->timestamp;
	*size=(size_t)(parent->pixbuf.rows * parent->pixbuf.cols * 3);

    return 0;
}

/**
 * image buffer full, unlock at end
 */
void vlc_imem_release_callback(void* data, const char* cookie,
		size_t size, void* unknown){

	//unlock
	VlcStream* parent = (VlcStream*)data;
	pthread_mutex_unlock(&parent->imagemutex);


}
