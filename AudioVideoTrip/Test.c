////
////  Test.c
////  AudioVideoTrip
////
////  Created by fanglei on 2020/11/19.
////
//
//#include "Test.h"
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
//AVFormatContext* openInput() {
//    avdevice_register_all();
//
//    AVInputFormat *iformat = av_find_input_format("avfoundation");
//    AVDictionary *options = NULL;
//    AVFormatContext *fmt_ctx = NULL;
//    // 查看 ffmpeg 的官网
//    char *deviceName = ":0";
//    int ret = 0;
//    char errors[1024];
//
//    av_log_set_level(AV_LOG_DEBUG);
//    if ((ret = avformat_open_input(&fmt_ctx, deviceName, iformat, &options)) < 0) { // 出错
//        av_strerror(ret, errors, 1024);
//        printf("Failed to open audio device, [%d]%s\n", ret, errors);
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
//AVFrame *create_av_frame() {
//    AVFrame *frame = av_frame_alloc();
//    frame->nb_samples      = 512; // 单通道每秒采样数
//    frame->format          = AV_SAMPLE_FMT_S16; // 采样大小
//    frame->channel_layout  = AV_CH_LAYOUT_STEREO; // 声道布局（声道数）
//    av_frame_get_buffer(frame, 0);  // buffer == 512 * 2 => 2048
//    if (!frame || !frame->buf[0]) {
//        printf("alloc av_frame failed");
//        return NULL;
//    }
//    return frame;
//}
//
//void encode(AVCodecContext *codec_ctx,
//            AVFrame *frame,
//            AVPacket *pkt,
//            FILE *outfile) {
//    int ret = 0;
//    
//    // 将一帧数据，输送给编码器
//    // 函数内部会将这一帧数据，放到编码器内部的 buffer 中
//    // 输送一帧数据，并不会立刻返回一帧 packet，而是当编码器的 buffer 装满时才会进行编码
//    ret = avcodec_send_frame(codec_ctx, frame); // ret >= 0，说明 frame 输送成功
//    while (ret >= 0) {
//        /*
//         此处 ret >= 0，说明成功取回一个编码好的音频数据包；
//         同时，可能还有更多的音频包，因而通过 while 循环继续尝试获取
//         */
//        ret = avcodec_receive_packet(codec_ctx, pkt);
//        if (ret < 0) {
//            // ret < 0，的原因是没有可编码的数据了，需要读取新的音频 frame
//            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                return;
//            }
//            printf("Error, encoding audio frame failed.\n");
//            return;
//        }
//        fwrite(pkt->data, 1, pkt->size, outfile);
//        fflush(outfile);
//    }
//}
//
//// create file
//FILE* openFile(char *filepath) {
//    return fopen(filepath, "wb+");
//}
//
//void record_audio(void) {
//    recordable = true;
//    AVFormatContext* ctx = openInput();
//    
//    char* out = "/Users/fanglei/Desktop/audio_out/audio.pcm";
//    FILE* outfile = openFile(out);
//    
//    AVPacket pkt;
//    av_init_packet(&pkt);
//    int ret = 0;
//    char errors[1024];
//    
//    while (recordable) {
//        if ((ret = av_read_frame(ctx, &pkt)) < 0) {
//            if (ret == -35) {
//                usleep(20);
//                continue;
//            }
//            av_strerror(ret, errors, 1024);
//            printf("record error, [%d]%s\n", ret, errors);
//            break;
//        } else {
//            printf("recording pkt size == %d\n", pkt.size);
//            fwrite(pkt.data, pkt.size, 1, outfile);
//            fflush(outfile);
//        }
//        av_packet_unref(&pkt);
//    }
//    
//    // close file
//    fclose(outfile);
//    
//    // release ctx
//    avformat_close_input(&ctx);
//    
//    _log("finish");
//}
//
//void stop_record(void) {
//    recordable = false;
//}
//
///* ------------------------- 音频重采样 ------------------------- */
//void record_audio_v2(void) {
//    recordable = true;
//    AVFormatContext* ctx = openInput();
//    
//    char* out = "/Users/fanglei/Desktop/audio_out/audio.pcm";
//    FILE* outfile = openFile(out);
//    
//    AVPacket pkt;
//    av_init_packet(&pkt);
//    int ret = 0;
//    char errors[1024];
//    
//    SwrContext *swr_ctx = NULL;
//    /*
//     channel layout == channel 声道数
//     */
//    swr_ctx = swr_alloc_set_opts(NULL,                // 从采样上下文
//                                 AV_CH_LAYOUT_STEREO, // 输出 channel 布局
//                                 AV_SAMPLE_FMT_S16,   // 输出采样格式
//                                 44100,               // 输出采样率
//                                 AV_CH_FRONT_CENTER,  // 输入 channel 布局
//                                 AV_SAMPLE_FMT_FLT,   // 输入采样格式
//                                 48000,               // 输入采样率
//                                 0,                   // log 信息
//                                 NULL);               // log 信息
//    
//    if (!swr_ctx) { // 创建重采样上下文失败
//        
//    }
//    if (swr_init(swr_ctx) < 0) {
//        
//    }
//    
//    // 创建输入缓冲区
//    uint8_t **src_data = NULL;
//    int src_linesize = 0;
//    av_samples_alloc_array_and_samples(
//                                       &src_data,           // 输入缓冲区地址
//                                       &src_linesize,       // 缓冲区的大小
//                                       1,                   // 声道个数
//                                       512,                 // 单通道采样个数
//                                       AV_SAMPLE_FMT_FLT,   // 采样格式
//                                       0);                  // 对齐方式
//    
//    // 创建输出缓冲区
//    uint8_t **dst_data = NULL;
//    int dst_linesize = 0;
//    av_samples_alloc_array_and_samples(
//                                       &dst_data,           // 输出缓冲区地址
//                                       &dst_linesize,       // 缓冲区的大小
//                                       2,                   // 声道个数
//                                       512,                 // 单通道采样个数
//                                       AV_SAMPLE_FMT_S16,   // 采样格式
//                                       0);                  // 对齐方式
//    
//    while (recordable) {
//        if ((ret = av_read_frame(ctx, &pkt)) < 0) {
//            if (ret == -35) {
//                usleep(20);
//                continue;
//            }
//            av_strerror(ret, errors, 1024);
//            printf("record error, [%d]%s\n", ret, errors);
//            break;
//        } else {
//            printf("recording pkt size == %d\n", pkt.size);
//            
//            // 将 pkt 中的 data 拷贝到输入缓冲区，准备进行重采样
//            memcpy(src_data[0], pkt.data, pkt.size);
//            
//            swr_convert(swr_ctx,                    // 重采样上下文
//                        dst_data,                   // 输出缓冲区
//                        512,                        // 输出单通道的采样数
//                        (const uint8_t **)src_data, // 输入缓冲区
//                        512);                       // 输入单通道的采样数
//            
//            fwrite(dst_data[0], dst_linesize, 1, outfile);
//            fflush(outfile);
//        }
//        av_packet_unref(&pkt);
//    }
//    
//    // close file
//    fclose(outfile);
//    
//    // 释放输入输出缓冲区
//    if (src_data) {
//        av_freep(&src_data[0]);
//    }
//    av_freep(&src_data);
//    
//    if (dst_data) {
//        av_freep(&dst_data[0]);
//    }
//    av_freep(&dst_data);
//    
//    swr_free(&swr_ctx);
//    
//    // release ctx
//    avformat_close_input(&ctx);
//    
//    _log("finish");
//}
//
///* ------------------------- AAC 编码器 ------------------------- */
//void record_audio_aac(void) {
//    recordable = true;
//    AVFormatContext* ctx = openInput();
//    
//    char* out = "/Users/fanglei/Desktop/audio_out/audio.aac";
//    FILE* outfile = openFile(out);
//    
//    AVCodecContext *codec_ctx = open_codec();
//    // 输入 frame
//    AVFrame *frame = create_av_frame();
//    
//    // 输出 packet
//    AVPacket *newpkt = av_packet_alloc();
//    if (!newpkt) {
//        printf("alloc new packet failed");
//        return;
//    }
//    
//    
//    AVPacket pkt;
//    av_init_packet(&pkt);
//    int ret = 0;
//    char errors[1024];
//    
//    SwrContext *swr_ctx = NULL;
//    /*
//     channel layout == channel 声道数
//     */
//    swr_ctx = swr_alloc_set_opts(NULL,                // 从采样上下文
//                                 AV_CH_LAYOUT_STEREO, // 输出 channel 布局
//                                 AV_SAMPLE_FMT_S16,   // 输出采样格式
//                                 44100,               // 输出采样率
//                                 1,                   // 输入 channel 布局
//                                 AV_SAMPLE_FMT_FLT,   // 输入采样格式
//                                 48000,               // 输入采样率
//                                 0,                   // log 信息
//                                 NULL);               // log 信息
//    
//    if (!swr_ctx) { // 创建重采样上下文失败
//        
//    }
//    if (swr_init(swr_ctx) < 0) {
//        
//    }
//    
//    // 创建输入缓冲区
//    // 512 = 2048/4(32位 == 4字节)
//    uint8_t **src_data = NULL;
//    int src_linesize = 0;
//    av_samples_alloc_array_and_samples(
//                                       &src_data,           // 输入缓冲区地址
//                                       &src_linesize,       // 缓冲区的大小
//                                       1,                   // 声道个数
//                                       512,                 // 单通道采样个数
//                                       AV_SAMPLE_FMT_FLT,   // 采样格式
//                                       0);                  // 对齐方式
//    
//    // 创建输出缓冲区
//    uint8_t **dst_data = NULL;
//    int dst_linesize = 0;
//    av_samples_alloc_array_and_samples(
//                                       &dst_data,           // 输出缓冲区地址
//                                       &dst_linesize,       // 缓冲区的大小
//                                       2,                   // 声道个数
//                                       512,                 // 单通道采样个数
//                                       AV_SAMPLE_FMT_S16,   // 采样格式
//                                       0);                  // 对齐方式
//    
//    while (recordable) {
//        if ((ret = av_read_frame(ctx, &pkt)) < 0) {
//            if (ret == -35) {
//                usleep(20);
//                continue;
//            }
//            av_strerror(ret, errors, 1024);
//            printf("record error, [%d]%s\n", ret, errors);
//            break;
//        } else {
//            printf("recording pkt size == %d\n", pkt.size);
//            
//            // 将 pkt 中的 data 拷贝到输入缓冲区，准备进行重采样
//            memcpy(src_data[0], pkt.data, pkt.size);
//            
//            swr_convert(swr_ctx,                    // 重采样上下文
//                        dst_data,                   // 输出缓冲区
//                        512,                        // 输出单通道的采样数
//                        (const uint8_t **)src_data, // 输入缓冲区
//                        512);                       // 输入单通道的采样数
//            
//            // 将重采样的数据，拷贝到 frame 中
//            memcpy((void *)frame->data[0], dst_data[0], dst_linesize);
//            
//            encode(codec_ctx, frame, newpkt, outfile);
//        }
//        av_packet_unref(&pkt);
//    }
//    
//    // 强制将编码器缓冲区中的数据进行编码输出
//    encode(codec_ctx, NULL, newpkt, outfile);
//    
//    // close file
//    fclose(outfile);
//    
//    av_frame_free(&frame);
//    
//    av_packet_free(&newpkt);
//    
//    // 释放输入输出缓冲区
//    if (src_data) {
//        av_freep(&src_data[0]);
//    }
//    av_freep(&src_data);
//    
//    if (dst_data) {
//        av_freep(&dst_data[0]);
//    }
//    av_freep(&dst_data);
//    
//    swr_free(&swr_ctx);
//    
//    // release ctx
//    avformat_close_input(&ctx);
//    
//    _log("finish");
//}
