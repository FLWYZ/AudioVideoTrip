////
////  AudioRecord.c
////  AudioVideoTrip
////
////  Created by fanglei on 2020/11/25.
////
//
//#include "AudioRecord.h"
//#include "libavdevice/avdevice.h"
//#include "libavformat/avformat.h"
//#include "libavcodec/avcodec.h"
//#include "libswresample/swresample.h"
//#include <unistd.h>
//#include <stdbool.h>
//#include <string.h>
//
//void _log(const char *fmt) {
//    av_log(NULL, AV_LOG_DEBUG, "%s", fmt);
//}
//
//static bool recordable = false;
//
//void stop_record(void) {
//    recordable = false;
//}
//
//AVFormatContext* open_dev() {
//    int ret = 0;
//    char errors[1024] = {0};
//    
//    AVFormatContext *fmt_ctx = NULL;
//    AVDictionary *options = NULL;
//    
//    // [video device]:[audio device]
//    char *devicename = ":0";
//    
//    AVInputFormat *iformat = av_find_input_format("avfoundation");
//    
//    if ((ret = avformat_open_input(&fmt_ctx, devicename, iformat, &options)) < 0) {
//        av_strerror(ret, errors, 1024);
//        printf("Failed to open audio device, [%d]%s\n", ret, errors);
//        return NULL;
//    }
//    return fmt_ctx;
//}
//
//AVCodecContext * open_codec() {
//    // 打开编码器
//    AVCodec *codec = avcodec_find_encoder_by_name("libfdk_aac");
//    
//    // 创建解码上下文
//    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
//    codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;          // 音频采样大小
//    codec_ctx->channel_layout = AV_CH_LAYOUT_STEREO;    // 音频声道数
//    codec_ctx->sample_rate = 44100;                     // 采样频率
//    // 只有将 bit_rate 设为 0，ffmpeg 才会从 profile 中查找码流数据
//    codec_ctx->bit_rate = 0; // 64k
//    codec_ctx->profile = FF_PROFILE_AAC_HE_V2;
//    
//    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
//        printf("open codec failed");
//        return NULL;
//    }
//    return codec_ctx;
//}
//
//void record_audio_convert_to_aac(void) {
//    avdevice_register_all();
//    
//    recordable = true;
//    
//    char* out = "/Users/fanglei/Desktop/audio_out/audio.aac";
//    FILE *out_file = fopen(out, "wb+");
//    
//    AVFormatContext *fmt_ctx = open_dev();
//    AVCodecContext *codec_ctx = open_codec();
//    
//}
