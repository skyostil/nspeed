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

#ifndef MIXER_H
#define MIXER_H

#include "engine/Engine.h"
#include "FixedPointMath.h"

#include <stdio.h>

// 16 bit internal mixing
class Channel
{
public:
	Channel(int _outputFreq);

	bool	isActive();
	
	inline Game::Sample play()
	{
		Game::Sample a;
		
		if (!sample)
			return 0;
		
		switch(sample->format.bytesPerSample)
		{
		case 1:
			a = sample->data[pos] * volume;
		break;
		case 2:
			a = (((unsigned short*)sample->data)[pos] * volume) >> 8;
		break;
		}
		
		counter += lspeed;
		pos += hspeed;
		
		if (counter>0xffff)
		{
			counter -= 0xffff;
			pos++;
		}
		
		if (loopLength)
			if (pos>=loopStart+loopLength)
			{
				pos-=loopLength;
			}
		
		if (pos >= sample->length)
			sample = 0;
		
		return a;
	}
	
	void	start(Game::SampleChunk *_sample, int _freq, int _loopStart = 0, int _loopLength = 0);

	Game::SampleChunk	*sample;
	int			pos, counter, freq;
	int			lspeed, hspeed;
	int			loopStart, loopLength;
	int			volume;
	int			outputFreq;
};

class Ticker
{
public:
	virtual void tick() = 0;
};

class Mixer
{
public:
	Mixer(int _outputFreq, int _channelCount = 8);
	~Mixer();

	void	render(Game::SampleChunk *buffer);
	void	installTicker(Ticker *t, int bpm);
	
	//! \returns the channel the sample ended up on.
	Channel	*playSample(Game::SampleChunk *sample, int freq, bool loop = false, int ch = -1);

	Channel	*channel;
	int	channelCount;
	int	outputFreq;
protected:
	Ticker	*ticker;
	int	tickerCounter, tickerInterval;
};

// PAL
//#define MOD_FREQ_BASE	(int)(7159090.5)

// NTSC
#define MOD_FREQ_BASE	(int)(7093789.2)


class ModPlayer: private Ticker
{
public:
	ModPlayer(Mixer *_mixer);
	~ModPlayer();

	bool	load(const char *file);
	void	unload();
	void	restart();
	
	Mixer	*mixer;

protected:
	unsigned short	bigEndian16(unsigned short b);
	int		amigaToHz(int amigaval);

	int	channels;
	
	class ModSample
	{
	public:
		ModSample(int _length, char _fineTune, char _volume, unsigned short _loopStart, unsigned short _loopLength);
		~ModSample();
		
		Game::SampleChunk	*sample;
		char			fineTune, volume;
		unsigned short		loopStart, loopLength;
	};
	
	class ModNote
	{
	public:
		unsigned char	sampleNumber;
		unsigned char	periodFrequency;
		unsigned char	effectNumber;
		unsigned char	effectParameter;
	};

	ModSample	*sample[31];
	ModNote		*note;
	char		order[128]; // XXX - allocate this dynamically
	char		songLength;
	char		patternCount;
	char		currentOrder;
	char		currentTick;
	char		currentRow;

private:
	void	tick();
	void	playNote(Channel *channel, ModNote *n);
};

#endif
