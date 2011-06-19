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

    This file written by Ryan C. Gordon (icculus@icculus.org)
*/
#include "SDL_config.h"

/* Output audio to nowhere... */

#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_s3eaudio.h"

/* The tag name used by DUMMY audio */
#define S3EAUD_DRIVER_NAME         "dummy"

/* Audio driver functions */
static int S3EAUD_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void S3EAUD_WaitAudio(_THIS);
static void S3EAUD_PlayAudio(_THIS);
static Uint8 *S3EAUD_GetAudioBuf(_THIS);
static void S3EAUD_CloseAudio(_THIS);

/* Audio driver bootstrap functions */
static int S3EAUD_Available(void)
{
	return(1);
}

static void S3EAUD_DeleteDevice(SDL_AudioDevice *device)
{
	S3EAUD_CloseAudio(device);

	SDL_free(device->hidden);
	SDL_free(device);
}


static void GenerateAudioMonoCB(s3eSoundGenAudioInfo* info, SDL_AudioDevice* dev)
{
	int numSamples = info->m_NumSamples;
	int mixpos = dev->hidden->mixpos;
	int mixlen = dev->hidden->mixlen;
	int16* mixbuf = (int16*)dev->hidden->mixbuf;

	while (numSamples > 0)
	{
		int copy = (mixlen - mixpos) >> 1; 
		int c;
		if( numSamples*2 < copy )
			copy = numSamples * 2;
		c = copy >> 1;
		while(c--)
		{
			int16 val = ((mixbuf)[mixpos>>1] >> 1) + ((mixbuf)[(mixpos>>1)+1] >> 1);
			*info->m_Target++ = val;
			mixpos += 4;
		}

		numSamples -= copy >> 1;
		if( mixpos >= mixlen )
		{
			SDL_RunAudio(dev);
			mixpos = 0;
			dev->enabled = 1;
		}
	}

	dev->hidden->mixpos = mixpos;
}

static void GenerateAudioStereoCB(s3eSoundGenAudioInfo* info, SDL_AudioDevice* dev)
{
	int numSamples = info->m_NumSamples;
	int mixpos = dev->hidden->mixpos;
	int mixlen = dev->hidden->mixlen;
	int16* mixbuf = (int16*)dev->hidden->mixbuf;

	while (numSamples > 0)
	{
		int copy = (mixlen - mixpos) >> 1; 
		int c;
		if( numSamples*2 < copy )
			copy = numSamples * 2;
		c = copy >> 1;
		while(c--)
		{
			int16 val = mixbuf[mixpos>>1] >> 1;
			*info->m_Target++ = val;
			val = mixbuf[(mixpos>>1)+1] >> 1;
			*info->m_Target++ = val;
			mixpos += 4;
		}

		numSamples -= copy >> 1;
		if( mixpos >= mixlen )
		{
			SDL_RunAudio(dev);
			mixpos = 0;
			dev->enabled = 1;
		}
	}

	dev->hidden->mixpos = mixpos;
}

int GenerateAudioCB(s3eSoundGenAudioInfo* info, SDL_AudioDevice* dev)
{
	if (!dev || !dev->spec.callback || dev->paused == 1)
	{
		if(!info->m_Mix )
			memset(info->m_Target, 0, info->m_NumSamples*2);
		return info->m_NumSamples;
	}

	if (info->m_Stereo)
		GenerateAudioStereoCB(info, dev);
	else
		GenerateAudioMonoCB(info, dev);
	return info->m_NumSamples;

}
static int16 dummy_sound_buf[2000];

static SDL_AudioDevice *S3EAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	/* Set the function pointers */
	this->OpenAudio = S3EAUD_OpenAudio;
	this->WaitAudio = S3EAUD_WaitAudio;
	this->PlayAudio = S3EAUD_PlayAudio;
	this->GetAudioBuf = S3EAUD_GetAudioBuf;
	this->CloseAudio = S3EAUD_CloseAudio;

	this->free = S3EAUD_DeleteDevice;

	s3eSoundChannelRegister(0, S3E_CHANNEL_GEN_AUDIO, (s3eCallback)GenerateAudioCB, this);
	s3eSoundChannelRegister(0, S3E_CHANNEL_GEN_AUDIO_STEREO, (s3eCallback)GenerateAudioCB, this);
	s3eSoundChannelPlay(0, dummy_sound_buf, 1000, -1, 0);
	this->paused = 1;
//	s3eSoundChannelPause(0);

	return this;
}

AudioBootStrap S3EAUD_bootstrap = {
	S3EAUD_DRIVER_NAME, "SDL s3e audio driver",
	S3EAUD_Available, S3EAUD_CreateDevice
};

/* This function waits until it is possible to write a full sound buffer */
static void S3EAUD_WaitAudio(_THIS)
{
	this->enabled = 0; //force sdl audio loop to exit
}

static void S3EAUD_PlayAudio(_THIS)
{
	this->paused = 0;
}

static Uint8 *S3EAUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static void S3EAUD_CloseAudio(_THIS)
{
	this->spec.callback = NULL;
	s3eSoundChannelUnRegister(0, S3E_CHANNEL_GEN_AUDIO_STEREO);
	s3eSoundChannelUnRegister(0, S3E_CHANNEL_GEN_AUDIO);
	s3eSoundChannelStop(0);

	s3eDeviceYield(100);
	
	if ( this->hidden->mixbuf != NULL ) {
		SDL_FreeAudioMem(this->hidden->mixbuf);
		this->hidden->mixbuf = NULL;
	}
}

static int S3EAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	spec->freq = s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ);		/* DSP frequency -- samples per second */
	spec->format = AUDIO_S16;		/* Audio data format */
	
	//setting stereo here so Kobo works...!
	spec->channels = 2;	/* Number of channels: 1 mono, 2 stereo */

//	float bytes_per_sec = 0.0f;

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *) SDL_AllocAudioMem(this->hidden->mixlen);
	if ( this->hidden->mixbuf == NULL ) {
		return(-1);
	}
	SDL_memset(this->hidden->mixbuf, spec->silence, spec->size);

//	bytes_per_sec = (float) (((spec->format & 0xFF) / 8) *
	//                   spec->channels * spec->freq);

	/*
	 * We try to make this request more audio at the correct rate for
	 *  a given audio spec, so timing stays fairly faithful.
	 * Also, we have it not block at all for the first two calls, so
	 *  it seems like we're filling two audio fragments right out of the
	 *  gate, like other SDL drivers tend to do.
	 */
/*	this->hidden->initial_calls = 2;
	this->hidden->write_delay =
	               (Uint32) ((((float) spec->size) / bytes_per_sec) * 1000.0f); */

	/* We're ready to rock and roll. :-) */
	return(1);
}

