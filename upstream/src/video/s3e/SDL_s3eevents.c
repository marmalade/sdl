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

/* Being a null driver, there's no event stream. We just define stubs for
   most of the API. */

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"

#include "SDL_s3evideo.h"
#include "SDL_s3eevents_c.h"

SDLKey s_s3eToSDLTranslation[s3eKeyCount] = 
{
	0, //s3eKeyFirst
	SDLK_ESCAPE,//	s3eKeyEsc ,				//!< Esc.
	SDLK_TAB,//	s3eKeyTab ,				//!< Tab.
	SDLK_BACKSPACE,//	s3eKeyBackspace ,		//!< Backspace.
	SDLK_RETURN,//  s3eKeyEnter ,			//!< Enter.
	SDLK_LSHIFT,//  s3eKeyShift ,			//!< Key Shift.
	SDLK_LCTRL,//  s3eKeyControl ,			//!< Key Control. 
	0,//  s3eKeyResevered , 		//!< Reserved, do not use.
	SDLK_SPACE,//  s3eKeySpace ,			//!< Key Space.
	SDLK_LEFT,//  s3eKeyLeft ,			//!< Key Left. 
	SDLK_UP,//  s3eKeyUp ,				//!< Key Up. 
	SDLK_RIGHT,//  s3eKeyRight ,			//!< Key Right. 
	SDLK_DOWN,//  s3eKeyDown ,			//!< Key Down. 
	SDLK_0,//  s3eKey0 ,				//!< Key 0. 
	SDLK_1,//  s3eKey1 ,				//!< Key 1. 
	SDLK_2,//  s3eKey2 ,				//!< Key 2. 
	SDLK_3,//  s3eKey3 ,				//!< Key 3. 
	SDLK_4,//  s3eKey4 ,				//!< Key 4. 
	SDLK_5,//  s3eKey5 ,				//!< Key 5. 
	SDLK_6,//  s3eKey6 ,				//!< Key 6. 
	SDLK_7,//  s3eKey7 ,				//!< Key 7. 
	SDLK_8,//  s3eKey8 ,				//!< Key 8. 
	SDLK_9,//  s3eKey9 ,				//!< Key 9. 
	SDLK_a,//3eKeyA ,				//!< Key A.
	SDLK_b,//  s3eKeyB ,				//!< Key B. 
	SDLK_c,//3eKeyC ,				//!< Key C. 
	SDLK_d,//  s3eKeyD ,				//!< Key D. 
	SDLK_e,//  s3eKeyE ,				//!< Key E. 
	SDLK_f,//  s3eKeyF ,				//!< Key F. 
	SDLK_g,//  s3eKeyG ,				//!< Key G. 
	SDLK_h,//  s3eKeyH ,				//!< Key H. 
	SDLK_i,//  s3eKeyI ,				//!< Key I. 
	SDLK_j,//  s3eKeyJ ,				//!< Key J. 
	SDLK_k,//  s3eKeyK ,				//!< Key K. 
	SDLK_l,//  s3eKeyL ,				//!< Key L. 
	SDLK_m,//  s3eKeyM ,				//!< Key M. 
	SDLK_n,//  s3eKeyN ,				//!< Key N. 
	SDLK_o,//  s3eKeyO ,				//!< Key O.
	SDLK_p,//  s3eKeyP ,				//!< Key P. 
	SDLK_q,//  s3eKeyQ ,				//!< Key Q. 
	SDLK_r,//  s3eKeyR ,				//!< Key R. 
	SDLK_s,//  s3eKeyS ,				//!< Key S. 
	SDLK_t,//  s3eKeyT ,				//!< Key T. 
	SDLK_u,//  s3eKeyU ,				//!< Key U. 
	SDLK_v,//  s3eKeyV ,				//!< Key V. 
	SDLK_w,//  s3eKeyW ,				//!< Key W.
	SDLK_x,//3eKeyX ,				//!< Key X. 
	SDLK_y,//  s3eKeyY ,				//!< Key Y. 
	SDLK_z,//  s3eKeyZ ,				//!< Key Z. 
	SDLK_F1,//  s3eKeyF1 ,				//!< Key F1. 
	SDLK_F2,//3eKeyF2 ,				//!< Key F2.
	SDLK_F3,//  s3eKeyF3 ,				//!< Key F3. 
	SDLK_F4,//  s3eKeyF4 ,				//!< Key F4.
	SDLK_F5,//  s3eKeyF5 ,				//!< Key F5. 
	SDLK_F6,//  s3eKeyF6 ,				//!< Key F6. 
	SDLK_F7,//  s3eKeyF7 ,				//!< Key F7. 
	SDLK_F8,//3eKeyF8 ,				//!< Key F8.
	SDLK_F9,//  s3eKeyF9 ,				//!< Key F9. 
	SDLK_F10,//  s3eKeyF10 ,				//!< Key F10. 
	SDLK_KP0,//3eKeyNumPad0 ,			//!< Key NumPad0.
	SDLK_KP1,//3eKeyNumPad1 ,			//!< Key NumPad1.
	SDLK_KP2,//3eKeyNumPad2 ,			//!< Key NumPad2.
	SDLK_KP3,//3eKeyNumPad3 ,			//!< Key NumPad3.
	SDLK_KP4,//3eKeyNumPad4 ,			//!< Key NumPad4.
	SDLK_KP5,//3eKeyNumPad5 ,			//!< Key NumPad5.
	SDLK_KP6,//3eKeyNumPad6 ,			//!< Key NumPad6.
	SDLK_KP7,//3eKeyNumPad7 ,			//!< Key NumPad7.
	SDLK_KP8,//3eKeyNumPad8 ,			//!< Key NumPad8.
	SDLK_KP9,//3eKeyNumPad9 ,			//!< Key NumPad9.
	SDLK_KP_PLUS,//3eKeyNumPadPlus ,		//!< Key NumPadPlus.
	SDLK_KP_MINUS,//3eKeyNumPadMinus ,		//!< Key NumPadMinus.
	SDLK_KP_ENTER,//3eKeyNumPadEnter ,		//!< Key NumPadEnter.

	//Not too sure about these...

	SDLK_RETURN,//3eKeyRSK ,				//!< Right Soft Key.
	SDLK_ESCAPE,//3eKeyLSK ,				//!< Left Soft Key.
	0,//3eKeyLS ,				//!< Left Shoulder button.
	0,//3eKeyRS ,				//!< Right shoulder button.
	SDLK_HASH,//3eKeyHash ,			//!< # Key.
	SDLK_ASTERISK,//3eKeyStar ,			//!< * Key.
	SDLK_SPACE,//3eKeyOk ,				//!< Select Key.
	SDLK_BACKSPACE,//3eKeyCLR ,				//!< CLR key.
	//   ,// Volume
	0,//3eKeyVolUp ,			//!< Volume Up Key.
	0,//3eKeyVolDown ,			//!< Volume Down Key.
	//  ,
	///,Misc.
	0,//3eKeyCamera ,			//!< Camera button.
	0,//3eKeyMic ,				//!< Microphone button.
	0,//3eKeyFn ,				//!< Fn button.
	0,//3eKeySym ,				//!< Sym button.
	////,Call  
	0,//3eKeyAccept ,			//!< call accept (talk).
	0,//3eKeyEnd ,				//!< call end (reject).
	SDLK_HOME,//3eKeyHomePage ,		//!< Home key.
	SDLK_WORLD_0,//s3eKeyButton1 ,			//<! Generic Button1.
	SDLK_WORLD_1,//3eKeyButton2 ,			//<! Generic Button2.
	SDLK_WORLD_2,//3eKeyButton3 ,			//<! Generic Button3.
	SDLK_WORLD_3,//3eKeyButton4 ,			//<! Generic Button4.
	SDLK_WORLD_4,//3eKeyButton5 ,			//<! Generic Button5.
	SDLK_WORLD_5,//3eKeyButton6 ,			//<! Generic Button6.
	SDLK_WORLD_6,//3eKeyButton7 ,			//<! Generic Button7.
	SDLK_WORLD_7,//3eKeyButton8 ,			//<! Generic Button8.
	SDLK_F11,//s3eKeyF11 ,				//!< Key F11. 
	SDLK_F12,//3eKeyF12 ,				//!< Key F12. 
	SDLK_LALT,//3eKeyAlt ,				//!< Alt key.
	SDLK_a,//s3eKeyAbsGameA = 200,	//<! Abstract Game keyA.
	SDLK_s,//3eKeyAbsGameB ,		//<! Abstract Game keyB.
	SDLK_d,//3eKeyAbsGameC ,		//<! Abstract Game keyC.
	SDLK_f,//3eKeyAbsGameD ,		//<! Abstract Game keyD.
	SDLK_UP,//3eKeyAbsUp	,			//<! Abstract Up.
	SDLK_DOWN,//3eKeyAbsDown ,			//<! Abstract Down.
	SDLK_LEFT,//3eKeyAbsLeft ,			//<! Abstract Left.
	SDLK_RIGHT,//3eKeyAbsRight ,		//<! Abstract Right.
	SDLK_SPACE,//3eKeyAbsOk ,			//<! Abstract Ok.
	SDLK_RETURN,//3eKeyAbsASK,			//<! Abstract action softkey.
	SDLK_ESCAPE,//s3eKeyAbsBSK,			//<! Abstract backwards softkey.
};

