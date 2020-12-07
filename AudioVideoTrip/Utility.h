//
//  Utility.h
//  AudioVideoTrip
//
//  Created by fanglei on 2020/11/30.
//

#ifndef Utility_h
#define Utility_h

#include <stdio.h>
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

AVFormatContext *open_video(void);
AVFormatContext *open_dev(char *devicename, AVDictionary* options);

void stop_record(void);

void record_video(char *output_path);



#endif /* Utility_h */
