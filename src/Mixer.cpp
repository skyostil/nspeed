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
#include <stdio.h>

Channel::Channel(int _outputFreq):
	sample(0),
	pos(0),
	lspeed(0),
	hspeed(0),
	loopEnd(0),
	outputFreq(_outputFreq),
	volume(0x10)
{
}

bool Channel::isActive()
{
	return sample;
}

void Channel::start(Game::SampleChunk *_sample, int _freq)
{
	sample = _sample;
	pos = 0;
	counter = 0;
	loopEnd = 0;
	hspeed = _freq  / outputFreq;
	lspeed = ((_freq % outputFreq)<<16) / outputFreq;
}

Mixer::Mixer(int _outputFreq, int _channelCount):
	channelCount(_channelCount),
	outputFreq(_outputFreq)
{
	channel = new Channel[channelCount](outputFreq);
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
			Game::Sample a = 0;
			for(ch=0; ch<channelCount; ch++)
				a+=channel[ch].play();
				
			*data++ = a;
		}
	}
	break;
	case 1:
	{
		Game::Sample8 *data = (Game::Sample8*)buffer->data;
		
		while(count--)
		{
			Game::Sample a = 0;
			for(ch=0; ch<channelCount; ch++)
				a+=channel[ch].play();
			*data++ = (a>>8);
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
	channel[ch].start(sample, freq);
	
	if (loop)
		channel[ch].loopEnd = sample->length;
	
	return &channel[ch];
}

