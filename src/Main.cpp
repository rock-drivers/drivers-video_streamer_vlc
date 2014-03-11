#include <iostream>
#include <video_streamer_vlc/VlcStream.hpp>

int main(int argc, char** argv)
{

	cv::VideoCapture cap(0); // open the default camera
	    if(!cap.isOpened())  // check if we succeeded
	        return -1;


	cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);

	cv::Mat image;


    for(;;){
		cap >> image;

		imshow("image", image);


		if(cv::waitKey(30) >= 0) break;
    }

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height =  cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    int fps = 30;//cap.get(CV_CAP_PROP_FPS);

    //std::string sout = "#transcode{vcodec=MJPG, vb=500}:duplicate{dst=display,dst=std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg})";
    std::string sout = "#transcode{vcodec=MJPG, vb=500}:std{access=http{mime=multipart/x-mixed-replace; boundary=--7b3cc56e5f51db803f790dad720ed50a},mux=mpjpeg,dst=:8080/vid.mjpg})";
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
