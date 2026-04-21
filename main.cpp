#include <iostream>
#include "avframequeue.h"
#include "avpacketqueue.h"
#include "demuxthread.h"
#include "decodethread.h"
#include "videooutput.h"
#include "audiooutput.h"
#include "windows.h"
#include "avsync.h"
using namespace std;

#undef main

int main(int argc, char *argv[])
{
    // 先设控制台为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    AVPacketQueue audio_packet_queue;
    AVPacketQueue video_packet_queue;

    AVFrameQueue audio_frame_queue;
    AVFrameQueue video_frame_queue;

    AVSync sync;

    DemuxThread *demux_thread = new DemuxThread(&audio_packet_queue, &video_packet_queue);
    int ret = demux_thread->Init(argv[1]);

    if (ret < 0) {
        cout << "demux_thread->Init failed" << endl;
    }

    ret = demux_thread->Start();

    if (ret < 0) {
        cout << "demux_thread->Start failed" << endl;
    }

    DecodeThread *audio_decode_thread = new DecodeThread(&audio_packet_queue, &audio_frame_queue);

    ret = audio_decode_thread->Init(demux_thread->AudioCodecParameters());
    if (ret < 0) {
        std::cout << "audio_decode_thread->Init(demux_thread->AudioCodecParameters()) failed" << endl;
    }

    ret = audio_decode_thread->Start();
    if (ret < 0) {
        std::cout << "audio_decode_thread->Start() failed" << endl;
    }


    DecodeThread *video_decode_thread = new DecodeThread(&video_packet_queue, &video_frame_queue);

    ret = video_decode_thread->Init(demux_thread->VideoCodecParameters());
    if (ret < 0) {
        std::cout << "audio_decode_thread->Init(demux_thread->AudioCodecParameters()) failed" << endl;
    }

    ret = video_decode_thread->Start();
    if (ret < 0) {
        std::cout << "audio_decode_thread->Start() failed" << endl;
    }

    //初始化audio输出
    AudioParams audio_params = {0};
    memset(&audio_params, 0, sizeof (AudioParams));
    audio_params.channels = demux_thread->AudioCodecParameters()->channels;
    audio_params.channel_layout = demux_thread->AudioCodecParameters()->channel_layout;
    audio_params.fmt = (enum AVSampleFormat)demux_thread->AudioCodecParameters()->format;
    audio_params.freq = demux_thread->AudioCodecParameters()->sample_rate;
    audio_params.frame_size = demux_thread->AudioCodecParameters()->frame_size;

    AudioOutput *audio_output = new AudioOutput(&sync, demux_thread->AudioStreamTimebase(), audio_params, &audio_frame_queue);
    ret = audio_output->Init();
    if (ret < 0) {
        std::cout << ("audio_output->Init() failed") << std::endl;
        return -1;
    }

    VideoOutPut *video_output = new VideoOutPut(&sync, demux_thread->VideoStreamTimebase(), &video_frame_queue, demux_thread->VideoCodecParameters()->width, demux_thread->VideoCodecParameters()->height);
    ret = video_output->Init();
    if (ret < 0) {
        std::cout << "video_output->Init() failed" << endl;
    }

    ret = video_output->MainLoop();
    if (ret < 0) {
        std::cout << "video_output->MainLoop() failed" << endl;
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    demux_thread->Stop();
    delete demux_thread;

    return 0;
}
