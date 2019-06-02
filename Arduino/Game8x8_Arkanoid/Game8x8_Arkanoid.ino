/*****************************************************************************
*  Game8x8-打磚塊遊戲.
*
* created  by channel
*
* todo list:
*
* note:
*
****************************************************************************/
#include "Game8x8Tone.h"
#include "Game8x8Time.h"
#include "Game8x8Graphics.h"

// 繪圖函數.
Game8x8Graphics game8x8Graphics = Game8x8Graphics();
// 蜂鳴器函數.
Game8x8Tone    game8x8Tone = Game8x8Tone();

// 繪圖更新時脈.
Game8x8Time TimeGraphicsUpdate = Game8x8Time();
// 移動版子更新時脈.
Game8x8Time TimeMovePaddle = Game8x8Time();
// 移動球更新時脈.
Game8x8Time TimeMoveBall = Game8x8Time();

// 磚塊.
byte brick[8][8] = {
{ 0,0,0,0,0,0,0,0 },
{ 0,1,1,1,1,1,1,0 },
{ 0,1,1,1,1,1,1,0 },
{ 0,1,1,1,1,1,1,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
};
// 磚塊數量.
byte brickMax = 18;

// 球初始位置.
int x = 3;
int y = 6;
// 移動像素.
int dx = 1;
int dy = 1;

// 版子.
int  paddleBegin = 2;
int  paddleEnd = 4;

// 遊戲狀態.
// 0:發球中.
// 1:遊戲中.
byte gameStatus = 0;

// 音效開關.
bool soundSwitch = true;

// 發球旗標.
// 0:初始狀態.
// 1:左.
// 2:中.
// 3:右.
// 4:發射.
byte fireBallFlag = 0;

// 可變電阻數值.
int sensorValue;

//---------------------------------------------------------------------------
// 初始.
//---------------------------------------------------------------------------
void setup()
{
	Serial.begin(9600);

	// 將PIN13腳位設定為GND.
	pinMode( 13, OUTPUT);
	digitalWrite( 13, 0);

	// 初始發球狀態.
	sensorValue = analogRead(A0);
	// 版子右移.
	if (sensorValue > 52) 
		fireBallFlag = 3;
	// 版子左移.
	else if (sensorValue < 48)
		fireBallFlag = 1;
	// 不移動.
	else
		fireBallFlag = 2;
	
}

//---------------------------------------------------------------------------
// 主迴圈.
//---------------------------------------------------------------------------
void loop()
{
	// *移動球更新時脈(60~180).
	if (TimeMoveBall.update(140) && gameStatus == 1) {
		// 判斷球碰磚塊.
		if (brick[y][x] == 1) {
			brick[y][x] = 2;
			brickMax--;
			// 播放音效.
			if (soundSwitch) {
				// 發出聲音.
				game8x8Tone.playTone(262, 33);
			}
			// 重新開始遊戲.
			if (brickMax == 0) {
				resetGame();
				return;
			}
			dy = -dy;      // 反彈
		}

		// 處理球移動.
		if (x + dx > 7 || x + dx < 0) {
			dx = -dx;
		}
		if (y + dy < 0) {
			dy = -dy;
		}
		else if (y + dy > 7) {
			// 判斷碰到底板反彈.
			if (x >= paddleBegin && x <= paddleEnd) {
				// 播放音效.
				if (soundSwitch) {
					// 發出聲音.
					game8x8Tone.playTone(494, 33);
				}
				// 碰左角.
				if (x == paddleBegin) {
					x--;
				}
				// 碰右角.
				else if (x == paddleEnd) {
					x++;
				}
				dy = -dy;
			}
			// 沒碰到底板.
			else
			{
				// 暫停遊戲.
				gameStatus = 0;

				// 球初始位置.
				x = paddleBegin + 1;
				y = 6;

				return;
			}
		}
		x += dx;
		y += dy;
	}

	// *移動版子更新時脈.
	if (TimeMovePaddle.update(80)) {
		// 可變電阻.
		sensorValue = analogRead(A0);
		sensorValue = map(sensorValue, 0, 765, 0, 100);
		//Serial.println(sensorValue, DEC);

		// 移動版子.
		// 版子右移.
		if (sensorValue > 52) {
			paddleBegin++; paddleEnd++;
			if (sensorValue > 62) {
				paddleBegin++; paddleEnd++;
			}
			if (paddleBegin >= 5) { paddleBegin = 5; }
			if (paddleEnd >= 7) { paddleEnd = 7; }
			// 移動旗標.
			if(fireBallFlag != 3)

			// 球移動中.
			if(gameStatus != 0)
				fireBallFlag = 3;
			// 開球中.
			else if (gameStatus == 0) {
				if (fireBallFlag != 3)
					fireBallFlag = 4;					
			}
		}
		// 版子左移.
		else if (sensorValue < 48)
		{
			paddleBegin--; paddleEnd--;
			if (sensorValue < 38) {
				paddleBegin--; paddleEnd--;
			}
			if (paddleBegin <= 0) { paddleBegin = 0; }
			if (paddleEnd <= 2) { paddleEnd = 2; }

			// 球移動中.
			if (gameStatus != 0)
				fireBallFlag = 1;
			// 開球中.
			else if (gameStatus == 0) {
				if (fireBallFlag != 1)
					fireBallFlag = 4;
			}
		}
		// 不移動.
		else 
		{
			// 球移動中.
			if (gameStatus != 0)
				fireBallFlag = 2;
			// 開球中.
			else if (gameStatus == 0) {
				if (fireBallFlag != 2)
					fireBallFlag = 4;
			}
		}
	}

	// *發球中.
	if (gameStatus == 0) {
		// 搖桿往上發球.
		// 發球旗標.
		// 0:代收訊息.
		// 1:左.
		// 2:中.
		// 3:右.
		// 4:發射.
		if (fireBallFlag == 4) {
			dx = -1;
			dy = -1;
			gameStatus = 1;
			fireBallFlag = 0;
		}
		else 
		{
			// 讓球跟著版子跑.
			x = paddleBegin + 1;
		}
	}

	//--------------------------------------------------------------------
	// 繪圖更新時脈-FPS 30.
	if (TimeGraphicsUpdate.update(33)) {

		// 清除畫面.
		game8x8Graphics.clearDisplay(0);

		//--------------------------------------------------------------------
		// 畫磚塊.
		for (int y = 0; y<8; y++) {
			for (int x = 0; x<8; x++) {
				if (brick[y][x] == 1) {
					game8x8Graphics.setPixcls(x, y, 1);
				}
				else {
					game8x8Graphics.setPixcls(x, y, 0);
				}
			}
		}

		//--------------------------------------------------------------------
		// 移動球.
		game8x8Graphics.setPixcls(x, y, 1);

		//--------------------------------------------------------------------
		// 畫版子.
		for (byte i = paddleBegin; i <= paddleEnd; i++) {
			game8x8Graphics.setPixcls(i, 7, 1);
		}

		// 更新.
		game8x8Graphics.update();
	}
}

//---------------------------------------------------------------------------
// 重新開始遊戲.
//---------------------------------------------------------------------------
void resetGame()
{
	// 暫停遊戲.
	gameStatus = 0;

	// 球初始位置.
	x = paddleBegin + 1;
	y = 6;

	// 初始磚塊.
	for (int y = 0; y<8; y++) {
		for (int x = 0; x<8; x++) {
			if (brick[y][x] == 2) {
				brick[y][x] = 1;
			}
		}
	}
	// 磚塊數量.
	brickMax = 18;
}
