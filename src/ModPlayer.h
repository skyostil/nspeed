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

#ifndef MODPLAYER_H
#define MODPLAYER_H

#include "Mixer.h"
#include "engine/Engine.h"
  
// PAL
#define MOD_FREQ_BASE	(int)(7159090.5)

// NTSC
//#define MOD_FREQ_BASE	(int)(7093789.2)

class ModPlayer: private Ticker
{
public:
	ModPlayer(Mixer *_mixer);
	virtual ~ModPlayer();

	bool	load(const char *file);
	void	unload();
	void	play();
	
	Mixer	*mixer;

protected:
	unsigned short	bigEndian16(unsigned short b);
	int		amigaToHz(int period);

	int	channels;
	
	class ModSample
	{
	public:
		ModSample(int _length, char _fineTune, char _volume, unsigned short _loopStart, unsigned short _loopLength);
		~ModSample();
		
		Game::SampleChunk	*sample;
		signed char		fineTune, volume;
		unsigned short		loopStart, loopLength;
	};
	
	class ModNote
	{
	public:
		unsigned char	sampleNumber;
		unsigned short	amigaPeriod;
		signed short	note;
		unsigned char	effectNumber;
		unsigned char	effectParameter;
	};
	
	class ModChannel
	{
	public:
		ModChannel();
		
		ModSample	*sample;
		signed char	volume;
		unsigned short	amigaPeriod;
		short		note;
		
		unsigned char	portaSpeed;
		unsigned short	portaTarget;	// amiga period
		bool		glissando;
		
//		char		vibratoWaveform;
//		char		vibratoWaveformRetrig;	// four bits
		signed char	vibratoDepth;
		signed char	vibratoSpeed;
		signed char	vibratoPos;
		signed char	vibratoNeg;

//		char		tremoloWaveform;
//		char		tremoloWaveformRetrig;	// four bits
		signed char	tremoloDepth;
		signed char	tremoloSpeed;
		signed char	tremoloPos;
		signed char	tremoloNeg;
		
		signed char	arpeggioCounter;
				
		signed char	loopRow;
		signed char	loopCounter;
	};

	ModSample	*sample[31];
	ModNote		*note;
	ModChannel	*channel;
	signed char	*order;
	signed char	songLength, songSpeed;
	signed char	patternCount;
	signed char	currentOrder;
	signed char	currentTick;
	signed char	currentRow;
	signed char	patternDelay;
	
private:
	void	tick();
	void	playNote(int ch, ModNote *n);
};

#endif
