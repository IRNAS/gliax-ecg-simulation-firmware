#ifndef _ECG_SIMULATOR_H
#define _ECG_SIMULATOR_H

#include "mbed.h"

static const int ECG_MAX_SEND_SIZE = 2160;
//The worst case output size after compression. It is very unlikely that
//this will be ever filled, but it's the theoretical maximum.
static const int ECG_COMPRESS_OUTPUT_BUFFER_SIZE = 25*ECG_MAX_SEND_SIZE/24+1;

static const int maxChannels = 8;
static const int fullBitNum = 19;
static const int smallBitNum = 8;

struct ECGSenderData{
    //ADS1298 data
    float currLsbInMv;
    int selectedChannels;
    //ECGSender data
    float currFrequency;
    uint8_t compressBuffer[ECG_COMPRESS_OUTPUT_BUFFER_SIZE];
    int32_t sampleOfChannels[maxChannels];
    //Test signaling
    bool testSignal;
};

struct ECGHeader{                                                                                                        
    uint8_t channelCount;
    uint32_t sampleCount;
    uint32_t numBits;
    float lsbInMv;
    float samplingFrequency;
} __attribute__((packed));


void ecg_sender_init(void);
void ecg_sender_send(Serial *pc);

#endif /* _ECG_SIMULATOR_H */
