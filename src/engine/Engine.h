#ifndef GAMEENGINE_H
#define GAMEENGINE_H

namespace Game
{

typedef unsigned int    Pixel32;
typedef unsigned short  Pixel16;
typedef unsigned char   Pixel8;
typedef Pixel16         Pixel;          // internal format

typedef signed int      Sample32;
typedef signed short    Sample16;
typedef signed char     Sample8;
typedef Sample16        Sample;         // default format

typedef unsigned long	Size;

class PixelFormat
{
public:
        PixelFormat(int _bitsPerPixel);
        //! Create pixel format with the given masks and shifts.
        PixelFormat(int _bitsPerPixel, int rm, int rs, int gm, int gs, int bm, int bs);

        inline Pixel    makePixel(Pixel8 r, Pixel8 g, Pixel8 b)
        {
                return (
                        ((r>>(8-rsize))<<rshift) +
                        ((g>>(8-gsize))<<gshift) +
                        ((b>>(8-bsize))<<bshift)
                );
        }
        
        inline void     makeRGB(Pixel p, Pixel8 *r, Pixel8 *g, Pixel8 *b)
        {
                *r = ((p&rmask)>>rshift)<<(8-rsize);
                *g = ((p&gmask)>>gshift)<<(8-gsize);
                *b = ((p&bmask)>>bshift)<<(8-bsize);
        }
        
                
        int bytesPerPixel;
        int bitsPerPixel;
        int rmask, rshift, rsize;
        int gmask, gshift, gsize;
        int bmask, bshift, bsize;
};

class Surface
{
public:
        Surface(PixelFormat* _format, Pixel* _pixels, int _width, int _height);
        Surface(PixelFormat* _format, int _width, int _height);
        Surface(PixelFormat* _format, Surface* s);      // makes a copy with the given pixel format
        virtual ~Surface();
        
        //! Clears the surface. \param pixel must be in the correct format.
        void                    clear(Pixel color = 0);
        
        // warning: these are slow
        
        //! \param pixel must be in the correct format.
        Pixel                   getPixel(int x, int y);
        //! \param pixel must be in the correct format.
        void                    setPixel(int x, int y, Pixel color);

        Pixel                   *pixels;
        int                     width, height, bytes;
        bool                    autoDelete;
        PixelFormat             format;
};

class SampleFormat
{
public:
        SampleFormat(int _bits, int _channels);
        
        //! Convert from the default sample format to this format.
        inline Sample makeSample(Sample sample)
        {
                return sample>>(sizeof(Sample)*8-bits);
        }

        int                     bits, bytesPerSample;
        int                     channels;
};

class SampleChunk
{
public:
        SampleChunk(SampleFormat* _format, Sample8* _data, int _length, int _rate);
        SampleChunk(SampleFormat* _format, int _length, int _rate);
        virtual ~SampleChunk();
        
        //! \param sample must be in the correct format.
        void    setSample(int n, int channel, Sample sample);

        Sample8*	data;
        int             length;
        int             rate;
        int             bytes;
        bool            autoDelete;
        SampleFormat    format;
};

class Event
{
public:
        enum Type
        {
                NullEvent,
                KeyPressEvent,
                KeyReleaseEvent,
                KeyEvent,
                PointerMoveEvent,
                PointerButtonPressedEvent,
                PointerButtonReleasedEvent,
                ExitEvent,
        };

        Event(Type t=NullEvent): type(t) {}

        Type    type;

        union
        {
                struct
                {
                        int code;
                } key;
                struct
                {
                        int x, y;
                        int button;
                } pointer;
        };
};

class Framework
{
public:
        Framework();
        virtual ~Framework();

        virtual void exit() = 0;
        virtual unsigned int getTickCount() = 0;
        virtual unsigned int getTicksPerSecond() = 0;
        
		virtual const char *findResource(const char *name) { return name; }

        virtual Surface *loadImage(const char *name, PixelFormat *pf = 0);
        virtual SampleChunk *loadSample(const char *name, SampleFormat *sf = 0);
};

class Engine
{
public:
        Engine(Framework* _framework);
        virtual ~Engine();

        virtual void renderVideo(Surface* screen) {};
        virtual void renderAudio(SampleChunk* sample) {};
        virtual void handleEvent(Event* event) {};

        virtual void configureVideo(Surface* screen) {};
        virtual void configureAudio(SampleChunk* sample) {};
protected:
        Framework*      framework;
};


extern "C"
{

extern Engine*  CreateEngine(Framework *framework);

};


};

#endif
