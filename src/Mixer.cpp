/***************************************************************************
 *   Copyright (C) 2004 by Sami Kyöstilä                                   *
 *   skyostil@kempele.fi                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "Mixer.h"

Channel::Channel(int _outputFreq):
	sample(0),
	pos(0),
	lspeed(0),
	hspeed(0),
	loopStart(0),
	loopLength(0),
	outputFreq(_outputFreq),
	volume(64)
{
}

bool Channel::isActive()
{
	return sample;
}

void Channel::start(Game::SampleChunk *_sample, int _freq, int _loopStart, int _loopLength)
{
	counter = 0;
	loopStart = _loopStart;
	loopLength = _loopLength;
	setSample(_sample);
	setPosition(0);
	setFrequency(_freq);
}

void Channel::setFrequency(int _freq)
{
	freq = _freq;
	hspeed = _freq  / outputFreq;
	lspeed = ((_freq % outputFreq)<<16) / outputFreq;
}

void Channel::setSample(Game::SampleChunk *_sample)
{
	sample = _sample;
}

void Channel::setPosition(int _pos)
{
	pos = _pos;
}

void Channel::setVolume(int _volume)
{
	volume = _volume;
}

void Channel::setOutputFrequency(int _outputFreq)
{
	outputFreq = _outputFreq;
}


Mixer::Mixer(int _outputFreq, int _channelCount):
	channelCount(_channelCount),
	outputFreq(_outputFreq),
	ticker(0)
{
#ifdef __VC32__
	int ch;

	channel = new Channel[channelCount];
	for(ch=0; ch<channelCount; ch++)
		channel[ch].setOutputFrequency(outputFreq);
#else
	channel = new Channel[channelCount](outputFreq);
#endif
}

Mixer::~Mixer()
{
	delete[] channel;
}

void Mixer::render(Game::SampleChunk *buffer)
{
	int ch;
	int count = buffer->length;
	
	switch(buffer->format.bytesPerSample)
	{
	case 2:
	{
		Game::Sample16 *data = (Game::Sample16*)buffer->data;
		
		while(count--)
		{
			Game::Sample32 a = 0;
			
			for(ch=0; ch<channelCount; ch++)
			{
				a+=channel[ch].play();
			}
				
			*data++ = a / channelCount;
			
			if (ticker)
			{
				if (++tickerCounter == tickerInterval)
				{
					tickerCounter = 0;
					ticker->tick();
				}
			}
		}
	}
	break;
	case 1:
	{
		Game::Sample8 *data = (Game::Sample8*)buffer->data;
		
		while(count--)
		{
			Game::Sample32 a = 0;
			
			for(ch=0; ch<channelCount; ch++)
			{
				if (channel[ch].isActive())
				{
					a+=channel[ch].play();
				}
			}
			
			*data++ = (a/channelCount)>>8;
			
			if (ticker)
			{
				if (++tickerCounter == tickerInterval)
				{
					tickerCounter = 0;
					ticker->tick();
				}
			}
		}
	}
	break;
	};
}

Channel *Mixer::playSample(Game::SampleChunk *sample, int freq, bool loop, int ch)
{
	if (ch == -1)
	{
		for(ch=0; ch<channelCount-1; ch++)
			if (!channel[ch].isActive())
				break;
	}
	channel[ch].start(sample, freq, 0, loop?sample->length:0);
	
	return &channel[ch];
}

void Mixer::installTicker(Ticker *t, int hz)
{
	ticker = t;
	tickerCounter = 0;
	tickerInterval = outputFreq / hz;
}

