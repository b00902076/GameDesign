/*==============================================================
This is a demo program for Fly2 System

Hello !

(C)2004-2012 Chuan-Chang Wang, All Rights Reserved
Created : 0303, 2004, C. Wang

Last Updated : 0904, 2012, C.Wang
===============================================================*/
#include "FlyWin32.h"
#include "FyMedia.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

OBJECTid bgID, selectID;

SCENEid sID2;                // the 2D scene
OBJECTid spID0 = FAILED_ID;  // the sprite
OBJECTid spID1 = FAILED_ID;  // the sprite
OBJECTid slotMachine = FAILED_ID;  // the sprite
OBJECTid status = FAILED_ID;  // the sprite
OBJECTid timer[8];  // the sprite
OBJECTid startPanel[8];  // the sprite
OBJECTid endID;
OBJECTid bound;
FnCharacter *LvbuInUI;
OBJECTid Ending_bg, Ending_select, BadEnding_bg;  // the sprite

int item = 0, item_eff = 11, status_frame = 0, status_timer = 0, start_timer = 0, end_timer = 0;
int next_item = 0;
float lastSPD, lastATK;
char statusFilename[4][64] = { "null.png", "spdUP_", "atkUP_", "muteki.png" };
bool titleFlag = FALSE;
bool startFlag = FALSE;
bool endFlag = FALSE;
bool failFlag = FALSE;
bool restartFlag = FALSE;

OBJECTid sPID[2];
int Aai_x[8], Aai_y[8], Aai_z[8];
int Aai_xx[2], Aai_yy[2], Aai_zz[2];

MEDIAid mmID, getItemID, titlemID, endingmID, deadmID, clearmID, bgmmID;
int endingcount = 0;
clock_t start, finish;

void UIAI(int);
void init_UI();
void show_timer();
void randomSlot(int next_item);
void useItem();
void updateStatus();
void gameStart();
void gameEnd();
void gameDead();
void gameClear();
void titleSelect();
void setDifferentImage(BYTE, BOOL4);

void UIAI(int skip)
{
	if (endFlag){
		gameClear();
	}
	if (failFlag){
		gameDead();
	}
	if (game_end_flag){
		gameEnd();
		return;
	}

	if (titleFlag){
		FnMedia titleM;
		titleM.Object(titlemID);
		if (!titleM.IsPlaying())
			titleM.Stop();
		titleM.SetVolume(0.9);
		titleM.Play(ONCE);
		return;
	}

	if (startFlag){
		gameStart();
		return;
	}

	FnMedia BGM;
	BGM.Object(bgmmID);
	if (!BGM.IsPlaying())
		BGM.Stop();
	BGM.SetVolume(0.9);
	BGM.Play(ONCE);

	char filename[64];
	int speedChr;
	FnSprite sp;

	int power = LvbuInUI->power;
	if (power < 0)
		power = 0;
	sp.ID(spID0);
	speedChr = power % 10;
	sprintf(filename, "%d.png", speedChr);
	sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);

	sp.ID(spID1);
	speedChr = power / 10;
	if (speedChr > 0){
		sprintf(filename, "%d.png", speedChr);
		sp.SetSize(50, 100);
		sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	}
	else{
		sp.SetSize(0, 0);
	}

	sp.ID(slotMachine);
	FnMedia mP;
	mP.Object(mmID);

	if (LvbuInUI->mono == NULL || LvbuInUI->mono->span <= 0)
		useItem();

	if (item < 0){
		speedChr = rand() % 4 + 1;
		sprintf(filename, "item%d.png", speedChr);
		sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		if (!mP.IsPlaying()){
			item = -item;
			sprintf(filename, "item%d.png", item);
			sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
			item_eff = 0;
			mP.Object(getItemID);
			mP.Stop();
			mP.Play(ONCE);
		}
	}

	if (item_eff <= 5){
		sp.SetSize(250 - 4 * item_eff*item_eff, 250 - 4 * item_eff*item_eff);
		sp.SetOpacity(item_eff*0.2 - 0.000001);
		item_eff++;
	}

	updateStatus();
	show_timer();

	FySwapBuffers();
}

