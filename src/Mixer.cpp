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

Mixer::Mixer(int _outputFreq, int _channelCount):
	channelCount(_channelCount),
	outputFreq(_outputFreq),
	ticker(0)
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
//	tickerInterval = outputFreq / 32;
}


static const unsigned short periodTable[][36] =
{
	{
	// Tuning 0, Normal
		856,808,762,720,678,640,604,570,538,508,480,453, // C-1 to B-1
		428,404,381,360,339,320,302,285,269,254,240,226, // C-2 to B-2
		214,202,190,180,170,160,151,143,135,127,120,113, // C-3 to B-3
	},
	{
	// Tuning 1
		850,802,757,715,674,637,601,567,535,505,477,450, // same as above
		425,401,379,357,337,318,300,284,268,253,239,225, // but with 
		213,201,189,179,169,159,150,142,134,126,119,113, // finetune +1
	},
	{
	// Tuning 2
		844,796,752,709,670,632,597,563,532,502,474,447, // etc, 
		422,398,376,355,335,316,298,282,266,251,237,224, // finetune +2
		211,199,188,177,167,158,149,141,133,125,118,112,
	},
	{
	// Tuning 3
		838,791,746,704,665,628,592,559,528,498,470,444,
		419,395,373,352,332,314,296,280,264,249,235,222,
		209,198,187,176,166,157,148,140,132,125,118,111,
	},
	{
	// Tuning 4
		832,785,741,699,660,623,588,555,524,495,467,441,
		416,392,370,350,330,312,294,278,262,247,233,220,
		208,196,185,175,165,156,147,139,131,124,117,110,
	},
	{
	// Tuning 5
		826,779,736,694,655,619,584,551,520,491,463,437,
		413,390,368,347,328,309,292,276,260,245,232,219,
		206,195,184,174,164,155,146,138,130,123,116,109,
	},
	{
	// Tuning 6
		820,774,730,689,651,614,580,547,516,487,460,434,
		410,387,365,345,325,307,290,274,258,244,230,217,
		205,193,183,172,163,154,145,137,129,122,115,109,
	},
	{
	// Tuning 7
		814,768,725,684,646,610,575,543,513,484,457,431,
		407,384,363,342,323,305,288,272,256,242,228,216,
		204,192,181,171,161,152,144,136,128,121,114,108,
	},
	{
	// Tuning -8
		907,856,808,762,720,678,640,604,570,538,508,480,
		453,428,404,381,360,339,320,302,285,269,254,240,
		226,214,202,190,180,170,160,151,143,135,127,120,
	},
	{
	// Tuning -7
		900,850,802,757,715,675,636,601,567,535,505,477,
		450,425,401,379,357,337,318,300,284,268,253,238,
		225,212,200,189,179,169,159,150,142,134,126,119,
	},
	{
	// Tuning -6
		894,844,796,752,709,670,632,597,563,532,502,474,
		447,422,398,376,355,335,316,298,282,266,251,237,
		223,211,199,188,177,167,158,149,141,133,125,118,
	},
	{
	// Tuning -5
		887,838,791,746,704,665,628,592,559,528,498,470,
		444,419,395,373,352,332,314,296,280,264,249,235,
		222,209,198,187,176,166,157,148,140,132,125,118,
	},
	{
	// Tuning -4
		881,832,785,741,699,660,623,588,555,524,494,467,
		441,416,392,370,350,330,312,294,278,262,247,233,
		220,208,196,185,175,165,156,147,139,131,123,117,
	},
	{
	// Tuning -3
		875,826,779,736,694,655,619,584,551,520,491,463,
		437,413,390,368,347,328,309,292,276,260,245,232,
		219,206,195,184,174,164,155,146,138,130,123,116,
	},
	{
	// Tuning -2
		868,820,774,730,689,651,614,580,547,516,487,460,
		434,410,387,365,345,325,307,290,274,258,244,230,
		217,205,193,183,172,163,154,145,137,129,122,115,
	},
	{
	// Tuning -1
		862,814,768,725,684,646,610,575,543,513,484,457,
		431,407,384,363,342,323,305,288,272,256,242,228,
		216,203,192,181,171,161,152,144,136,128,121,114,
	}
};


