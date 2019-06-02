/*****************************************************************************
*  8x8蜂鳴器.
*
*  created  by channel
*
*  todo list:
*
*  note:
****************************************************************************/

#include "Game8x8Tone.h"

//---------------------------------------------------------------------------
// 建構式.
//---------------------------------------------------------------------------
Game8x8Tone::Game8x8Tone() {
	pinMode( SP_PIN, OUTPUT);
}

//---------------------------------------------------------------------------
// 播放音效.
//---------------------------------------------------------------------------
void Game8x8Tone::playTone( unsigned int frequency, unsigned long duration = 0) {
	tone( SP_PIN, frequency, duration);
}