void init_UI(FnCharacter *init_Lvbu)
{
	item = 0; item_eff = 11; status_frame = 0; status_timer = 0; start_timer = 0; end_timer = 0;
	next_item = 0; endingcount = 0;

	LvbuInUI = init_Lvbu;
	lastSPD = LvbuInUI->movementspeed;
	lastATK = LvbuInUI->dmgmultiplier;
	FySetTexturePath("Data\\textures");
	FyBeginMedia("Data\\media", 2);

	FnScene scene;
	FnSprite sp;

	// create a 2D scene for sprite rendering which will be rendered on the top of 3D
	sID2 = FyCreateScene(1);
	scene.ID(sID2);
	scene.SetSpriteWorldSize(800, 600);         // 2D scene size in pixels
	
	if (LvbuInUI->power > 0){
		titleFlag = TRUE;
		bgID = scene.CreateObject(SPRITE);
		sp.ID(bgID);
		sp.SetSize(800, 600);
		sp.SetImage("bg.jpg", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(0, 0, 5);

		selectID = scene.CreateObject(SPRITE);
		sp.ID(selectID);
		sp.SetSize(800, 50);
		sp.SetImage("title_start.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(0, 275, 6);
	}

	startPanel[0] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[0]);
	sp.SetSize(800, 46);
	sp.SetImage("keepout0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(-800, 120, 1);

	startPanel[1] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[1]);
	sp.SetSize(800, 46);
	sp.SetImage("keepout0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(-1600, 120, 1);

	startPanel[2] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[2]);
	sp.SetSize(800, 46);
	sp.SetImage("keepout1.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(-800, 445, 1);

	startPanel[3] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[3]);
	sp.SetSize(800, 46);
	sp.SetImage("keepout1.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(-1600, 445, 1);

	startPanel[4] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[4]);
	sp.SetSize(800, 250);
	sp.SetImage("keepout2.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(800, 180, 1);

	startPanel[5] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[5]);
	sp.SetSize(800, 250);
	sp.SetImage("keepout2.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(1600, 180, 1);


	startPanel[6] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[6]);
	sp.SetSize(800, 600);
	sp.SetImage("red.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(0, 0, 0);
	sp.SetOpacity(0);


	startPanel[7] = scene.CreateObject(SPRITE);
	sp.ID(startPanel[7]);
	sp.SetSize(200, 200);
	sp.SetImage("null.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(300, 200, 2);

	spID0 = scene.CreateObject(SPRITE);
	sp.ID(spID0);
	sp.SetSize(50, 100);
	sp.SetImage("5.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(70, 500, 0);

	spID1 = scene.CreateObject(SPRITE);
	sp.ID(spID1);
	sp.SetSize(50, 100);
	sp.SetImage("1.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(20, 500, 0);

	slotMachine = scene.CreateObject(SPRITE);
	sp.ID(slotMachine);
	sp.SetSize(150, 150);
	sp.SetImage("item0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(30, 30, 0);


	status = scene.CreateObject(SPRITE);
	sp.ID(status);
	sp.SetSize(150, 150);
	sp.SetImage("null.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(30, 180, 0);

	bound = scene.CreateObject(SPRITE);
	sp.ID(bound);
	sp.SetSize(0, 0);

	timer[4] = scene.CreateObject(SPRITE);
	sp.ID(timer[4]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(645, 500, 0);

	timer[3] = scene.CreateObject(SPRITE);
	sp.ID(timer[3]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(605, 500, 0);

	timer[1] = scene.CreateObject(SPRITE);
	sp.ID(timer[1]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(540, 500, 0);

	timer[0] = scene.CreateObject(SPRITE);
	sp.ID(timer[0]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(500, 500, 0);

	timer[2] = scene.CreateObject(SPRITE);
	sp.ID(timer[2]);
	sp.SetSize(25, 100);
	sp.SetImage("time_m.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(580, 500, 0);

	timer[7] = scene.CreateObject(SPRITE);
	sp.ID(timer[7]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(750, 500, 0);

	timer[6] = scene.CreateObject(SPRITE);
	sp.ID(timer[6]);
	sp.SetSize(40, 100);
	sp.SetImage("time_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(710, 500, 0);

	timer[5] = scene.CreateObject(SPRITE);
	sp.ID(timer[5]);
	sp.SetSize(25, 100);
	sp.SetImage("time_m.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(685, 500, 0);

	Ending_bg = scene.CreateObject(SPRITE);
	sp.ID(Ending_bg);
	sp.SetSize(800, 200);
	sp.SetImage("null.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(0, 200, 10);

	BadEnding_bg = scene.CreateObject(SPRITE);
	sp.ID(BadEnding_bg);
	sp.SetSize(800, 200);
	sp.SetImage("dead.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(0, 200, 10);
	sp.SetOpacity(0);

	Ending_select = scene.CreateObject(SPRITE);
	sp.ID(Ending_select);
	sp.SetSize(0, 0);
	sp.SetImage("title_restart.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	sp.SetPosition(0, 180, 11);

	endID = scene.CreateObject(SPRITE);

	sPID[0] = spID0; sPID[1] = spID1;
	for (int i = 0; i < 8; i++) {
		sp.ID(timer[i]);
		sp.GetPosition(&Aai_x[i], &Aai_y[i], &Aai_z[i]);
	}
	for (int i = 0; i < 2; i++) {
		sp.ID(sPID[i]);
		sp.GetPosition(&Aai_xx[i], &Aai_yy[i], &Aai_zz[i]);
	}


	if (game_end_flag){
		FnMedia mP;
		mP.Object(deadmID);
		mP.Stop();
		mP.Object(clearmID);
		mP.Stop();
	}

	FyBindTimer(6, 30.0f, UIAI, TRUE);
	mmID = FyCreateMediaPlayer("slot_short.mp3", 0, 0, 0, 0);
	getItemID = FyCreateMediaPlayer("ab_ancilla.wav", 0, 0, 0, 0);
	titlemID = FyCreateMediaPlayer("menu.mp3", 0, 0, 0, 0);
	deadmID = FyCreateMediaPlayer("samurai_shouting2.mp3", 0, 0, 0, 0);
	clearmID = FyCreateMediaPlayer("clear.mp3", 0, 0, 0, 0);
	bgmmID = FyCreateMediaPlayer("bgm.mp3", 0, 0, 0, 0);



	//FyDefineHotKey(FY_Z, setDifferentImage, FALSE);
	//FyDefineHotKey(FY_X, setDifferentImage, FALSE);
	//FyDefineHotKey(FY_RETURN, setDifferentImage, FALSE);

	startFlag = TRUE;
	endFlag = FALSE;
	failFlag = FALSE;
	restartFlag = FALSE;
}

void gameStart()
{
	start_timer++;

	FnSprite sp;
	FnMedia mP;
	int x, y, z;

	sp.ID(startPanel[6]);
	sp.SetOpacity(abs(sin(start_timer*0.08)));

	for (int i = 0; i < 4; i++){
		sp.ID(startPanel[i]);
		sp.GetPosition(&x, &y, &z);
		if (x >= 800)
			x = -800;
		sp.SetPosition(x + 50, y, z);
	}
	for (int i = 4; i < 6; i++){
		sp.ID(startPanel[i]);
		sp.GetPosition(&x, &y, &z);
		if (x <= -800)
			x = 800;
		sp.SetPosition(x - 50, y, z);
	}

	if (start_timer == 20){
		mP.Object(FyCreateMediaPlayer("plasma_damage.wav", 0, 0, 0, 0));
		mP.Play(ONCE);
		sp.ID(startPanel[7]);
		sp.SetImage("start_3.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	}
	if (start_timer == 50){
		mP.Object(FyCreateMediaPlayer("plasma_damage.wav", 0, 0, 0, 0));
		mP.Play(ONCE);
		sp.ID(startPanel[7]);
		sp.SetImage("start_2.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	}
	if (start_timer == 80){
		mP.Object(FyCreateMediaPlayer("plasma_damage.wav", 0, 0, 0, 0));
		mP.Play(ONCE);
		sp.ID(startPanel[7]);
		sp.SetImage("start_1.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	}
	if (start_timer == 110){
		mP.Object(FyCreateMediaPlayer("plasma_die.wav", 0, 0, 0, 0));
		mP.Play(ONCE);
		sp.ID(startPanel[7]);
		sp.SetSize(800, 200);
		sp.SetImage("start_0.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(0, 200, 2);
	}

	if (start_timer < 110 && start_timer > 20){
		sp.ID(startPanel[7]);
		sp.SetPosition(300 + rand() % 15, 200 + rand() % 15, 2);
	}
	if (start_timer > 110){
		sp.ID(startPanel[7]);
		sp.SetPosition(rand() % 15, 200 + rand() % 15, 2);
	}
	if (start_timer > 140){
		for (int i = 0; i < 8; i++){
			sp.ID(startPanel[i]);
			sp.SetOpacity(0.999 - ((start_timer - 140)*0.1));
		}
	}
	if (start_timer >= 150){
		startFlag = FALSE;
		sp.ID(bound);
		sp.SetSize(180, 180);
		sp.SetImage("bound.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(620, 0, 0);
		start = clock();
	}
}

void gameDead(){
	FnSprite sp;
	endingcount++;

	sp.ID(BadEnding_bg);
	sp.SetOpacity(min(endingcount*0.015, 0.9999));
}

void gameClear(){
	FnSprite sp;
	char filename[64];

	sp.ID(Ending_bg);
	if (endingcount < 0)
		sprintf(filename, "clear_0.png");
	else
		sprintf(filename, "clear_%d.png", endingcount);
	sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);

	if (endingcount >= 0)
		endingcount++;
	if (endingcount > 32)
		endingcount = -1;
	if (endingcount < 0)
		endingcount -= rand() % 2 + 1;
	if (endingcount < -30)
		endingcount = 0;
}

void gameEnd() {
	end_timer++;
	FnSprite sp;

	if (end_timer == 1) {
		sp.ID(slotMachine);
		sp.SetSize(0, 0);
		sp.ID(bound);
		sp.SetSize(0, 0);
		FnMedia BGM;
		BGM.Object(bgmmID);
		BGM.Stop();
		if (endFlag)
			BGM.Object(clearmID);
		else
			BGM.Object(deadmID);
		BGM.Play(ONCE);
	}

	if (120 < end_timer&&end_timer <= 210) {
		for (int i = 0; i < 8; i++) {
			sp.ID(timer[i]);
			sp.SetPosition((int)((float)Aai_x[i] - 245.0 / 90 * (end_timer - 120)), (int)((float)Aai_y[i] - 190.0 / 90 * (float)(end_timer - 120)), Aai_z[i]);
		}
		/*for (int i = 0; i < 2; i++) {
			sp.ID(sPID[i]);
			sp.SetPosition((int)((float)Aai_xx[i] - 300.0 / 90 * (float)(end_timer - 120)), (int)((float)Aai_yy[i] + 140.0 / 90 * (float)(end_timer - 120)), Aai_zz[i]);
		}*/
	}
	if (end_timer == 119){
		sp.ID(Ending_bg);
		sp.SetSize(0, 0);
		sp.ID(BadEnding_bg);
		sp.SetSize(0, 0);
		endFlag = FALSE;
		failFlag = FALSE;
	}
	if (end_timer == 210){
		sp.ID(Ending_select);
		sp.SetSize(800, 50);
		sp.SetImage("title_restart.png", 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(0, 120, 11);
		restartFlag = TRUE;
	}
}

void show_timer()
{
	finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	int minute = int(duration) / 60;
	int sec = int(duration) % 60;
	int minisec = int((duration - int(duration)) * 100);
	int timer_array[6] = { minute / 10, minute % 10, sec / 10, sec % 10, minisec / 10, minisec % 10 };
	for (int i = 0, j = 0; i < 8; i++){
		if (i == 2 || i == 5)
			continue;
		FnSprite sp;
		char filename[64];
		sp.ID(timer[i]);
		sprintf(filename, "time_%d.png", timer_array[j]);
		sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		j++;
	}
}

void randomSlot(int next_item)
{
	FnMedia mP;
	mP.Object(mmID);
	mP.Stop();
	mP.Play(ONCE);
	item = -next_item;
}

void useItem()
{
	char filename[64];
	FnSprite sp;

	sp.ID(slotMachine);
	sprintf(filename, "item0.png");
	sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	
	sp.ID(status);
	sp.SetPosition(30, 180, 0);

	item = 0;

}

void updateStatus()
{
	if (item < 0) return;

	char filename[64];
	FnSprite sp;
	sp.ID(status);
	if (item == 3){
		sp.SetImage(statusFilename[3], 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		sp.SetPosition(20 + rand() % 20, 170 + rand() % 20, 0);
		return;
	}

	status_timer++;
	if (status_timer % 6 != 0)
		return;

	if (item == 0 || item == 4){
		sp.SetImage(statusFilename[0], 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
		return;
	}

	if (status_frame < 3)
		sprintf(filename, "%s%d.png", statusFilename[item], status_frame);
	else
		sprintf(filename, "%s%d.png", statusFilename[item], 3 + (status_frame - 3) % 4);

	sp.SetImage(filename, 0, NULL, 0, NULL, NULL, MANAGED_MEMORY, FALSE, FALSE);
	status_frame++;
	if (status_frame > 10000000)
		status_frame = 3 + (status_frame - 3) % 4;
}

void setDifferentImage(BYTE code, BOOL4 value)
{
	if (code == FY_X && value) {
		if (item == 0)
			randomSlot(1);
	}
	if (code == FY_Z && value) {
		if (item > 0)
			useItem();
	}
	if (code == FY_RETURN && value) {
		titleFlag = FALSE;
		FnMedia titleM;
		titleM.Object(titlemID);
		titleM.Stop();

		FnSprite sp;
		sp.ID(bgID);
		sp.SetSize(0, 0);
		sp.ID(selectID);
		sp.SetSize(0, 0);
	}

}

class MyFX
{
protected:
	GAMEFX_SYSTEMid gFXID = FAILED_ID;
	char gFXfile[128];
	MEDIAid mFXID;
	FnCharacter *dummy;
	bool flag, gF, mF;

public:
	MyFX(){
		flag = FALSE;
	};

	void init(char *init_gFXfile, char *mFXfile, FnCharacter *init_dummy){
		gF = TRUE;
		mF = TRUE;
		if (init_gFXfile == NULL)
			gF = FALSE;
		if (mFXfile == NULL)
			mF = FALSE;
		sprintf(gFXfile, "%s", init_gFXfile);
		mFXID = FyCreateMediaPlayer(mFXfile, 0, 0, 0, 0);
		dummy = init_dummy;
	}

	void Start(SCENEid sID){
		flag = TRUE;
		if (mF){
			FnMedia mP;
			mP.Object(mFXID);
			mP.Stop();
			mP.Play(ONCE);
		}
		if (gF){
			FnScene scene(sID);
			if (gFXID != FAILED_ID){
				scene.DeleteGameFXSystem(gFXID);
			}
			gFXID = scene.CreateGameFXSystem();
			FnGameFXSystem gxS(gFXID);
			BOOL4 beOK = gxS.Load(gFXfile, TRUE);

			float xyz[3];
			dummy->GetPosition(xyz);
			gxS.SetPlayLocation(xyz);
		}
	}

	void Start(SCENEid sID, float *xyz){
		flag = TRUE;
		if (mF){
			FnMedia mP;
			mP.Object(mFXID);
			mP.Stop();
			mP.Play(ONCE);
		}
		if (gF){
			FnScene scene(sID);
			if (gFXID != FAILED_ID){
				scene.DeleteGameFXSystem(gFXID);
			}
			gFXID = scene.CreateGameFXSystem();
			FnGameFXSystem gxS(gFXID);
			BOOL4 beOK = gxS.Load(gFXfile, TRUE);

			gxS.SetPlayLocation(xyz);
		}
	}

	void Play(int skip, SCENEid sID){
		if (!flag || !gF)
			return;
		FnGameFXSystem gxS(gFXID);
		BOOL4 beOK = gxS.Play((float)skip, ONCE);
		if (!beOK){
			FnScene scene(sID);
			scene.DeleteGameFXSystem(gFXID);
			flag = FALSE;
			gFXID = FAILED_ID;
		}
	}

	bool isID(FnCharacter *a){
		return (dummy == a);
	}

	char *test(){
		if (gFXID == FAILED_ID)
			return "FAILED_ID";
		else
			return "HAVE_ID";
	}
};