/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"
//#include "IwDebug.h"

/* Dummy SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_s3evideo.h"
#include "SDL_s3eevents_c.h"
#include "SDL_s3emouse_c.h"

#define S3EVID_DRIVER_NAME "s3e"

/* Initialization/Query functions */
static int S3E_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **S3E_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *S3E_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int S3E_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void S3E_VideoQuit(_THIS);

/* Hardware surface functions */
static int S3E_AllocHWSurface(_THIS, SDL_Surface *surface);
static int S3E_LockHWSurface(_THIS, SDL_Surface *surface);
static void S3E_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void S3E_FreeHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void S3E_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* DUMMY driver bootstrap functions */

static int S3E_Available(void)
{
#ifdef __S3E__
	return (1);
#else
	
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
	if ((envr) && (SDL_strcmp(envr, DUMMYVID_DRIVER_NAME) == 0)) {
		return(1);
	}

	return(0);
#endif
}

static void S3E_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *S3E_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = S3E_VideoInit;
	device->ListModes = S3E_ListModes;
	device->SetVideoMode = S3E_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = S3E_SetColors;
	device->UpdateRects = S3E_UpdateRects;
	device->VideoQuit = S3E_VideoQuit;
	device->AllocHWSurface = S3E_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = S3E_LockHWSurface;
	device->UnlockHWSurface = S3E_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = S3E_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = S3E_InitOSKeymap;
	device->PumpEvents = S3E_PumpEvents;
	device->CheckMouseMode = S3E_CheckMouseMode;

	device->free = S3E_DeleteDevice;

	return device;
}

VideoBootStrap S3E_bootstrap = {
	S3EVID_DRIVER_NAME, "SDL dummy video driver",
	S3E_Available, S3E_CreateDevice
};

int S3E_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	//always use 565 for now
	s3eSurfaceSetup(S3E_SURFACE_PIXEL_TYPE_RGB565, 0, NULL, S3E_SURFACE_BLIT_DIR_NATIVE);
	
	vformat->BitsPerPixel = 16;
	vformat->BytesPerPixel = 2;

	return(0);
}

SDL_Rect **S3E_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
   	 return (SDL_Rect **) -1;
}

int SurfaceScaleCopyup(s3eSurfaceCopyupInfo* info, SDL_Surface* current)
{
	uint32 dstx,dsty;
	uint32 dx = (current->w << 12) / info->m_Width;
	uint32 dy = (current->h << 12) / info->m_Height;
	uint32 srcx = 0;
	uint32 srcy = 0;
	uint16* src;

	s3eSurfaceBlitDirection dir = s3eSurfaceGetInt(S3E_SURFACE_DEVICE_BLIT_DIRECTION);

	if( dir == S3E_SURFACE_BLIT_DIR_NORMAL ) //not rot
	{
		if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB565 )
		{
			uint16* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < info->m_Height; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < info->m_Width; dstx++)
				{
					dest[dstx] = src[srcx>>12];
					srcx+=dx;
				}
				dest+=info->m_DestSurface.m_Pitch/2;
				srcy+=dy;
				srcx=0;
			}
		}
		else if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB666 )
		{
			uint32* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < info->m_Height; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < info->m_Width; dstx++)
				{
					uint32 srcp = src[srcx>>12];
					dest[dstx] = ((srcp & 0xf800) << 2) | ((srcp & 0x07e0) << 1) | ((srcp & 0x001f) << 1);
					srcx+=dx;
				}
				dest+=info->m_DestSurface.m_Pitch/4;
				srcy+=dy;
				srcx=0;
			}
		}
		else if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB888 )
		{
			uint32* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < info->m_Height; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < info->m_Width; dstx++)
				{
					uint32 srcp = src[srcx>>12];
					dest[dstx] = ((srcp & 0xf800) << 8) | ((srcp & 0x07e0) << 5) | ((srcp & 0x001f) << 3);
					srcx+=dx;
				}
				dest+=info->m_DestSurface.m_Pitch/4;
				srcy+=dy;
				srcx=0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		int32 xstep = 1;
		int32 start = 0;
		int32 ystep = info->m_DestSurface.m_Pitch;
		int32 ystart = 0;
		uint32 maxx = info->m_DestSurface.m_Width;
		uint32 maxy = info->m_DestSurface.m_Height;
		int32 depth = (info->m_DestSurface.m_PixelType & S3E_SURFACE_PIXEL_SIZE_MASK) >> 4;

		switch( dir )
		{
		case S3E_SURFACE_BLIT_DIR_NORMAL:
			break;

		case S3E_SURFACE_BLIT_DIR_ROT90:
			xstep = (int32)info->m_DestSurface.m_Pitch / depth;
			ystep = -1;
			dx = (current->w << 12) / info->m_Height;
			dy = (current->h << 12) / info->m_Width;
			start = (info->m_DestSurface.m_Width - 1);
			maxx = info->m_DestSurface.m_Height;
			maxy = info->m_DestSurface.m_Width;
			break;

		case S3E_SURFACE_BLIT_DIR_ROT180:
			xstep = -1;
			ystep = -(int32)info->m_DestSurface.m_Pitch / depth;
			start = (info->m_DestSurface.m_Height - 1) * (info->m_DestSurface.m_Pitch / depth)
				+ info->m_DestSurface.m_Width - 1;
			break;

		case S3E_SURFACE_BLIT_DIR_ROT270:
			xstep = -(int32)info->m_DestSurface.m_Pitch / depth;
			ystep = 1;
			dx = (current->w << 12) / info->m_Height;
			dy = (current->h << 12) / info->m_Width;
			start = (info->m_DestSurface.m_Height - 1) * (info->m_DestSurface.m_Pitch / depth);
			maxx = info->m_DestSurface.m_Height;
			maxy = info->m_DestSurface.m_Width;
			break;
		}
		