ModPlayer::ModPlayer(Mixer *_mixer):
	mixer(_mixer),
	channels(0),
	songSpeed(6),
	channel(0),
	note(0)
{
	int i;
	for(i=0; i<sizeof(sample)/sizeof(sample[0]); i++)
		sample[i] = 0;
	restart();
}

ModPlayer::~ModPlayer()
{
	unload();
}

#define PACKED __attribute__((packed)) 

bool ModPlayer::load(const char *file)
{
	typedef struct
	{
		char		name[22] PACKED;
		unsigned short	length PACKED;
		char		fineTune PACKED;
		char		volume PACKED;
		unsigned short	loopStart PACKED;
		unsigned short	loopLength PACKED;
	} SampleHeader;
	
	typedef struct
	{
		unsigned char	byte0 PACKED;
		unsigned char	byte1 PACKED;
		unsigned char	byte2 PACKED;
		unsigned char	byte3 PACKED;
	} Note;
		
	FILE *f = fopen(file,"rb");
	char modType[4];
	int i, j, k;
	
	if (!f)
		return false;
	
	unload();
	
	fseek(f,1080,SEEK_SET);
	fread(modType, sizeof(modType), 1, f);
	
	// read module type
	if (modType[0]=='M' && modType[1]=='.' && modType[2]=='K' && modType[3]=='.')
		channels = 4;
	else if (modType[0]=='6' && modType[1]=='C' && modType[2]=='H' && modType[3]=='N')
		channels = 6;
	else if (modType[0]=='8' && modType[1]=='C' && modType[2]=='H' && modType[3]=='N')
		channels = 8;
	else
		// no support for old 15 sample mods
		return false;
		
	channel = new ModChannel[channels];
		
	// read sample information (skip the module name)
	fseek(f,20,SEEK_SET);	
	for(i=0; i<31; i++)
	{
		SampleHeader header;
		fread(&header, sizeof(SampleHeader), 1, f);
		
		// fix endianess
		header.length = bigEndian16(header.length) * 2;
		header.loopStart = bigEndian16(header.loopStart);
		header.loopLength = bigEndian16(header.loopLength);
		
//		if (header.fineTune > 7)
//			header.fineTune -= 16;
		
		if (header.length > 1)
		{
			sample[i] = new ModSample(header.length, header.fineTune, header.volume,
			                          header.loopStart, header.loopLength);
		}
	}
	
	songLength = fgetc(f);
	fseek(f,1,SEEK_CUR); // skip unused byte
	
	// read pattern order and figure out pattern count
	fread(&order, sizeof(order), 1, f);
	patternCount = 0;
	for(i=0; i<128; i++)
	{
		if (order[i] > patternCount-1)
			patternCount = order[i]+1;
	}
	
	fseek(f,4,SEEK_CUR); // skip module id
		
	note = new ModNote[channels * 64 /* rows per channel */ * patternCount];
	
	// read notes
	ModNote *n = note;
	for(i=0; i<patternCount; i++)
		for(j=0; j<channels*64; j++)
		{
			Note d;
			
			fread(&d, sizeof(Note), 1, f);
			
			n->sampleNumber = (d.byte0 & 0xf0) + (d.byte2 >> 4);
			n->effectNumber = d.byte2 & 0xf;
			n->effectParameter = d.byte3;
			n->amigaPeriod = ((d.byte0 & 0xf) << 8) + d.byte1;
			
			n->note = -1;
			
			if (n->amigaPeriod > 0)
				for(k=0; k<37; k++)
					if (n->amigaPeriod > periodTable[0][k]-2 && n->amigaPeriod < periodTable[0][k]+2)
					{
						n->note = k;
					}
			
			n++;
		}
	
	// read sample data
	for(i=0; i<31; i++)
	{
		if (sample[i])
		{
			fread(sample[i]->sample->data, sizeof(char), sample[i]->sample->bytes, f); 
			for(j=0; j<sample[i]->sample->length; j++)
				((char*)(sample[i]->sample->data))[j] -= 0x80;
		}
	}
//	mixer->playSample(sample[i]->sample, sample[i]->sample->rate);

	restart();
	fclose(f);
}

