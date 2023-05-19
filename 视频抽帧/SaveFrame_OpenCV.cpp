#pragma once
#include "public.h"
#include <string>

#include <opencv2/opencv.hpp>
#include <iostream>

#pragma comment(lib, "opencv_world460.lib")

using namespace cv;
using namespace std;

bool getimage_fromvideo(string video_path, string output_dir, int frame_num = 0)
{
    if (video_path.empty() || output_dir.empty())
    {
        printf("[getimage_fromvideo] input videopath/imagepath is empty...\n");
        return false;
    }

    // 加载视频文件
    VideoCapture cap(video_path);
    if (!cap.isOpened())
    {
        printf("[getimage_fromvideo]Error opening video file...\n");
        return false;
    }

    // 获取帧数和帧率
    int num_frames = cap.get(CAP_PROP_FRAME_COUNT);
    double fps = cap.get(CAP_PROP_FPS);

    // 设置帧数
    cap.set(CAP_PROP_POS_FRAMES, frame_num);

    // 读取该帧的图像数据
    Mat frame;
    bool success = cap.read(frame);
    if (!success)
    {
        printf("[getimage_fromvideo]Error reading frame...\n");
        return false;
    }

    //指定图片宽高
    int width = 480, height = 270;
    resize(frame, frame, Size(width, height));

    //保存为图片文件
    char filename[256] = { 0 };
    sprintf_s(filename, "%s/way1_%04d.jpg", output_dir.c_str(), frame_num);
    imwrite(filename, frame);

    return true;
}

int main(int argc, char** argv)
{
    string videofilepath = "D:/KeyFrame/video1.mp4";
    string imagefilepath = "D:/KeyFrame/video_frame.jpg";

    bool ret = getimage_fromvideo(videofilepath, imagefilepath);
    if (ret) {
        cerr << "Error extracting frames" << endl;
        return false;
    }

    system("pause");
    return 0;
}
