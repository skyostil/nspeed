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

// 16 bit internal mixing
class Channel
{
public:
        Channel(int _outputFreq = 0);

        bool    isActive();
        
        inline Game::Sample play()
        {
                Game::Sample a;
                
                if (!sample)
                        return 0;
                
                if (pos >= sample->length)
				{
                    sample = 0;
					return 0;
				}

                switch(sample->format.bytesPerSample)
                {
                case 1:
                        a = (sample->data[pos] * volume);
                break;
                case 2:
                        a = (((Game::Sample16*)sample->data)[pos] * volume) >> 8;
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
                
                return a;
        }
        
        void    start(Game::SampleChunk *_sample, int _freq, int _loopStart = 0, int _loopLength = 0);
        void    playSample(Game::SampleChunk *sample, int freq, bool loop = false);
        void    setFrequency(int _freq);
        void    setOutputFrequency(int _outputFreq);
        void    setSample(Game::SampleChunk *_sample);
        void    setPosition(int _pos);
        void    setVolume(int _volume);
        void    stop();

        Game::SampleChunk       *sample;
        int                     pos, counter, freq;
        int                     lspeed, hspeed;
        int                     loopStart, loopLength;
        int                     volume;
        int                     outputFreq;
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

        void    render(Game::SampleChunk *buffer);
        void    installTicker(Ticker *t, int hz);
        
        //! \returns the channel the sample ended up on.
        Channel *playSample(Game::SampleChunk *sample, int freq, bool loop = false, int ch = -1);
        Channel *getChannel(int ch) const;

        int     channelCount;
        int     outputFreq;
protected:
        Channel *channel;
        Ticker  *ticker;
        int     tickerCounter, tickerInterval;
};

#endif
