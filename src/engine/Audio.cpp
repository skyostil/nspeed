#include "engine/Engine.h"

using namespace Game;

SampleFormat::SampleFormat(int _bits, int _channels):
        bits(_bits),
        channels(_channels)
{
        bytesPerSample = bits >> 3;
}

SampleChunk::SampleChunk(SampleFormat* _format, Sample8* _data, int _length, int _rate):
        format(*_format),
        autoDelete(false),
        data(_data),
        length(_length),
        rate(_rate)
{
        bytes = length * (format.bits>>3) * format.channels;
}

SampleChunk::SampleChunk(SampleFormat* _format, int _length, int _rate):
        format(*_format),
        autoDelete(true),
        data(0),
        length(_length),
        rate(_rate)
{
        bytes = length * (format.bits>>3) * format.channels;
        data = (Sample8*)new char[bytes];
}

SampleChunk::~SampleChunk()
{
        if (autoDelete)
        {
                delete[] data;
        }
        data = 0;
}

void SampleChunk::setSample(int n, int channel, Sample sample)
{
        switch(format.bits)
        {
        case 8:
        {
                ((Sample8*)data)[n*format.channels + channel] = sample;
        }
        break;
        case 16:
        {
                ((Sample16*)data)[n*format.channels + channel] = sample;
        }
        break;
        }
}