int _s3eKeyboardCallback(s3eKeyboardEvent* pInfo)
{
	SDL_keysym sym;
	sym.scancode = pInfo->m_Key;
	sym.mod = 0;
	sym.sym = s_s3eToSDLTranslation[pInfo->m_Key];
	sym.unicode = sym.sym;

	SDL_PrivateKeyboard((Uint8)(pInfo->m_Pressed ? SDL_PRESSED : SDL_RELEASED), &sym);
	return 1;
}

int _s3ePointerMotionCallback(s3ePointerMotionEvent* pInfo)
{
	Sint16 positionx = (Sint16)((pInfo->m_x * current_video->hidden->m_xFactor) >> 12);
	Sint16 positiony = (Sint16)((pInfo->m_y * current_video->hidden->m_yFactor) >> 12);

	SDL_PrivateMouseMotion(SDL_RELEASED, 0, positionx, positiony);
	current_video->hidden->m_xCursorPos = positionx;
	current_video->hidden->m_yCursorPos = positiony;
	return 0;
}

int _s3ePointerCallback(s3ePointerEvent* pInfo)
{
	Uint8 button = 0;
	Sint16 positionx;
	Sint16 positiony;

	if( !current_video )
		return 0;

	switch( pInfo->m_Button )
	{
		case S3E_POINTER_BUTTON_LEFTMOUSE:
			button = SDL_BUTTON_LEFT;
			break;
		case S3E_POINTER_BUTTON_RIGHTMOUSE:
			button = SDL_BUTTON_RIGHT;
			break;
		case S3E_POINTER_BUTTON_MIDDLEMOUSE:	
			button = SDL_BUTTON_MIDDLE;
			break;
		case S3E_POINTER_BUTTON_MOUSEWHEELUP:
			button = SDL_BUTTON_WHEELUP;
			break;
		case S3E_POINTER_BUTTON_MOUSEWHEELDOWN:
			button = SDL_BUTTON_WHEELDOWN;
			break;
		default:
			return 0;
	}

	positionx = (Sint16)((pInfo->m_x * current_video->hidden->m_xFactor) >> 12);
	positiony = (Sint16)((pInfo->m_y * current_video->hidden->m_yFactor) >> 12);
	
	SDL_PrivateMouseButton((Uint8)(pInfo->m_Pressed ? SDL_PRESSED : SDL_RELEASED), 
		button, 
		positionx, 
		positiony
		);

	return 1;
}