unsigned short ModPlayer::bigEndian16(unsigned short x)
{
	return (x>>8) + ((x&0xff)<<8);
}

ModPlayer::ModSample::ModSample(int _length, char _fineTune, char _volume, unsigned short _loopStart, unsigned short _loopLength):
	fineTune(_fineTune),
	volume(_volume),
	loopStart(_loopStart),
	loopLength(_loopLength)
{
	Game::SampleFormat sf(8,1);
	sample = new Game::SampleChunk(&sf, _length, MOD_FREQ_BASE / (428*2));
}

ModPlayer::ModSample::~ModSample()
{
	delete sample;
}

void ModPlayer::unload()
{
	int i;
	for(i=0; i<sizeof(sample)/sizeof(sample[0]); i++)
	{
		delete sample[i];
		sample[i] = 0;
	}
	delete[] note;
	delete[] channel;
}

void ModPlayer::restart()
{
	currentOrder = 0;
	currentTick = 0;
	currentRow = 0;
	patternDelay = 0;
	mixer->installTicker(this, 2 * 125 / 5);
}

void ModPlayer::tick()
{
	// cache values so effects won't mess them up
	int ch;
	int cr = currentRow;
	int co = currentOrder;

	printf("%d/%d %2d:%02d: ", currentTick, songSpeed, order[currentOrder], currentRow);
	
	for(ch=0; ch<channels; ch++)
//	ch=3;
	{
		ModNote *n = &note[order[co]*(channels*64) + cr*channels + ch];
		
		if (currentTick == 0)
			if (!patternDelay)
				playNote(ch, n);
		else
		{
			char x = n->effectNumber >> 4;
			char y = n->effectNumber & 4;
			
			switch(n->effectNumber)
			{
			case 0x0: // arpeggio
				switch((currentTick+1) % 3)
				{
				case 0:
					mixer->getChannel(ch)->setFrequency(amigaToHz(n->amigaPeriod));
				break;
				case 1:
					mixer->getChannel(ch)->setFrequency(amigaToHz(periodTable[x][n->note]));
				break;
				case 2:
					mixer->getChannel(ch)->setFrequency(amigaToHz(periodTable[y][n->note]));
				break;
				}
			break;
			}
		}
	}
	printf("\n");
	
	if (++currentTick == songSpeed)
	{
		currentTick = 0;
		
		if (patternDelay > 0)
			patternDelay--;
		else
		{
			currentRow++;
		
			if (currentRow >= 64)
			{
				currentRow = 0;
				currentOrder++;
				if (currentOrder >= songLength)
					currentOrder = 0;
			}
		}
	}
}

int ModPlayer::amigaToHz(int period)
{
	return MOD_FREQ_BASE / (period*2);
}

