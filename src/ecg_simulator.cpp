#include "mbed.h"
#include "ecg_simulator.h"
#include "TestSignalGenerator.h"
#include "FlatEcgPredictor.hpp"
#include "BitFifo.hpp"
#include "Packetizer.h"
#include "DifferenceEcgCompressor.hpp"

struct ECGSenderData ecgSenderData;
const int ECG_CHOP_BITS = 5;
const float ECG_LSB_IN_MV = 0.0001430511475f;

const unsigned int nHardwareChannels[3] = {4, 6, 8};
const int gain[7] = {1, 2, 3, 4, 6, 8, 12};

static const int32_t smallMin = (1 << (smallBitNum - 1)) - (1 << smallBitNum);
static const int32_t smallMax = -smallMin - 1;
static const int32_t fullMin = (1 << (fullBitNum - 1)) - (1 << fullBitNum);
static const int32_t fullMax = -fullMin - 1;

TestSignalGenerator testGenerator(15000, 500);

//Create required data
ecg::BitFifo compressFifo((char *)ecgSenderData.compressBuffer, ECG_COMPRESS_OUTPUT_BUFFER_SIZE);
ecg::FlatEcgPredictor ecgPredictor;
ecg::DifferenceEcgCompressor compressor(compressFifo, ecgPredictor);
Packetizer packetizer;

#if 0
static bool put_sample(const int32_t* channels) 
{
	for (unsigned int i = 0; i < nHardwareChannels[0]; ++i) 
    {
		int32_t diff = channels[i] - ecgPredictor.getPrediction(i);
		bool okay = true;
		if (diff >= smallMin && diff <= smallMax) 
        {
			okay &= compressFifo.pushBits(0, 1);
			okay &= compressFifo.pushBits(diff, smallBitNum);
		} 
        else 
        {
			okay &= compressFifo.pushBits(1, 1);
			okay &= compressFifo.pushBits(channels[i], fullBitNum);
		}
		
        if(!okay)
        {
			return false;
        }
	}
	ecgPredictor.putSample(channels);
	
    return true;
}
#endif

void ecg_sender_init(void)
{
    ecgSenderData.currLsbInMv = ECG_LSB_IN_MV / 6.0;
    ecgSenderData.currFrequency = 488.28125f;
    ecgSenderData.selectedChannels = nHardwareChannels[0];
    //Enable test generator
    ecgSenderData.testSignal = true;
}

void ecg_sender_send(Serial *pc)
{
	//Create header and calculate data size
	uint8_t header[Packetizer::HEADER_SIZE + sizeof(ECGHeader)];
	ECGHeader *ecgHeader = (ECGHeader *)(header + Packetizer::HEADER_SIZE);

	ecgHeader->lsbInMv = ecgSenderData.currLsbInMv * (1 << ECG_CHOP_BITS);
	ecgHeader->samplingFrequency = ecgSenderData.currFrequency;
	ecgHeader->channelCount = ecgSenderData.selectedChannels;

	uint32_t blockSize = (ecgHeader->channelCount + 1) * 3;
    //Number 15 is selected as an example of sending data with this simple simulator
    //When selecting this number, we have taken into accound value of blockSize and to begin with channelCount
    //Actual size value depends on data in circular buffer at this moment. 
    //This number might be changed when testing with app
    //Original implementation is available in ADS1298.cpp file on line 254
	uint32_t size = 15;
    size = size - size % blockSize;

	if (size > ECG_MAX_SEND_SIZE)
    {
		size = ECG_MAX_SEND_SIZE;
    }

	ecgHeader->sampleCount = size / blockSize;

	if (size == 0)
    {
		return;
	}

	//Initialize the compressor
	compressFifo.reset();
	ecgPredictor.reset();
	ecgPredictor.setNumChannels(ecgHeader->channelCount);
	compressor.setNumChannels(ecgHeader->channelCount);

//	ADS1298 &ecg=ADS1298::instance();

	//Compress
	for (unsigned pos = 0; pos < size; pos += blockSize)
    {
		//ecg.getSample(sampleOfChannels);

		if (ecgSenderData.testSignal)
        {
			for (int a = 0; a < ecgHeader->channelCount; a++)
            {
				ecgSenderData.sampleOfChannels[a] = testGenerator.getSample(testGenerator.getPeriod()*a/ecgHeader->channelCount);
				testGenerator.next();
			}
		}

		for (int a = 0; a < ecgHeader->channelCount; a++){
			ecgSenderData.sampleOfChannels[a] >>= ECG_CHOP_BITS;
		}
		//Compress
		compressor.putSample(ecgSenderData.sampleOfChannels);
	}

	ecgHeader->numBits = compressFifo.getAvailableBits();
	size = (ecgHeader->numBits + 7) / 8;

	//Send header
	packetizer.startPacket(header, Packetizer::ECG, (uint16_t)(size+sizeof(ECGHeader)));
	packetizer.checksumBlock((uint8_t*)ecgHeader, sizeof(ECGHeader));
    //Add serial/bluetooth communication to send header
    //(char*)header, Packetizer::HEADER_SIZE + sizeof(ECGHeader)
    
    //Calculate checksum over compressBuffer and add to final checksum value
	packetizer.checksumBlock(ecgSenderData.compressBuffer, size);
    //Add serial/bluetooth communication to send compressBuffer
    //(char*)compressBuffer,

	//Send checksum
	Packetizer::Checksum chksum = packetizer.getChecksum();

    //Add serial/bluetooth communication to send checksum
    //(char*)&chksum

    pc->printf("First message from inside function\n");
}