int QuitMessageSent = 0;

void S3E_PumpEvents(_THIS)
{
	s3eDeviceYield(0);
//	s3ePointerUpdate();

	if( s3eDeviceCheckPauseRequest() )
	{
		SDL_PrivateAppActive(0, SDL_APPACTIVE);
		SDL_PrivateAppActive(1, SDL_APPACTIVE);
	}

	if( s3eDeviceCheckQuitRequest() && !QuitMessageSent)
	{
		QuitMessageSent = 1;	
		SDL_PrivateQuit();
	}
}

void S3E_InitOSKeymap(_THIS)
{
	uint32 i, t;
	
	s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, (s3eCallback)_s3eKeyboardCallback, NULL);
	s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)_s3ePointerCallback, NULL);
	s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)_s3ePointerMotionCallback, NULL);

	for(i = 1; i < s3eKeyButton1; i++)
	{
		char key[S3E_KEYBOARD_DISP_LENGTH];
		char assign[255];
		s3eKeyboardGetDisplayName(key, (s3eKey)i, 1);
		if (!strncmp(key, "###", 3))
			continue;
		if (!strcmp(key, "#"))
			sprintf(key,"Hash");
		if (s3eConfigGetString("SDLKeys", key, assign) == S3E_RESULT_SUCCESS)
		{
			for(t = 0; t < SDLK_LAST; t++)
			{
				if( strcmp(assign, SDL_GetKeyName(t)) == 0 )
				{
					s_s3eToSDLTranslation[i] = t;
					break;
				}

			}
		}
	}
}

/* end of SDL_nullevents.c ... */

