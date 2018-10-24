#include <iostream>
#include <stdio.h>
#include <time.h>
#include <common_opencv.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "dual_mode_deblock.h"

using namespace cv;

Mat Image_deblock(Mat src)
{
	//resize(src, src, Size(1080, 720)); //640,480   256, 256  1080,720  1920,1080  320, 240

	int width = src.cols;
	int height = src.rows;

	//imshow("original", src);
	src.convertTo(src, CV_32F);
	Mat channel_pepper[3];
	Mat channel_pepper_out[3];

	double cost, start, end;
	start = clock();
	 
	split(src, channel_pepper);

	Dual_Mode_Deblock_Learner *p_0 = new Dual_Mode_Deblock_Learner(channel_pepper[0], height, width);
	Dual_Mode_Deblock_Learner *p_1 = new Dual_Mode_Deblock_Learner(channel_pepper[1], height, width);
	Dual_Mode_Deblock_Learner *p_2 = new Dual_Mode_Deblock_Learner(channel_pepper[2], height, width);

	channel_pepper_out[0] = p_0->Process_and_Output();//13ms	
	channel_pepper_out[1] = p_1->Process_and_Output();
	//channel_pepper_out[2] = p_2->Process_and_Output();


	Mat result;
	merge(channel_pepper_out, 3, result);//1ms
	result.convertTo(result, CV_8U);

	end = clock();
	cost = difftime(end, start);
	printf("%f\n", cost);
	//imshow("result", result);
	//waitKey(1000);
	delete(p_0);
	delete(p_1);
	delete(p_2);

	return result;
}

int main()
{
	//string path_in = "test1.jpg";
	//Mat pepper = imread(path_in);

	//Mat result = Image_deblock(pepper);
	//imshow("test", result);
	//waitKey(0);


	string  video_path = "./test_video/bitstream_new.ts"; //1Mbps_10fps_320240_02.avi   //
	/*./test_video/bitstream_new.ts 
	./test_video/bitstream.ts
	01.avi 20180723_640480_10fps_01.avi//
	1970_01_01_00_02_01_0060.avi//
	1970_01_01_00_01_01_0060//
	640_480_10fps_03//
	640_480_10fps_02//
	640_480_10fps_01//
	1Mbps_10fps_320240_02//
	1Mbps_10fps_320240_01//
	*/
	VideoCapture cap(video_path);
	int num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	printf("%d\n", num_frames);
	if (!cap.isOpened())
	{
		printf("open failed!\n");
		return -1;
	}
	//VideoWriter writer("videoresult.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(640, 480), true);
	bool status = true;  // 判断循环是否中止的布尔量
	int n = 0;

	//FILE *fp = fopen("video.txt", "wb");
	//if (fp == NULL)
	//{
	//	return -1;
	//}

	while (1)
	{
		Mat frame;
		cap.read(frame);
		if (frame.empty())
			break;
		imshow("video", frame);
		//int i = frame.channels();
		//int bufLen = frame.cols * frame.rows * 3;
		//unsigned char *pYuvBuf = new unsigned char[bufLen];
		Mat result;
		//cvtColor(frame, result, CV_BGR2YUV_I420);

		//memcpy(pYuvBuf, frame.data, bufLen * sizeof(unsigned char));

		//fwrite(pYuvBuf, sizeof(unsigned char)*bufLen, 1, fp);
		double start, end, cost;
		start = clock();
		result = Image_deblock(frame);
		end = clock();
		cost = difftime(end, start);
		printf("%f\n", cost);
		imshow("dst", result);
		//writer.write(result);
		printf("%d\n", n);
		n++;
		waitKey(30);
		if (waitKey(27) >= 0)
		{
			imwrite("test.bmp", frame);
		}
		//delete(pYuvBuf);

	}
	cap.release();
	//fclose(fp);
	//writer.release();
	return 0;
}
