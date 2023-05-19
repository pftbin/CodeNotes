// VideoKeyFrame.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#pragma comment (lib, "avcodec.lib")
#pragma comment (lib, "avdevice.lib")
#pragma comment (lib, "avfilter.lib")
#pragma comment (lib, "avformat.lib")
#pragma comment (lib, "avutil.lib")
#pragma comment (lib, "postproc.lib")
#pragma comment (lib, "swresample.lib")
#pragma comment (lib, "swscale.lib")

using namespace std;
#pragma warning(disable:4996)

bool save_frame(AVFrame* pFrame, char* out_name)
{
	int width = pFrame->width;
	int height = pFrame->height;
	AVCodecContext* pCodeCtx = NULL;
	AVFormatContext* pFormatCtx = avformat_alloc_context();
	// 设置输出文件格式
	pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
	// 创建并初始化输出AVIOContext
	if (avio_open(&pFormatCtx->pb, out_name, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Couldn't open output file.");
		return false;
	}
	// 构建一个新stream
	AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
	if (pAVStream == NULL) {
		return false;
	}
	AVCodecParameters* parameters = pAVStream->codecpar;
	parameters->codec_id = pFormatCtx->oformat->video_codec;
	parameters->codec_type = AVMEDIA_TYPE_VIDEO;
	parameters->format = AV_PIX_FMT_YUVJ420P;
	parameters->width = pFrame->width;
	parameters->height = pFrame->height;
	AVCodec* pCodec = (AVCodec*)avcodec_find_encoder(pAVStream->codecpar->codec_id);
	if (!pCodec) {
		printf("Could not find encoder\n");
		return false;
	}
	pCodeCtx = avcodec_alloc_context3(pCodec);
	if (!pCodeCtx) {
		fprintf(stderr, "Could not allocate video codec context\n");
		exit(1);
	}
	if ((avcodec_parameters_to_context(pCodeCtx, pAVStream->codecpar)) < 0) {
		fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
			av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
		return false;
	}

	pCodeCtx->time_base.den = 1;
	pCodeCtx->time_base.num = 25;
	if (avcodec_open2(pCodeCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.");
		return false;
	}
	int ret = avformat_write_header(pFormatCtx, NULL);
	if (ret < 0) {
		printf("write_header fail\n");
		return false;
	}
	int y_size = width * height;
	//Encode
	// 给AVPacket分配足够大的空间
	AVPacket pkt;
	av_new_packet(&pkt, y_size * 3);
	// 编码数据
	ret = avcodec_send_frame(pCodeCtx, pFrame);
	if (ret < 0) {
		printf("Could not avcodec_send_frame.");
		return false;
	}
	// 得到编码后数据
	ret = avcodec_receive_packet(pCodeCtx, &pkt);
	if (ret < 0) {
		printf("Could not avcodec_receive_packet");
		return false;
	}
	ret = av_write_frame(pFormatCtx, &pkt);
	if (ret < 0) {
		printf("Could not av_write_frame");
		return false;
	}
	av_packet_unref(&pkt);
	//Write Trailer
	av_write_trailer(pFormatCtx);
	avcodec_close(pCodeCtx);
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);
	return true;
}

bool extract_frames(string video_path, string output_dir, int frame_num = 0)
{
	AVFormatContext* format_ctx = NULL;
	int ret = avformat_open_input(&format_ctx, video_path.c_str(), NULL, NULL);
	if (ret < 0) {
		cerr << "Could not open input file: " << video_path << endl;
		return false;
	}

	ret = avformat_find_stream_info(format_ctx, NULL);
	if (ret < 0) {
		cerr << "Could not find stream information" << endl;
		return false;
	}

	AVCodec* codec = NULL;
	int stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, (const AVCodec**)&codec, 0);
	if (stream_index < 0) {
		cerr << "Could not find video stream in the input file" << endl;
		return false;
	}

	AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		cerr << "Could not allocate codec context" << endl;
		return false;
	}

	ret = avcodec_parameters_to_context(codec_ctx, format_ctx->streams[stream_index]->codecpar);
	if (ret < 0) {
		cerr << "Could not copy codec parameters to codec context" << endl;
		return false;
	}

	ret = avcodec_open2(codec_ctx, codec, NULL);
	if (ret < 0) {
		cerr << "Could not open codec" << endl;
		return false;
	}

	AVFrame* frame = av_frame_alloc();
	if (!frame) {
		cerr << "Could not allocate frame" << endl;
		return false;
	}

	AVPacket packet;
	av_init_packet(&packet);

	int frame_index = 0;
	while (av_read_frame(format_ctx, &packet) >= 0)
	{
		if (packet.stream_index == stream_index)
		{
			ret = avcodec_send_packet(codec_ctx, &packet);
			if (ret < 0) {
				cerr << "Error sending packet to decoder" << endl;
				return false;
			}

			while (ret >= 0)
			{
				ret = avcodec_receive_frame(codec_ctx, frame);
				if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				{
					break;
				}
				else if (ret < 0)
				{
					cerr << "Error during decoding" << endl;
					return false;
				}
				

				if (frame_index != frame_num)
					continue;

				char filename[256] = { 0 };
				sprintf_s(filename, "%s/way1_%04d.jpg", output_dir.c_str(), frame_index);
				save_frame(frame, filename); //保存为图片
				frame_index++;
			}
		}

		av_packet_unref(&packet);
	}

	av_frame_free(&frame);
	avcodec_close(codec_ctx);
	avcodec_free_context(&codec_ctx);
	avformat_close_input(&format_ctx);

	return true;
}

int main(int argc, char** argv)
{
	string video_path = "D:/KeyFrame/video1.mp4";
	string output_dir = "D:/KeyFrame";

	int ret = extract_frames(video_path, output_dir);
	if (ret < 0) {
		cerr << "Error extracting frames" << endl;
		return false;
	}

	system("pause");
	return 0;
}

