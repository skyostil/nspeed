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
	~ModPlayer();

	bool	load(const char *file);
	void	unload();
	void	restart();
	
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
		char			fineTune, volume;
		unsigned short		loopStart, loopLength;
	};
	
	class ModNote
	{
	public:
		unsigned char	sampleNumber;
		unsigned short	amigaPeriod;
		short		note;
		unsigned char	effectNumber;
		unsigned char	effectParameter;
	};
	
	class ModChannel
	{
	public:
		ModChannel();
		
		ModSample	*sample;
		char		volume;
		unsigned short	amigaPeriod;
		short		note;
		
		unsigned char	portaSpeed;
		unsigned short	portaTarget;	// amiga period
		bool		glissando;
		
//		char		vibratoWaveform;
//		char		vibratoWaveformRetrig;	// four bits
		char		vibratoDepth;
		char		vibratoSpeed;
		char		vibratoPos;
		char		vibratoNeg;

//		char		tremoloWaveform;
//		char		tremoloWaveformRetrig;	// four bits
		char		tremoloDepth;
		char		tremoloSpeed;
		char		tremoloPos;
		char		tremoloNeg;
		
		char		arpeggioCounter;
				
		char		loopRow;
		char		loopCounter;
	};

	ModSample	*sample[31];
	ModNote		*note;
	ModChannel	*channel;
	char		*order;
	char		songLength, songSpeed;
	char		patternCount;
	char		currentOrder;
	char		currentTick;
	char		currentRow;
	char		patternDelay;
	
private:
	void	tick();
	void	playNote(int ch, ModNote *n);
};

#endif
