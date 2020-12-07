//
//  Utility.c
//  AudioVideoTrip
//
//  Created by fanglei on 2020/11/30.
//

#include "Utility.h"

static bool isRecording = false;

void stop_record(void) {
    isRecording = false;
}

AVFormatContext *open_video(void) {
    /*
     对于 mac
     0: 从摄像头输入
     1：从桌面输入
     */
    AVDictionary *options = NULL;
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "pixel_format", "nv12", 0);
    
    return open_dev("0", options);
}

AVFormatContext *open_dev(char *devicename, AVDictionary* options) {
    AVFormatContext *fmt_ctx = NULL;
    avdevice_register_all();
    
    AVInputFormat *iformat = av_find_input_format("avfoundation");
    
    int ret = 0;
    char errors[1024] = {0};
    if ((ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options)) < 0) {
        av_strerror(ret, errors, 1024);
        fprintf(stderr, "Failed to open device: [%d]%s\n", ret, errors);
        return NULL;
    }
    
    return fmt_ctx;
}

void record_video(char *output_path) {
    isRecording = true;
    
    int ret = 0;
    
    AVFormatContext *fmt_ctx = NULL;
    
    AVPacket pkt;
    
    av_log_set_level(AV_LOG_DEBUG);
    
    char *out = output_path;
    FILE *outfile = fopen(out, "wb+");
    
   fmt_ctx = open_video();
    
    while (isRecording) {
        if ((ret = av_read_frame(fmt_ctx, &pkt)) < 0) {
            if (ret == AVERROR(EAGAIN)) {
                usleep(20);
                continue;
            }
        } else {
            av_log(NULL, AV_LOG_INFO, "packet size is %d(%p)\n", pkt.size, pkt.data);
            
            /*
             uyvu422 => 640x480x2 (u、v 数据是 y 数据的 2 倍)
             nv12 == 420p => 640x480x1.5 (u、v 数据是 y 数据的 1.5 倍)
             */
            
            fwrite(pkt.data, 1, 460800, outfile);
            fflush(outfile);
            av_packet_unref(&pkt); // release pkt
        }
    }
    
__Error:
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }
    
    av_log(NULL, AV_LOG_DEBUG, "finish\n");
    return;;
}
