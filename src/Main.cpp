#include <iostream>
#include <video_streamer_vlc/VlcStream.hpp>


/**
 * Open a FullHD webcam and streams as motion jpg to http://localhost:8080/vid.mjpg (can be opened by browser)
 */

int main(int argc, char** argv)
{

	cv::VideoCapture cap(0); // open the default camera
	    if(!cap.isOpened())  // check if we succeeded
	        return -1;


	cap.set(cv::CAP_PROP_FRAME_WIDTH,1920);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT,1080);

	cv::Mat image;


    for(;;){
		cap >> image;

		imshow("image", image);


		if(cv::waitKey(30) >= 0) break;
    }

    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height =  cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = 30;//cap.get(CV_CAP_PROP_FPS);

    //provide as mjpg stream and display locally
    //std::string sout = "#transcode{vcodec=MJPG, vb=500}:duplicate{dst=display,dst=std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg})";

    //provide as mjpg stream
    std::string sout = "#transcode{vcodec=MJPG, vb=500}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg})";

    //save to file using a mp4 container and the h264 codec with a bitrate og 4096 kbps
    //std::string sout = "#transcode{vcodec=h264, vb=4096}:std{access=file, mux=mp4, dst=video.mp4}";


    VlcStream streamer (sout,fps,width,height);

    streamer.write(image);
    streamer.start();

    for(;;){
		cap >> image;

		//imshow("image", image);
		streamer.write(image);

		if(cv::waitKey(30) >= 0) break;
    }



	return 0;
}