#define DEST_LOCATION(_x, _y) dest[start + (_x*xstep) + _y*ystep]

		if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB565 )
		{
			uint16* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < maxy; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < maxx; dstx++)
				{
					DEST_LOCATION(dstx, dsty) = src[srcx>>12];
					srcx+=dx;
				}
				srcy+=dy;
				srcx=0;
			}
		}
		else if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB666 )
		{
			uint32* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < maxy; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < maxx; dstx++)
				{
					uint32 srcp = src[srcx>>12];
					DEST_LOCATION(dstx, dsty) = ((srcp & 0xf800) << 2) | ((srcp & 0x07e0) << 1) | ((srcp & 0x001f) << 1);
					srcx+=dx;
				}
				srcy+=dy;
				srcx=0;
			}
		}
		else if( info->m_DestSurface.m_PixelType == S3E_SURFACE_PIXEL_TYPE_RGB888 )
		{
			uint32* dest = info->m_DestSurface.m_Data;
			for(dsty = 0; dsty < maxy; dsty++)
			{
				src = (uint16*)current->pixels + (srcy >> 12) * current->w;
				for(dstx = 0; dstx < maxx; dstx++)
				{
					uint32 srcp = src[srcx>>12];
					DEST_LOCATION(dstx, dsty) = ((srcp & 0xf800) << 8) | ((srcp & 0x07e0) << 5) | ((srcp & 0x001f) << 3);
					srcx+=dx;
				}
				srcy+=dy;
				srcx=0;
			}
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

SDL_Surface *S3E_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	int sw = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
	int sh = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);
	int offScreenSurface = 0;

	if( width != sw || height != sh ) 
	{
		int scale = 1;
		s3eConfigGetInt("SDL", "AllowScale", &scale);
		if( !scale )
		{
			//IwError(("VideoMode selected (%d, %d) is greater than s3e surface size", width, height));
		}
		else
		{
			offScreenSurface = 1;
			s3eSurfaceRegister(S3E_SURFACE_COPYUP, (s3eCallback)SurfaceScaleCopyup, current);
		}
	}
	//todo: centre this...
	current->w = width;
	current->h = height;
	current->flags = (flags & SDL_FULLSCREEN);
	if( offScreenSurface )
	{
		current->pixels = s3eMalloc(width*height*2);
		current->pitch = width * 2;
		this->hidden->m_xFactor = (width << 12) / sw;
		this->hidden->m_yFactor = (height << 12) / sh;
	}
	else
	{
		current->flags |= SDL_PREALLOC;
		current->pitch = (uint16)s3eSurfaceGetInt(S3E_SURFACE_PITCH);
		current->pixels = s3eSurfacePtr();
		this->hidden->m_xFactor = 1 << 12;
		this->hidden->m_yFactor = 1 << 12;
	}

/*	unsigned short* ptr = (uint16*)s3eSurfacePtr();
	int y = 0;
	for(; y < height; y++)
	{
		memset(ptr, 0, 2*width);
		ptr+=current->pitch;
	}*/
	return current;
}

/* We don't actually allow hardware surfaces other than the main one */
static int S3E_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void S3E_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int S3E_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void S3E_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

static void S3E_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	int usingS3ESurface = SDL_VideoSurface->flags & SDL_PREALLOC;
	int i;

	if( usingS3ESurface ) //TODO: could scale rects here, I guess... but scared of off-by-one errors...
	{
		for(i = 0; i < numrects; i++)
		{
			s3eSurfaceInvalidate(rects[i].x, rects[i].y, rects[i].w, rects[i].h);
		}
	}

	s3eSurfaceShow();
	
	//TODO: if surface changes size need to update entire surface state
	if(usingS3ESurface)
	{
		SDL_VideoSurface->pixels = s3eSurfacePtr();
	}
	//TODO: what about rotation with the copyup?
}

int S3E_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void S3E_VideoQuit(_THIS)
{
}