void ModPlayer::playNote(int ch, ModNote *n)
{
	if (n->sampleNumber)
	{
		ModSample *s = sample[n->sampleNumber-1];
		channel[ch].sample = s;
		channel[ch].volume = s->volume;
		mixer->getChannel(ch)->setSample(s->sample);
		mixer->getChannel(ch)->setVolume(s->volume);
//		printf("%d\n", s->volume);
	}
	
	if (n->note != -1)
	{
		if (n->effectNumber != 3 && n->effectNumber != 5)
		{
			int freq = amigaToHz(periodTable[channel[ch].sample->fineTune][n->note]);
//			printf("%d -> %d\n", n->note, periodTable[channel[ch].sample->fineTune][n->note]);
//			mixer->getChannel(ch)->setFrequency(freq);
//			mixer->getChannel(ch)->setPosition(0);
			
			channel[ch].amigaPeriod = n->amigaPeriod;
			mixer->getChannel(ch)->start(channel[ch].sample->sample, freq, channel[ch].sample->loopStart, channel[ch].sample->loopLength);
		}
	}
	
	// process tick 0 effects
	switch(n->effectNumber)
	{
	case 0x3: // porta to note
	
	break;
	case 0x9: // sample offset
		mixer->getChannel(ch)->setPosition(n->effectParameter << 8);
	break;
	case 0xb: // jump to pattern
		currentOrder = (n->effectParameter>songLength-1)?(songLength-1):n->effectParameter;
	break;
	case 0xc: // set volume
		mixer->getChannel(ch)->setVolume(n->effectParameter);
	break;
	case 0xd: // pattern break
		currentRow = n->effectParameter;
		currentOrder++;
		if (currentRow > 63) currentRow = 0;
		if (currentOrder > songLength-1) currentOrder = 0;
	break;
	case 0xf: // set speed
		if (n->effectParameter < 0x1f)
			songSpeed = n->effectParameter;
		else
			mixer->installTicker(this, 2 * n->effectParameter / 5);
	break;
	case 0xe:
	{
		unsigned char x = n->effectParameter & 0xf;
		switch(n->effectParameter >> 4)
		{
		case 0x1: // fine porta up
			channel[ch].amigaPeriod -= x;
			mixer->getChannel(ch)->setFrequency(amigaToHz(channel[ch].amigaPeriod));
		break;
		case 0x2: // fine porta down
			channel[ch].amigaPeriod += x;
			mixer->getChannel(ch)->setFrequency(amigaToHz(channel[ch].amigaPeriod));
		break;
		case 0x3: // glissando control
			channel[ch].glissando = x?true:false;
		break;
		case 0x4: // set vibrato waveform
			if (x < 4)
				channel[ch].vibratoWaveform = x;
			else
				channel[ch].vibratoWaveformRetrig = (1<<x);
		break;
		case 0x5: // set finetune
			channel[ch].sample->fineTune = x;
		break;
		case 0x6: // loop
			if (x == 0)
				channel[ch].loopRow = currentRow;
			else
			{
				if (!channel[ch].loopCounter)
					channel[ch].loopCounter = x;
				else
					channel[ch].loopCounter--;
					
				if (channel[ch].loopCounter)
					currentRow = channel[ch].loopRow-1;
			}
		break;
		case 0x7: // set tremolo waveform
			if (x < 4)
				channel[ch].tremoloWaveform = x;
			else
				channel[ch].tremoloWaveformRetrig = (1<<x);
		break;
		case 0x8: // panning
		break;
		case 0xa: // fine volumeslide up
			channel[ch].volume += x;
			if (channel[ch].volume > 64)
				channel[ch].volume = 64;
			mixer->getChannel(ch)->setVolume(channel[ch].volume);
		break;
		case 0xb: // fine volumeslide down
			channel[ch].volume -= x;
			if (channel[ch].volume < 0)
				channel[ch].volume = 0;
			mixer->getChannel(ch)->setVolume(channel[ch].volume);
		break;
		case 0xe: // pattern delay
			patternDelay = x;
		break;
		}
	}
	break;
	}
	
/*
	if (s)
	{
		int freq = channel->freq;
		
		if (n->note != -1)
		{
			freq = amigaToHz(n->periodFrequency);
//			freq>>=2;
		}
		
		channel->start(s->sample, freq, s->loopStart, s->loopLength);
	}
*/	
}
/*
void ModPlayer::updateAmigaPeriod(int ch)
{
	int freq = amigaToHz(channel[ch].amigaPeriod);
	mixer->getChannel(ch)->setFrequency(freq);
}
*/

ModPlayer::ModChannel::ModChannel():
	sample(NULL),
	glissando(false),
	portaSpeed(0),
	vibratoWaveform(0),
	vibratoWaveformRetrig(0),
	tremoloWaveform(0),
	tremoloWaveformRetrig(0),
	volume(0xff),
	arpeggioCounter(0),
	loopRow(0),
	loopCounter(0)
{
}

