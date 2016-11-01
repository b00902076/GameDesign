/*==============================================================
character movement testing using Fly2

- Load a scene
- Generate a terrain object
- Load a character
- Control a character to move
- Change poses

(C)2012-2015 Chuan-Chang Wang, All Rights Reserved
Created : 0802, 2012

Last Updated : 1004, 2015, Kevin C. Wang
===============================================================*/
#include "FlyWin32.h"
int game_end_flag = 0;
#include "myui.h"
#include <list>
#define TEAMAX 21
FnObject teapot[TEAMAX];
const float teapotpos[TEAMAX][2] = {{4369.0f, -4708.0f}, {-1397, -3490}, {-2845, -208}, {-1580, 242}, {-1624, 1423}, {-1938, 2548}, {3455.0f, -2726.0f}, {957, -2933}, {1352, -2201}, {215, -1856}, {-244, -2897}, {2548, -2205}, {1202, -2232}, {-889, -2272}, {-1801, -3236}, {-2868, -1136}, {-1598, 1842}, {441, 2451}, {-1557, -242}, {-2228, 735}, {-1117, 2576}};
VIEWPORTid vID;						// the major viewport
SCENEid sID;						// the 3D scene
OBJECTid cID, tID, dID;			// the main camera and the terrain for terrain following
ROOMid terrainRoomID = FAILED_ID;
TEXTid textID = FAILED_ID;
FnObject terrain, dummy;
FnCamera camera;
FnCharacter Lvbu;
FnViewport vp, vp2;
FnScene scene;
std::list<Itr> injuryQueue;
std::list<FnCharacter*> targetpool;
std::list<FnCharacter*> totalcharacterpool;
std::list<FnObject*> teapool;
std::list<FnObject> potpool;
std::list<FnCharacter*> butsukaripool;
#define DONZOMAX 5
FnCharacter Donzo[DONZOMAX];
#define ZAKOMAX 10
FnCharacter zako[ZAKOMAX];
int zako_timer = 0;
float zako_start[3] = { 3295.0f, -3922.0f, 60.0f };
float zako_goal[3] = { 3808.0f, -3509.0f, 60.0f };
FnCamera camera2;
OBJECTid cID2;

#define BOSSMONSTERMAX 4
#define BOSSMONSTERDIS 150.0f
FnCharacter boss, boss_bullet;
FnCharacter boss_monster[BOSSMONSTERMAX];
float boss_monster_dir[4][2] = { { BOSSMONSTERDIS, 0.0f }, { 0.0f, BOSSMONSTERDIS }, { -BOSSMONSTERDIS, 0.0f }, { 0.0f, -BOSSMONSTERDIS } };
int boss_monster_target[4] = { 0, 1, 2, 3 };


#define BEARMAX 16
FnCharacter bear[BEARMAX];
float bear_start_pos[7][3] = { { 891.0f, -2673.0f, 60.0f }, { -1930.0f, -242.0f, 60.0f }, { -674.0f, -2585.0f, 60.0f }, { -2773.0f, -2364.0f, 60.0f }, { 81.0f, -2257.0f, 60.0f }, { -1785.0f, -2883.0f, 60.0f }, {-1952.0f ,1343.0f , 60.0f } };

#define MANMAX 6
FnCharacter man[MANMAX];
float man_start_pos[2][3] = { { -800.0f, 2301.0f, 60.0f }, { -1930.0f, -242.0f, 60.0f } };


float total_attack_time_span[100] = { 0 };

void bossAI(int skip);
void bearAI(int skip);
void manAI(int skip);
void donzoAI(int skip);

void check_die(FnCharacter& actor, int skip);
void check_stop(FnCharacter& actor, int skip);
int check_range(FnCharacter&);
void collision_back(FnCharacter&);
float global_fDir[3], global_time = 0.0;
void npc_face(FnCharacter&, FnCharacter&);
void move_toward(FnCharacter&, FnCharacter&, int);
void npc_attack(FnCharacter& actor1, FnCharacter& actor2, int num_attack_type, ACTIONid attack_pose[10], float attack_dis[10], float attackspan_list[10], int who, int skip);

int rotate_time = 0;
float goal_pos[3] = { 512.0f, 3283.0f, 30.0f };
void checkendgame();
void game_Restart();

void initKEY();
#define NUMMAX (DONZOMAX + ZAKOMAX + BEARMAX+MANMAX)
MyFX targetFX[NUMMAX];
MyFX LyubuFX;
MyFX LyubudFX;
MyFX BoomFX;
MyFX BearFX;
FnObject goal;
FnCharacter dummygoal;
MyFX WolfFX;
MyFX BossFX;
MyFX dummyFX;

void CreateZako(int);
void PutZako(int);
void MoveZako(void);
void ZakoMoveTest(FnCharacter*);
void RemoveZako(void);
void ZakoAI(int);
void Gravity(int);
void TeaPotTimer(int);
void ItrTimer(int);
// some globals
int frame = 0;
int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;
float actorPosition[3] = { 0.0f, 0.0f, 0.0f };
float cameraPosition[3] = { 0.0f, 0.0f, 0.0f };

// hotkey callbacks
void QuitGame(BYTE, BOOL4);
void Movement(BYTE, BOOL4);
void Attack(BYTE, BOOL4);
void Utility(BYTE, BOOL4);

// timer callbacks
void GameAI(int);
void RenderIt(int);

// basic
float rotatespeed = 5.5f;
float cameraZoom = 480.0f;
float cameraHeight = 60.0f;
float cameraAngle = 0.0f;
float headHeight = 60.0f;
int attackspan = 0;
int throwspan = 0;

// key status
BOOL4 KEY_UP = false;
BOOL4 KEY_DOWN = false;
BOOL4 KEY_LEFT = false;
BOOL4 KEY_RIGHT = false;
BOOL4 KEY_W = false;
BOOL4 KEY_A = false;
BOOL4 KEY_S = false;
BOOL4 KEY_D = false;
BOOL4 KEY_Q = false;
BOOL4 KEY_E = false;
BOOL4 KEY_NUM8 = false;
BOOL4 KEY_NUM2 = false;
BOOL4 KEY_NUM4 = false;
BOOL4 KEY_NUM6 = false;
BOOL4 KEY_NUMADD = false;
BOOL4 KEY_NUMMINUS = false;
BOOL4 KEY_R = false;
/*------------------
the main program
C.Wang 1010, 2014
-------------------*/
void FyMain(int argc, char **argv) {
	// create a new world
	BOOL4 beOK = FyStartFlyWin32("Team16", 300, 100, 1024, 768, FALSE);

	// setup the data searching paths
	FySetShaderPath("Data\\NTU6\\Shaders");
	FySetModelPath("Data\\NTU6\\Scenes");
	FySetTexturePath("Data\\NTU6\\Scenes\\Textures");
	FySetScenePath("Data\\NTU6\\Scenes");
	FyBeginMedia("data\\media", 2);
	FySetGameFXPath("Data\\NTU6\\FX0");

	// create a viewport
	vID = FyCreateViewport(0, 0, 1024, 768);
	VIEWPORTid vID2 = FyCreateViewport(804, 548, 200, 200);
	vp.ID(vID);
	vp2.ID(vID2);

	// create a 3D scene
	sID = FyCreateScene(10);
	scene.ID(sID);

	// load the scene
	scene.Load("gameScene02");
	scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);

	// create a dummy
	dID = scene.CreateObject(OBJECT);
	dummy.ID(dID);
	// load the terrain
	tID = scene.CreateObject(OBJECT);
	terrain.ID(tID);
	BOOL beOK1 = terrain.Load("terrain");
	terrain.Show(FALSE);

	// set terrain environment
	terrainRoomID = scene.CreateRoom(SIMPLE_ROOM, 10);
	FnRoom room;
	room.ID(terrainRoomID);
	room.AddObject(tID);

	// load the character
	FySetModelPath("Data\\NTU6\\Characters");
	FySetTexturePath("Data\\NTU6\\Characters");
	FySetCharacterPath("Data\\NTU6\\Characters");
	CHARACTERid LvbuID = scene.LoadCharacter("Lyubu2");
	//float pos[3] = { 3569.0f, -3908.0f, 60.0f }; //start point
	//float pos[3] = { 712.0f, 3083.0f, 60.0f }; //goal point
	float pos[3] = {3569.0f, -2808.0f, 60.0f};
	//float fDir[3] = { 1.0f, 1.0f, 0.0f };
	float fDir[3] = { -1.0f, 0.0f, 0.0f };
	float uDir[3] = { 0.0f, 0.0f, 1.0f };
	Lvbu.ID(LvbuID);
	Lvbu.SetDirection(fDir, uDir);
	Lvbu.SetTerrainRoom(terrainRoomID, 10.0f);
	beOK = Lvbu.PutOnTerrain(pos);
	Lvbu.loadActions(0);
	Lvbu.butsukari_range = 50;
	Lvbu.curPoseID = Lvbu.idleID;
	Lvbu.SetCurrentAction(NULL, 0, Lvbu.curPoseID);
	Lvbu.Play(START, 0.0f, FALSE, TRUE);
	LyubuFX.init(NULL, "swordslash5.wav", &Lvbu);
	LyubudFX.init("Blow_01e", "_acolyte_hit.wav", &Lvbu);
	BearFX.init(NULL, "bigfoot_attack.wav", NULL);
	WolfFX.init(NULL, "were_wolf_attack.wav", NULL);
	BossFX.init(NULL, "beelzebub__attack.wav", NULL);
	memcpy(Lvbu.sPos, pos, sizeof(pos));
	memcpy(Lvbu.sFDir, fDir, sizeof(fDir));
	memcpy(Lvbu.sUDir, uDir, sizeof(uDir));
	totalcharacterpool.push_back(&Lvbu);

	float posDZ[3] = { 3569.0f, -3908.0f, 60.0f };

	for (int i = 0; i < DONZOMAX; i++) {
		CHARACTERid DonzoID = scene.LoadCharacter("Donzo2");
		// put the character on terrain
		posDZ[1] += 100.0f;
		Donzo[i].ID(DonzoID);
		Donzo[i].SetDirection(fDir, uDir);
		Donzo[i].SetTerrainRoom(terrainRoomID, 10.0f);
		beOK = Donzo[i].PutOnTerrain(posDZ);
		Donzo[i].loadActions(1);
		Donzo[i].butsukari_range = 30;
		Donzo[i].butsukari_range_NPC = 300;
		Donzo[i].butsukari_flag = 1;

		// set the character to idle action
		Donzo[i].SetCurrentAction(NULL, 0, Donzo[i].curPoseID = Donzo[i].idleID);
		Donzo[i].Play(START, 0.0f, FALSE, TRUE);
		targetpool.push_back(&Donzo[i]);
		butsukaripool.push_back(&Donzo[i]);

		targetFX[i].init("Blow_01e", "_enemy_hit_fire2.wav", &Donzo[i]);
	}
	FySetModelPath("Data\\Models");
	FySetTexturePath("Data\\Models");
	for (int i = 0; i < TEAMAX; i++) {
		OBJECTid teapotid = scene.CreateObject(OBJECT);
		teapot[i].position[0] = teapotpos[i][0];
		teapot[i].position[1] = teapotpos[i][1];
		teapot[i].position[2] = 60.0f + 10.0f * (float)sin(teapot[i].currangle);
		teapot[i].ID(teapotid);
		teapot[i].Load("box");
		teapot[i].currangle = (double)(rand() % 180);
		teapot[i].alive = true;
		teapot[i].SetPosition(teapot[i].position);
		teapool.push_back(&teapot[i]);
	}
	BoomFX.init("Tower_atk01_e", "teapotBreak.wav", NULL);

	FySetModelPath("Data\\NTU6\\NPCs");
	FySetTexturePath("Data\\NTU6\\NPCs");
	FySetCharacterPath("Data\\NTU6\\NPCs");
	fDir[0] = zako_goal[0] - zako_start[0];
	fDir[1] = zako_goal[1] - zako_start[1];
	pos[0] = pos[1] = 0.0f;	pos[2] = -500.0f;
	for (int i = 0; i < ZAKOMAX; i++) {
		CHARACTERid zakoID = scene.LoadCharacter("AMA001");
		zako[i].ID(zakoID);
		zako[i].loadActions(2);
		// set the character to idle action
		zako[i].SetCurrentAction(NULL, 0, zako[i].curPoseID = zako[i].zako_idleID);
		zako[i].Play(START, 0.0f, FALSE, TRUE);
		// put the character on terrain
		zako[i].SetDirection(fDir, uDir);
		zako[i].SetTerrainRoom(terrainRoomID, 10.0f);
		zako[i].SetPosition(pos);
		zako[i].butsukari_range = 25.0f;
		zako[i].butsukari_flag = 1;

		targetpool.push_back(&zako[i]);
		butsukaripool.push_back(&zako[i]);

		targetFX[i + DONZOMAX].init("Blow_01e", "_enemy_hit_fire2.wav", &zako[i]);

		memcpy(zako[i].sPos, pos, sizeof(pos));
		memcpy(zako[i].sFDir, fDir, sizeof(fDir));
		memcpy(zako[i].sUDir, uDir, sizeof(uDir));
		totalcharacterpool.push_back(&zako[i]);
	}

	FySetModelPath("Data\\NTU6\\NPCs");
	FySetTexturePath("Data\\NTU6\\NPCs");
	FySetCharacterPath("Data\\NTU6\\NPCs");
	for (int i = 0; i < BEARMAX; i++){
		CHARACTERid bearID = scene.LoadCharacter("AMA001");
		if (i < 2) {
			bear[i].sPos[0] = bear_start_pos[0][0] + (float)(rand() % 250) - 125.0f;
			bear[i].sPos[1] = bear_start_pos[0][1] + (float)(rand() % 250) - 125.0f;
		}
		else if (i<7){
			bear[i].sPos[0] = bear_start_pos[1][0] + (float)(rand() % 250) - 125.0f;
			bear[i].sPos[1] = bear_start_pos[1][1] + (float)(rand() % 250) - 125.0f;
		}
		else if (i<11){
			bear[i].sPos[0] = bear_start_pos[2][0] + (float)(rand() % 300) - 150.0f;
			bear[i].sPos[1] = bear_start_pos[2][1] + (float)(rand() % 300) - 150.0f;
		}
		else if (i<13){
			bear[i].sPos[0] = bear_start_pos[3][0] + (float)(rand() % 200) - 100.0f;
			bear[i].sPos[1] = bear_start_pos[3][1] + (float)(rand() % 200) - 100.0f;
		}
		else if (i<14){
			bear[i].sPos[0] = bear_start_pos[4][0];
			bear[i].sPos[1] = bear_start_pos[4][1];
		}
		else if (i<15){
			bear[i].sPos[0] = bear_start_pos[5][0];
			bear[i].sPos[1] = bear_start_pos[5][1];
		}
		else if (i<16){
			bear[i].sPos[0] = bear_start_pos[6][0];
			bear[i].sPos[1] = bear_start_pos[6][1];
		}
		bear[i].sPos[2] = 60.0f;
		bear[i].sFDir[0] = (float)rand() / (RAND_MAX); bear[i].sFDir[1] = (float)rand() / (RAND_MAX); bear[i].sFDir[2] = 0.0f;
		bear[i].sUDir[0] = 0.0f; bear[i].sUDir[1] = 0.0f; bear[i].sUDir[2] = 1.0f;
		bear[i].ID(bearID);
		bear[i].loadActions(3);
		bear[i].healthpoint = 150.0f;
		bear[i].butsukari_range = 30.0f;
		bear[i].attack_range = 100.0f;
		bear[i].territory_range = 600.0f;
		bear[i].movementspeed = 8.0f;
		// set the character to idle action
		bear[i].SetCurrentAction(NULL, 0, bear[i].curPoseID = bear[i].npc_idleID);
		bear[i].Play(START, 0.0f, FALSE, TRUE);
		// put the character on terrain
		bear[i].SetTerrainRoom(terrainRoomID, 10.0f);
		bear[i].PutOnTerrain(bear[i].sPos);
		bear[i].SetDirection(bear[i].sFDir, bear[i].sUDir);
		totalcharacterpool.push_back(&bear[i]);

		targetFX[i + DONZOMAX + ZAKOMAX].init("Blow_01e", "_enemy_hit_fire2.wav", &bear[i]);

		targetpool.push_back(&bear[i]);
		//butsukaripool.push_back(&bear[i]);
	}

	FySetModelPath("Data\\NTU6\\NPCs");
	FySetTexturePath("Data\\NTU6\\NPCs");
	FySetCharacterPath("Data\\NTU6\\NPCs");
	for (int i = 0; i < MANMAX; i++){
		CHARACTERid manID = scene.LoadCharacter("AMA002");
		if (i < 5) {
			man[i].sPos[0] = man_start_pos[0][0] + (float)(rand() % 250) - 125.0f;
			man[i].sPos[1] = man_start_pos[0][1] + (float)(rand() % 250) - 125.0f;
		}
		else {
			man[i].sPos[0] = man_start_pos[1][0] ;
			man[i].sPos[1] = man_start_pos[1][1] ;
		}
		man[i].sPos[2] = 60.0f;
		man[i].sFDir[0] = ((float)rand()) / (float)(RAND_MAX); man[i].sFDir[1] = ((float)rand()) / (float)(RAND_MAX); man[i].sFDir[2] = 0.0f;
		man[i].sUDir[0] = 0.0f; man[i].sUDir[1] = 0.0f; man[i].sUDir[2] = 1.0f;
		man[i].ID(manID);
		man[i].loadActions(3);
		man[i].healthpoint = 400.0f;
		man[i].butsukari_range = 30.0f;
		man[i].attack_range = 100.0f;
		man[i].territory_range = 800.0f;
		man[i].movementspeed = 15.0f;
		// set the character to idle action
		man[i].SetCurrentAction(NULL, 0, man[i].curPoseID = man[i].npc_idleID);
		man[i].Play(START, 0.0f, FALSE, TRUE);
		// put the character on terrain
		man[i].SetTerrainRoom(terrainRoomID, 10.0f);
		man[i].PutOnTerrain(man[i].sPos);
		man[i].SetDirection(man[i].sFDir, man[i].sUDir);
		totalcharacterpool.push_back(&man[i]);

		targetFX[i + DONZOMAX + ZAKOMAX + BEARMAX].init("Blow_01e", "_enemy_hit_fire2.wav", &man[i]);

		targetpool.push_back(&man[i]);
		//butsukaripool.push_back(&man[i]);
	}

	FySetModelPath("Data\\NTU6\\NPCs");
	FySetTexturePath("Data\\NTU6\\NPCs");
	FySetCharacterPath("Data\\NTU6\\NPCs");
	CHARACTERid bossID = scene.LoadCharacter("CA004");
	boss.sPos[0] = -337.0f; boss.sPos[1] = 2394.0f; boss.sPos[2] = 60.0f;
	boss.sFDir[0] = -1.0f; boss.sFDir[1] = 0.0f; boss.sFDir[2] = 0.0f;
	boss.sUDir[0] = 0.0f; boss.sUDir[1] = 0.0f; boss.sUDir[2] = 1.0f;
	boss.ID(bossID);
	boss.loadActions(3);
	boss.butsukari_range = 30.0f;
	boss.attack_range = 500.0f;
	boss.territory_range = 1200.0f;
	boss.movementspeed = 30.0f;
	boss.healthpoint = 1000.0f;
	// set the character to idle action
	boss.SetCurrentAction(NULL, 0, boss.curPoseID = boss.npc_idleID);
	boss.Play(START, 0.0f, FALSE, TRUE);
	// put the character on terrain
	boss.SetTerrainRoom(terrainRoomID, 10.0f);
	boss.PutOnTerrain(boss.sPos);
	boss.SetDirection(boss.sFDir, boss.sUDir);
	totalcharacterpool.push_back(&boss);
	targetpool.push_back(&boss);
	//butsukaripool.push_back(&boss);

	FySetModelPath("Data\\NTU6\\NPCs_More");
	FySetTexturePath("Data\\NTU6\\NPCs_More");
	FySetCharacterPath("Data\\NTU6\\NPCs_More");
	CHARACTERid boss_bulletID = scene.LoadCharacter("AN002");
	boss_bullet.sPos[0] = -337.0f; boss_bullet.sPos[1] = 2394.0f; boss_bullet.sPos[2] = -500.0f;
	boss_bullet.sFDir[0] = -1.0f; boss_bullet.sFDir[1] = 0.0f; boss_bullet.sFDir[2] = 0.0f;
	boss_bullet.sUDir[0] = 0.0f; boss_bullet.sUDir[1] = 0.0f; boss_bullet.sUDir[2] = 1.0f;
	boss_bullet.ID(boss_bulletID);
	boss_bullet.loadActions(3);
	boss_bullet.butsukari_range = 50.0f;
	boss_bullet.movementspeed = 60.0f;
	// set the character to idle action
	boss_bullet.SetCurrentAction(NULL, 0, boss_bullet.curPoseID = boss_bullet.npc_runID);
	boss_bullet.Play(START, 0.0f, FALSE, TRUE);
	// put the character on terrain
	//boss_bullet.SetTerrainRoom(terrainRoomID, 10.0f);
	boss_bullet.SetPosition(boss_bullet.sPos);
	boss_bullet.SetDirection(boss_bullet.sFDir, boss_bullet.sUDir);
	totalcharacterpool.push_back(&boss_bullet);

	for (int i = 0; i < BOSSMONSTERMAX; i++) {
		CHARACTERid boss_monsterID = scene.LoadCharacter("AN001");
		boss_monster[i].sPos[0] = boss.sPos[0] + boss_monster_dir[i][0];
		boss_monster[i].sPos[1] = boss.sPos[1] + boss_monster_dir[i][1];
		boss_monster[i].sPos[2] = 15.0f;

		boss_monster[i].sFDir[0] = boss_monster_dir[(i + 1) % 4][0] - boss_monster_dir[i][0];
		boss_monster[i].sFDir[1] = boss_monster_dir[(i + 1) % 4][1] - boss_monster_dir[i][1];
		boss_monster[i].sFDir[2] = boss.sFDir[2];

		boss_monster[i].sUDir[0] = boss.sUDir[0];
		boss_monster[i].sUDir[1] = boss.sUDir[1];
		boss_monster[i].sUDir[2] = boss.sUDir[2];

		boss_monster[i].ID(boss_monsterID);
		boss_monster[i].loadActions(3);
		boss_monster[i].butsukari_range = 50.0f;
		boss_monster[i].movementspeed = 40.0f;
		boss_monster[i].healthpoint = 100.0f;

		boss_monster[i].SetCurrentAction(NULL, 0, boss_monster[i].curPoseID = boss_monster[i].npc_runID);
		boss_monster[i].Play(START, 0.0f, FALSE, TRUE);

		boss_monster[i].SetPosition(boss_monster[i].sPos);
		boss_monster[i].SetDirection(boss_monster[i].sFDir, boss_monster[i].sUDir);
		//boss_monster[i].MoveForward(i*300.0f);
		totalcharacterpool.push_back(&boss_monster[i]);
		targetpool.push_back(&boss_monster[i]);
	}

	FySetModelPath("Data\\Models");
	FySetTexturePath("Data\\Models");
	OBJECTid goalID = scene.CreateObject();
	goal.ID(goalID);
	goal.Load("box2");
	goal.SetPosition(goal_pos);
	dummyFX.init("Blow_01e", "_enemy_hit_fire2.wav", NULL);

	FySetModelPath("Data\\NTU6\\Characters");
	FySetTexturePath("Data\\NTU6\\Characters");
	FySetCharacterPath("Data\\NTU6\\Characters");
	CHARACTERid dddID = scene.LoadCharacter("Donzo2");
	dummygoal.ID(dddID);
	dummygoal.SetDirection(fDir, uDir);
	float pppos[3] = { goal_pos[0], goal_pos[1], -500.0 };
	dummygoal.SetPosition(pppos);
	dummygoal.healthpoint = 250.0f;
	targetpool.push_back(&dummygoal);
	// translate the camera
	cID = scene.CreateObject(CAMERA);
	camera.ID(cID);
	camera.SetNearPlane(5.0f);
	camera.SetFarPlane(100000.0f);

	cID2 = scene.CreateObject(CAMERA);
	camera2.ID(cID2);
	camera2.SetNearPlane(5.0f);
	camera2.SetFarPlane(100000.0f);

	float mainLightPos[3] = { -4579.0f, -714.0f, 15530.0f };
	float mainLightFDir[3] = { 0.276f, 0.0f, -0.961f };
	float mainLightUDir[3] = { 0.961f, 0.026f, 0.276f };

	FnLight lgt;
	lgt.ID(scene.CreateObject(LIGHT));
	lgt.Translate(mainLightPos[0], mainLightPos[1], mainLightPos[2], REPLACE);
	lgt.SetDirection(mainLightFDir, mainLightUDir);
	lgt.SetLightType(PARALLEL_LIGHT);
	lgt.SetColor(1.0f, 1.0f, 1.0f);
	lgt.SetName("MainLight");
	lgt.SetIntensity(0.4f);



	// create a text object for displaying messages on screen
	textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);

	// set Hotkeys
	FyDefineHotKey(FY_ESCAPE, QuitGame, FALSE);
	FyDefineHotKey(FY_UP, Movement, FALSE);
	FyDefineHotKey(FY_DOWN, Movement, FALSE);
	FyDefineHotKey(FY_LEFT, Movement, FALSE);
	FyDefineHotKey(FY_RIGHT, Movement, FALSE);
	FyDefineHotKey(FY_RETURN, Movement, FALSE);
	FyDefineHotKey(FY_W, Movement, FALSE);
	FyDefineHotKey(FY_A, Movement, FALSE);
	FyDefineHotKey(FY_S, Movement, FALSE);
	FyDefineHotKey(FY_D, Movement, FALSE);
	FyDefineHotKey(FY_Q, Movement, FALSE);
	FyDefineHotKey(FY_E, Movement, FALSE);
	FyDefineHotKey(FY_1, Attack, FALSE);
	FyDefineHotKey(FY_2, Attack, FALSE);
	FyDefineHotKey(FY_3, Attack, FALSE);
	FyDefineHotKey(FY_4, Attack, FALSE);
	FyDefineHotKey(FY_5, Attack, FALSE);
	FyDefineHotKey(FY_6, Attack, FALSE);
	FyDefineHotKey(FY_7, Attack, FALSE);
	FyDefineHotKey(FY_8, Attack, FALSE);
	FyDefineHotKey(VK_ADD, Utility, FALSE);
	FyDefineHotKey(VK_SUBTRACT, Utility, FALSE);
	FyDefineHotKey(VK_NUMPAD8, Utility, FALSE);
	FyDefineHotKey(VK_NUMPAD2, Utility, FALSE);
	FyDefineHotKey(VK_NUMPAD4, Utility, FALSE);
	FyDefineHotKey(VK_NUMPAD6, Utility, FALSE);
	FyDefineHotKey(VK_NUMPAD5, Utility, FALSE);

	// define some mouse functions

	// bind timers, frame rate = 30 fps
	FyBindTimer(0, 30.0f, GameAI, TRUE);
	FyBindTimer(1, 30.0f, RenderIt, TRUE);
	FyBindTimer(2, 30.0f, ZakoAI, TRUE);
	FyBindTimer(3, 30.0f, Gravity, TRUE);
	FyBindTimer(4, 30.0f, ItrTimer, TRUE);
	FyBindTimer(5, 30.0f, TeaPotTimer, TRUE);
	// invoke the system

	init_UI(&Lvbu);
	FyInvokeFly(TRUE);
}

void newDirection(const float u[3], const float f[3], float n[3]) {
	float t = (u[0] * f[0] + u[1] * f[1] + u[2] * f[2]) / (f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
	n[0] = u[0] - f[0] * t;
	n[1] = u[1] - f[1] * t;
	n[2] = u[2] - f[2] * t;
	return;
}

void calcCameraXYZ(float source[3], float destination[3], float distance) {
	float dx = destination[0] - source[0];
	float dy = destination[1] - source[1];
	float ds2 = dx * dx + dy * dy;
	float ds1 = (float)sqrt(ds2);
	if (ds1 < distance) {
		cameraPosition[0] = destination[0];
		cameraPosition[1] = destination[1];
		cameraPosition[2] = source[2] + cameraHeight * ds1 / distance;
	}
	else {
		float ratio = distance / ds1;
		cameraPosition[0] = source[0] + dx * ratio;
		cameraPosition[1] = source[1] + dy * ratio;
		cameraPosition[2] = source[2] + cameraHeight;
	}
	return;
}

void adjustCamera(float aPos[3], float fDir[3], float uDir[3]) {
	dummy.SetPosition(aPos);
	dummy.SetDirection(fDir, uDir);
	dummy.TurnRight(cameraAngle);
	dummy.GetPosition(aPos);
	dummy.GetDirection(fDir, uDir);
	float point[3];
	float newup[3];
	fDir[0] = -fDir[0];
	fDir[1] = -fDir[1];
	aPos[2] += headHeight;
	// positive: collision
	terrain.HitTest(aPos, fDir, point);
	calcCameraXYZ(aPos, point, cameraZoom);
	fDir[0] = aPos[0] - cameraPosition[0];
	fDir[1] = aPos[1] - cameraPosition[1];
	fDir[2] = aPos[2] - cameraPosition[2];
	newDirection(uDir, fDir, newup);
	camera.SetPosition(cameraPosition);
	camera.SetDirection(fDir, newup);

	fDir[0] = 0.0f; fDir[1] = 0.0f; fDir[2] = -1.0f;
	aPos[2] = 3000.0f;
	newDirection(uDir, fDir, newup);
	camera2.SetPosition(aPos);
	camera2.SetDirection(fDir, newup);
	return;
}

void teasystem() {
	for (auto& x : teapool) {
		if (x->alive) {
			if (x->position[0] > actorPosition[0] - 30 && x->position[0] < actorPosition[0] + 30 &&
				x->position[1] > actorPosition[1] - 30 && x->position[1] < actorPosition[1] + 30) {
				x->alive = false;
				int effect = (rand() % 4) + 1;
				Treasure *neo = new Treasure(effect);
				Lvbu.getTreasure(neo);
				randomSlot(effect);
				x->position[2] = -999.0f;
				x->SetPosition(x->position);
				continue;
			}
			x->TurnRight(6.0f);
			x->position[2] = 60.0f + 10.0f * (float)sin(x->currangle += 0.1f);
			x->SetPosition(x->position);
		}
	}
	return;
}

void potsystem() {
	for (std::list<FnObject>::iterator x = potpool.begin(); x != potpool.end();) {
		(*x).position[0] += (*x).direction[0];
		(*x).position[1] += (*x).direction[1];
		(*x).position[2] += (*x).direction[2];
		if ((*x).position[2] < 3.0f) {
			BoomFX.Start(sID, (*x).position);
			(*x).position[2] = -500.0f;
			x->SetPosition((*x).position);
			Itr neo(&Lvbu);
			neo.setBounds(190, 490, 2300);
			neo.injury = 550.0f;
			neo.setBomb(&(*x));
			injuryQueue.push_back(neo);
			x = potpool.erase(x);
			continue;
		}
		(*x).SetPosition((*x).position);
		(*x).TurnRight(6.0f);
		if (!(*x).alive) {
			FySetModelPath("Data\\Models");
			OBJECTid teapotid = scene.CreateObject(OBJECT);
			(*x).ID(teapotid);
			(*x).Load("teapot");
			(*x).alive = true;
		}
		++x;
	}
	return;
}

void TeaPotTimer(int skip) {
	teasystem();
	if (Lvbu.mono != NULL && --Lvbu.mono->span == 0)
		Lvbu.effect88();
	if (throwspan >= 0) {
		;
	}
	else if (++throwspan == 0) {
		float dir[3];
		Lvbu.GetDirection(dir, NULL);
		dir[2] = 0.3f;
		potpool.push_back(FnObject(false, actorPosition, dir));
	}
	potsystem();
	return;
}

void ItrTimer(int skip) {
	if (game_end_flag) {
		return;
	}

	for (std::list<Itr>::iterator itr = injuryQueue.begin(); itr != injuryQueue.end();) {
		if ((*itr).timer <= 0) {
			if ((*itr).timer != -9)
				(*itr).src->GetPosition((*itr).pos);
			(*itr).src->GetDirection((*itr).dir, NULL);
			for (auto& x : targetpool) {
				// alive
				if (x->mode != ONCE){
					if (x->injured(*itr)){
						if (x == &dummygoal){
							float xyz[3];
							goal.GetPosition(xyz);
							dummyFX.Start(sID,xyz);
						}
						for (int i = 0; i < NUMMAX; i++){
							if (targetFX[i].isID(x)){
								targetFX[i].Start(sID);
								break;
							}
						}
					}
				}
			}
			itr = injuryQueue.erase(itr);
		}
		else {
			--((*itr).timer);
			++itr;
		}
	}
	for (auto& x : targetpool) {
		if (x->recoverystatus >= 0) {
			x->Play(x->mode, (float)skip, FALSE, TRUE);
		}
		else if (++x->recoverystatus == 0) {
			x->SetCurrentAction(NULL, 0, x->curPoseID, 4);
		}
		else {
			x->Play(ONCE, (float)skip, FALSE, TRUE);
		}
	}

	return;
}
/*-------------------------------------------------------------
30fps timer callback in fixed frame rate for major game loop
--------------------------------------------------------------*/
void GameAI(int skip) {
	if (game_end_flag) {
		if (rotate_time == 60){
			if (Lvbu.power <= 0) {
				Lvbu.SetCurrentAction(NULL, 0, Lvbu.deadID, 4);
				Lvbu.Play(START, 0.0f, FALSE, TRUE);
			}
			else {
				Lvbu.TurnRight(180.0f);
				Lvbu.SetCurrentAction(NULL, 0, Lvbu.ultID, 4);
				Lvbu.Play(START, 0.0f, FALSE, TRUE);
			}
			rotate_time--;
		}
		else if (rotate_time > 0){
			camera.MoveForward(-2.0f);
			rotate_time--;
		}
		Lvbu.Play(ONCE, (float)skip, FALSE, TRUE);

		if (KEY_R && restartFlag){
			game_end_flag = 0;
			game_Restart();
			init_UI(&Lvbu);
			initKEY();
			Lvbu.power = 99;
		}

		return;
	}

	if (attackspan >= 0) {
		Lvbu.Play(LOOP, (float)skip, FALSE, TRUE);
	}
	else if (++attackspan == 0) {
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.curPoseID, 4);
	}
	else {
		Lvbu.Play(ONCE, (float)skip, FALSE, TRUE);
	}
	if (KEY_R && titleFlag){
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

	checkendgame();

	if (global_time > 0.0f) {
		float pos[3];
		Lvbu.GetPosition(pos);
		pos[0] = pos[0] + global_fDir[0] * 60.0f;
		pos[1] = pos[1] + global_fDir[1] * 60.0f;
		Lvbu.PutOnTerrain(pos);

		global_time = global_time - 1.0f;
	}

	if (KEY_W && KEY_S) {
		;
	}
	else if (KEY_W) {
		Lvbu.MoveForward(Lvbu.movementspeed, TRUE);
	}
	else if (KEY_S) {
		Lvbu.MoveForward(-Lvbu.movementspeed * 0.5f, TRUE);
	}

	if (KEY_UP && KEY_DOWN) {
		;
	}
	else if (KEY_UP) {
		Lvbu.MoveForward(Lvbu.movementspeed, TRUE);
	}
	else if (KEY_DOWN) {
		Lvbu.MoveForward(-Lvbu.movementspeed, TRUE);
	}

	if (KEY_RIGHT && KEY_LEFT) {
		;
	}
	else if (KEY_RIGHT) {
		Lvbu.TurnRight(rotatespeed);
		Lvbu.MoveForward(Lvbu.movementspeed * .65f, TRUE);
	}
	else if (KEY_LEFT) {
		Lvbu.TurnRight(-rotatespeed);
		Lvbu.MoveForward(Lvbu.movementspeed * .65f, TRUE);
	}

	if (KEY_A && KEY_D) {
		;
	}
	else if (KEY_D) {
		Lvbu.MoveRight(Lvbu.movementspeed * .77f, TRUE);
	}
	else if (KEY_A) {
		Lvbu.MoveRight(-Lvbu.movementspeed * .77f, TRUE);
	}

	if (KEY_Q && KEY_E) {
		;
	}
	else if (KEY_E) {
		Lvbu.TurnRight(rotatespeed);
	}
	else if (KEY_Q) {
		Lvbu.TurnRight(-rotatespeed);
	}

	if (KEY_NUMADD && KEY_NUMMINUS) {
		;
	}
	else if (KEY_NUMMINUS) {
		if (cameraZoom > 200.0f) {
			cameraZoom *= 0.94f;
			cameraHeight *= 0.94f;
		}
	}
	else if (KEY_NUMADD) {
		cameraZoom *= 1.06f;
		cameraHeight *= 1.06f;
	}

	if (KEY_NUM8 && KEY_NUM2) {
		;
	}
	else if (KEY_NUM8) {
		cameraHeight += 30.0f;
	}
	else if (KEY_NUM2) {
		if (cameraHeight > 30.0f)
			cameraHeight -= 30.0f;
	}

	if (KEY_NUM4 && KEY_NUM6) {
		;
	}
	else if (KEY_NUM4) {
		cameraAngle += 10.0f;
	}
	else if (KEY_NUM6) {
		cameraAngle -= 10.0f;
	}

	float aPos[3], fDir[3], uDir[3];
	Lvbu.GetPosition(aPos);
	actorPosition[0] = aPos[0];
	actorPosition[1] = aPos[1];
	actorPosition[2] = aPos[2];
	Lvbu.GetDirection(fDir, uDir);
	adjustCamera(aPos, fDir, uDir);

	bossAI(skip);
	bearAI(skip);
	manAI(skip);
	//donzoAI(skip);

	return;
}


void checkendgame() {
	float pos[3];
	Lvbu.GetPosition(pos);
	float pppos[3] = { goal_pos[0], goal_pos[1], -500.0 };
	dummygoal.SetPosition(pppos);
	goal.TurnRight(3.0f);
	//if (abs(pos[0] - goal_pos[0]) < 200.0f && abs(pos[1] - goal_pos[1]) < 200.0f){
	if (dummygoal.healthpoint <=0.0f){
		float pos[3] = { goal_pos[0], goal_pos[1], goal_pos[2] - 500.0 };
		goal.SetPosition(pos);
		game_end_flag = 1;
		endFlag = TRUE;
		rotate_time = 90;
	}
	else if (Lvbu.power <= 0) {
		game_end_flag = 1;
		rotate_time = 90;
		failFlag = TRUE;
	}
	
}

void game_Restart() {
	for (auto& x : totalcharacterpool){
		x->SetDirection(x->sFDir, x->sUDir);
		x->PutOnTerrain(x->sPos);
	}
	float pppos[3] = {goal_pos[0], goal_pos[1], -500.0f};
	dummygoal.SetPosition(pppos);
	dummygoal.healthpoint = 250.0f;
	
	for (int i = 0; i < BEARMAX; i++){
		bear[i].mode = LOOP;
		bear[i].healthpoint = 150.0f;
		bear[i].SetCurrentAction(NULL, 0, bear[i].idleID);
		bear[i].Play(START, 0.0f, FALSE, TRUE);
		
	}
	for (int i = 0; i < MANMAX; i++){
		man[i].mode = LOOP;
		man[i].healthpoint = 400.0f;
		man[i].SetCurrentAction(NULL, 0, man[i].idleID);
		man[i].Play(START, 0.0f, FALSE, TRUE);
	}
	for (auto& x: teapool) {
		float pos[3];
		x->alive = true;
		x->position[2] = 60.0f + 10.0f * (float)sin(x->currangle);
		x->SetPosition(x->position);
	}
	start = clock();
	Lvbu.SetCurrentAction(NULL, 0, Lvbu.idleID);
}

void donzoAI(int skip) {
	int range;

	for (int i = 0; i < DONZOMAX; i++) {
		range = check_range(Donzo[i]);
		if (range == 1)		collision_back(Donzo[i]);

	}

}

void manAI(int skip){
	int range;

	int num_attack_type = 1;
	float attack_dis[10] = { man[0].attack_range };
	float attackspan_list[10] = { 54.0f - 10.0f };

	for (int i = 0; i < MANMAX; i++){
		if (man[i].mode == ONCE) {
			check_die(man[i], skip);
			continue;
		}

		ACTIONid attack_pose[10] = { man[i].npc_attackID };
		int who = 1 + BEARMAX + i;
		if (total_attack_time_span[who] == 4.0f || total_attack_time_span[who] == 24.0f) {
			range = check_range(man[i]);
			if (range == 2){
				collision_back(man[i]);
			}

			total_attack_time_span[who]--;
		}
		else if (total_attack_time_span[who] >= 0.0f) {
			// man[i].Play(ONCE, (float)skip, FALSE, TRUE);
			total_attack_time_span[who]--;
		}

		range = check_range(man[i]);
		switch (range) {
		case 1:
			collision_back(man[i]);
			break;
		case 2:
			if (total_attack_time_span[who] < 0.0f) {
				npc_face(Lvbu, man[i]);
				npc_attack(Lvbu, man[i], num_attack_type, attack_pose, attack_dis, attackspan_list, who, skip);
				WolfFX.Start(sID);
			}
			break;
		case 3:
			if (total_attack_time_span[who] < 0.0f) {
				move_toward(Lvbu, man[i], skip);
			}
			break;
		default:
			check_stop(man[i], skip);
			break;
		}
	}
}

void bearAI(int skip){
	int range;

	int num_attack_type = 1;
	float attack_dis[10] = { bear[0].attack_range };
	float attackspan_list[10] = { 147.0f - 115.0f };

	for (int i = 0; i < BEARMAX; i++){
		if (bear[i].mode == ONCE) {
			check_die(bear[i], skip);
			continue;
		}

		ACTIONid attack_pose[10] = { bear[i].npc_attackID };
		int who = 1 + i;
		if (total_attack_time_span[who] == 20.0f) {
			range = check_range(bear[i]);
			if (range == 2){
				collision_back(bear[i]);
			}

			total_attack_time_span[who]--;
		}
		else if (total_attack_time_span[who] >= 0.0f) {
			// bear[i].Play(ONCE, (float)skip, FALSE, TRUE);
			total_attack_time_span[who]--;
		}

		range = check_range(bear[i]);
		switch (range) {
		case 1:
			collision_back(bear[i]);
			break;
		case 2:
			if (total_attack_time_span[who] < 0.0f) {
				npc_face(Lvbu, bear[i]);
				npc_attack(Lvbu, bear[i], num_attack_type, attack_pose, attack_dis, attackspan_list, who, skip);
				BearFX.Start(sID);
			}
			break;
		case 3:
			if (total_attack_time_span[who] < 0.0f) {
				move_toward(Lvbu, bear[i], skip);
			}
			break;
		default:
			check_stop(bear[i], skip);
			break;
		}
	}
}

void bossAI(int skip) {
	if (total_attack_time_span[0] == 20.0f || total_attack_time_span[0] == 21.0f) {
		total_attack_time_span[0] -= 2;
		if (boss.GetCurrentAction(NULL, 0) == boss.npc_skillID) {
			float pos[3], fDir[3], uDir[3];
			boss.GetPosition(pos);
			boss.GetDirection(fDir, uDir);
			pos[0] = pos[0] + fDir[0] * 10.0f;
			pos[1] = pos[1] + fDir[1] * 10.0f;
			pos[2] = 30.0f;
			boss_bullet.SetPosition(pos);
			boss_bullet.SetDirection(fDir, uDir);
		}
	}
	else if (total_attack_time_span[0] >= 0.0f) {
		boss.Play(ONCE, (float)skip, FALSE, TRUE);
		boss.Play(ONCE, (float)skip, FALSE, TRUE);
		total_attack_time_span[0] -= 2;
	}

	float pos[3];
	boss_bullet.GetPosition(pos);
	if (pos[2] >= 0.0f) {
		boss_bullet.MoveForward(boss_bullet.movementspeed, FALSE);
	}

	int range;
	range = check_range(boss_bullet);
	switch (range) {
	case 1:
		collision_back(boss_bullet);
		boss_bullet.SetPosition(boss_bullet.sPos);
		break;
	}

	for (int i = 0; i < BOSSMONSTERMAX; i++){
		range = check_range(boss_monster[i]);
		if (range == 1) {
			collision_back(boss_monster[i]);
			break;
		}

		float bossPos[3], mpos[3], fDir[3], uDir[3];
		boss.GetPosition(bossPos);
		boss_monster[i].GetPosition(mpos);
		boss_monster[i].GetDirection(fDir, uDir);

		float dis = (bossPos[0] + boss_monster_dir[boss_monster_target[i]][0] - mpos[0])*(bossPos[0] + boss_monster_dir[boss_monster_target[i]][0] - mpos[0]) + (bossPos[1] + boss_monster_dir[boss_monster_target[i]][1] - mpos[1])*(bossPos[1] + boss_monster_dir[boss_monster_target[i]][1] - mpos[1]);
		if (dis < 1000.0f) {
			boss_monster_target[i] = (boss_monster_target[i] + 1) % 4;
		}
		fDir[0] = bossPos[0] + boss_monster_dir[boss_monster_target[i]][0] - mpos[0];
		fDir[1] = bossPos[1] + boss_monster_dir[boss_monster_target[i]][1] - mpos[1];
		boss_monster[i].SetDirection(fDir, uDir);

		boss_monster[i].MoveForward(boss_monster[i].movementspeed);

	}

	range = check_range(boss);
	int num_attack_type = 2;
	ACTIONid attack_pose[10] = { boss.npc_attackID, boss.npc_skillID };
	float attack_dis[10] = { 150.0f, 500.0f };
	float attackspan_list[10] = { 155.0f - 110.0f, 220.0f - 160.0f };
	//float bound_list[10][3] = { { 50.0f, 150.0f, 100.0f }, { 30.0f, 300.0f, 100.0f } };
	//float injury_list[10] = { 50.0f , 100.0f };
	//float timer_list[10] = { 6, 6 };

	switch (range) {
	case 1:
		collision_back(boss);
		break;
	case 2:
		if (total_attack_time_span[0] < 0.0f) {
			npc_face(Lvbu, boss);
			npc_attack(Lvbu, boss, num_attack_type, attack_pose, attack_dis, attackspan_list, 0, skip);
			BossFX.Start(sID);
		}
		break;
	case 3:
		if (total_attack_time_span[0] < 0.0f) {
			move_toward(Lvbu, boss, skip);
		}
		break;
	default:
		check_stop(boss, skip);
		break;
	}
}

void check_die(FnCharacter& actor, int skip) {
	if (actor.GetCurrentAction(NULL, 0) != actor.npc_deadID){
		actor.SetCurrentAction(NULL, 0, actor.npc_deadID, 4);
		actor.Play(START, 0.0f, FALSE, TRUE);
	}
	else {
		actor.Play(ONCE, (float)skip, FALSE, TRUE);
	}
}

void check_stop(FnCharacter& actor, int skip){
	if (actor.GetCurrentAction(NULL, 0) != actor.npc_idleID){
		actor.SetCurrentAction(NULL, 0, actor.npc_runID, 4);
		actor.Play(START, 0.0f, FALSE, TRUE);
	}
	else {
		actor.Play(LOOP, (float)skip, FALSE, TRUE);
	}
}

int check_range(FnCharacter& actor2) {
	float pos2[3];
	actor2.GetPosition(pos2);

	float dis = (pos2[0] - actorPosition[0])*(pos2[0] - actorPosition[0]) + (pos2[1] - actorPosition[1])*(pos2[1] - actorPosition[1]);
	if (dis < (Lvbu.butsukari_range + actor2.butsukari_range)*(Lvbu.butsukari_range + actor2.butsukari_range)) return 1;
	else if (dis < actor2.attack_range*actor2.attack_range) return 2;
	else if (dis < actor2.territory_range*actor2.territory_range) return 3;
	else return 0;
}

void collision_back(FnCharacter& actor2) {
	if (Lvbu.muteki)	return;
	float pos[3], pos2[3], dis, fdir[3];
	actor2.GetPosition(pos2);

	fdir[0] = actorPosition[0] - pos2[0];
	fdir[1] = actorPosition[1] - pos2[1];
	fdir[2] = 1.0f;
	actor2.SetDirection(fdir, NULL);

	actor2.GetDirection(fdir, NULL);
	global_fDir[0] = fdir[0];
	global_fDir[1] = fdir[1];
	global_fDir[2] = fdir[2];

	global_time = 5.0f;

	Lvbu.power -= 1;
	LyubudFX.Start(sID);
}

void npc_face(FnCharacter& actor1, FnCharacter& actor2) {
	float pos1[3], pos2[3];
	float fDir[3], uDir[3] = { 0.0f, 0.0f, 1.0f };

	actor1.GetPosition(pos1);
	actor2.GetPosition(pos2);

	fDir[0] = pos1[0] - pos2[0];
	fDir[1] = pos1[1] - pos2[1];
	fDir[2] = 0.0f;

	actor2.SetDirection(fDir, uDir);
}

void move_toward(FnCharacter& actor1, FnCharacter& actor2, int skip) {
	float pos1[3], pos2[3];
	float fDir[3], uDir[3] = { 0.0f, 0.0f, 1.0f };

	actor1.GetPosition(pos1);
	actor2.GetPosition(pos2);

	fDir[0] = pos1[0] - pos2[0];
	fDir[1] = pos1[1] - pos2[1];
	fDir[2] = 0.0f;

	if (actor2.GetCurrentAction(NULL, 0) != actor2.npc_runID){
		actor2.SetCurrentAction(NULL, 0, actor2.npc_runID, 4);
		actor2.Play(START, 0.0f, FALSE, TRUE);
	}
	else{
		actor2.Play(LOOP, (float)skip, FALSE, TRUE);
	}

	actor2.SetDirection(fDir, uDir);
	actor2.MoveForward(actor2.movementspeed, TRUE);
}

//void npc_attack(FnCharacter actor1, FnCharacter actor2, int num_attack_type, ACTIONid attack_pose[10], float attack_dis[10], int attackspan_list[10], float bound_list[10][3], float injury_list[10], float timer_list[10]) {
void npc_attack(FnCharacter& actor1, FnCharacter& actor2, int num_attack_type, ACTIONid attack_pose[10], float attack_dis[10], float attackspan_list[10], int who, int skip) {
	float pos1[3], pos2[3];
	actor1.GetPosition(pos1);
	actor2.GetPosition(pos2);

	float dis = (pos2[0] - pos1[0])*(pos2[0] - pos1[0]) + (pos2[1] - pos1[1])*(pos2[1] - pos1[1]);
	for (int i = 0; i < num_attack_type; i++) {
		if (dis < attack_dis[i] * attack_dis[i]) {
			actor2.SetCurrentAction(NULL, 0, attack_pose[i], 4);
			total_attack_time_span[who] = attackspan_list[i];
			actor2.Play(START, 0.0f, FALSE, TRUE);
			break;
		}
	}
}

void PutZako() {
	float pos[3];
	for (int i = 0; i < ZAKOMAX; i++) {
		if (zako[i].mode == ONCE)	continue;
		zako[i].GetPosition(pos);
		if (pos[2] >= 0)	continue;
		if (zako[i].curPoseID != zako[i].zako_idleID) {
			zako[i].curPoseID = zako[i].zako_idleID;
			zako[i].SetCurrentAction(NULL, 0, zako[i].zako_idleID, 4);
		}
		if (pos[2] < 0) zako[i].PutOnTerrain(zako_start);
		break;
	}
	return;
}

void MoveZako() {
	float fdir[3] = { 0 }, udir[3] = { 0.0f, 0.0f, 1.0f }, pos[3];
	for (int i = 0; i < ZAKOMAX; i++) {
		if (zako[i].mode == ONCE)	continue;
		zako[i].GetPosition(pos);
		if (pos[2] < 0)	continue;
		fdir[0] = zako_goal[0] - pos[0]; fdir[1] = zako_goal[1] - pos[1];
		zako[i].SetDirection(fdir, udir);
		if (zako[i].curPoseID != zako[i].zako_runID) {
			zako[i].curPoseID = zako[i].zako_runID;
			zako[i].SetCurrentAction(NULL, 0, zako[i].zako_runID, 4);
		}
		zako[i].butsukari_flag = 0;
		ZakoMoveTest(&zako[i]);
		zako[i].MoveForward(Lvbu.movementspeed / 2.0f, TRUE);
	}
	return;
}

void ZakoMoveTest(FnCharacter* me) {
	float fdir1[3], fdir2[3], tmp[3], pos1[3], pos2[3], zz;
	int t;

	me->GetPosition(pos1);
	for (auto& x : butsukaripool) {
		x->GetPosition(pos2);
		if (x->butsukari_flag == 0 || pos2[2] < 0 || x->healthpoint <= 0.0f || x->butsukari_range_NPC <=0.0f) { continue; }

		if ((pos1[0] - pos2[0])*(pos1[0] - pos2[0]) + (pos1[1] - pos2[1])*(pos1[1] - pos2[1]) < (x->butsukari_range_NPC + me->butsukari_range)*(x->butsukari_range_NPC + me->butsukari_range)) {
			me->GetDirection(fdir1, tmp);
			//x->GetDirection(fdir2, tmp);
			fdir2[0] = pos2[0] - pos1[0]; fdir2[1] = pos2[1] - pos1[1];
			zz = fdir1[0] * fdir2[1] - fdir1[1] * fdir2[0];
			t = (zz > 0) ? 1 : -1;
			me->TurnRight(t*30.0f);
		}
	}
	me->butsukari_flag = 1;


	return;
}

void RemoveZako(void) {
	int rr = 5000, RR;
	float pos[3];
	for (int i = 0; i < ZAKOMAX; i++) {
		if (zako[i].mode == ONCE)	continue;
		zako[i].GetPosition(pos);
		if (pos[2] < 0)	continue;
		RR = (int)((zako_goal[0] - pos[0])*(zako_goal[0] - pos[0]) + (zako_goal[1] - pos[1])*(zako_goal[1] - pos[1]));
		if (RR < rr) {
			pos[2] = -500;
			zako[i].SetPosition(pos);
		}
	}
	return;
}

void ZakoAI(int skip) {
	for (int i = 0; i < ZAKOMAX; i++)
		zako[i].Play(zako[i].mode, (float)skip, FALSE, TRUE);
	if ((++zako_timer & 15) == 1) PutZako();
	MoveZako();
	RemoveZako();

	int range;
	for (int i = 0; i < ZAKOMAX; i++){
		range = check_range(zako[i]);
		if (range == 1) {
			//collision_back(zako[i]);
			break;
		}
	}

	return;
}

void Gravity(int skip) {
	for (auto& x : potpool) {
		if (x.position[2] + x.direction[2] < 1) {
			x.direction[2] = 0.0f;
			x.SetTerrainRoom(terrainRoomID, 10.0f);
			x.PutOnTerrain(x.position);
			continue;
		}
		x.direction[2] -= 0.5f;
	}
}
/*------------------
movement control
-------------------*/
void Movement(BYTE code, BOOL4 value) {
	// value TURE -> pressing
	// value FALSE -> releasing

	if (code == FY_RETURN)
		KEY_R = value;
	if (game_end_flag || startFlag) {
		return;
	}

	switch (code) {
	case FY_W:	KEY_W = value;	break;
	case FY_A:	KEY_A = value;	break;
	case FY_S:	KEY_S = value;	break;
	case FY_D:	KEY_D = value;	break;
	case FY_Q:	KEY_Q = value;	break;
	case FY_E:	KEY_E = value;	break;
	case FY_UP:		KEY_UP = value;		break;
	case FY_DOWN:	KEY_DOWN = value;	break;
	case FY_LEFT:	KEY_LEFT = value;	break;
	case FY_RIGHT:	KEY_RIGHT = value;	break;
	case FY_RETURN:	KEY_R = value;	break;
	}

	if (KEY_W || KEY_A || KEY_D || KEY_UP || KEY_DOWN || KEY_LEFT || KEY_RIGHT) {
		if (Lvbu.curPoseID != Lvbu.runID) {
			Lvbu.curPoseID = Lvbu.runID;
			if (attackspan >= 0)
				Lvbu.SetCurrentAction(NULL, 0, Lvbu.curPoseID, 4);
		}
	}
	else if (KEY_S) {
		if (Lvbu.curPoseID != Lvbu.walkID) {
			Lvbu.curPoseID = Lvbu.walkID;
			if (attackspan >= 0)
				Lvbu.SetCurrentAction(NULL, 0, Lvbu.curPoseID, 4);
		}
	}
	else {
		if (Lvbu.curPoseID != Lvbu.idleID) {
			Lvbu.curPoseID = Lvbu.idleID;
			if (attackspan >= 0)
				Lvbu.SetCurrentAction(NULL, 0, Lvbu.curPoseID, 4);
		}
	}
	return;
}

/*------------------
Attack control
-------------------*/

void Attack(BYTE code, BOOL4 value) {
	if (attackspan < 0 || (code == FY_7 && Lvbu.power <= 10)) return;
	Itr neo(&Lvbu);
	switch (code) {
	case FY_1:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.na1ID, 4);
		attackspan = 121 - 145;
		neo.setBounds(30, 200, 10);
		neo.injury = 70.0f;
		neo.timer = 6;
		injuryQueue.push_back(neo);
		break;
	case FY_2:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.na2ID, 4);
		attackspan = 171 - 218;
		neo.setBounds(200, 240, 160);
		neo.injury = 50.0f;
		neo.timer = 14;
		injuryQueue.push_back(neo);
		break;
	case FY_3:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.na3ID, 4);
		attackspan = 231 - 276;
		neo.setBounds(80, 240, 280);
		neo.injury = 170.0f;
		neo.timer = 21;
		injuryQueue.push_back(neo);
		break;
	case FY_4:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.na4ID, 4);
		attackspan = 291 - 339;
		break;
	case FY_5:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.ha1ID, 4);
		attackspan = 351 - 423;
		break;
	case FY_6:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.ha2ID, 4);
		attackspan = 431 - 489;
		break;
	case FY_7:
		Lvbu.power -= 10;
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.ha3ID, 4);
		attackspan = 501 - 557;
		throwspan = 501 - 557 + 30;
		break;
	case FY_8:
		Lvbu.SetCurrentAction(NULL, 0, Lvbu.ultID, 4);
		attackspan = 971 - 1091;
		neo.setBounds(120, 240);
		neo.timer = 20;
		injuryQueue.push_back(neo);
		neo.injury = 15;
		neo.setBounds(60, 240);
		neo.timer = 37;
		injuryQueue.push_back(neo);
		neo.timer = 45;
		injuryQueue.push_back(neo);
		neo.timer = 54;
		injuryQueue.push_back(neo);
		neo.timer = 62;
		injuryQueue.push_back(neo);
		neo.setBounds(180, 240);
		neo.injury = 55;
		neo.timer = 77;
		injuryQueue.push_back(neo);
		neo.timer = 89;
		injuryQueue.push_back(neo);
		neo.injury = 75;
		neo.timer = 101;
		injuryQueue.push_back(neo);
		break;
	}
	LyubuFX.Start(sID);
	return;
}

/*------------------
camera control
-------------------*/
void Utility(BYTE code, BOOL4 value) {
	switch (code) {
	case VK_NUMPAD8:	KEY_NUM8 = value;	break;
	case VK_NUMPAD2:	KEY_NUM2 = value;	break;
	case VK_NUMPAD4:	KEY_NUM4 = value;	break;
	case VK_NUMPAD6:	KEY_NUM6 = value;	break;
	case VK_NUMPAD5:
		cameraZoom = 480.0f;
		cameraHeight = 40.0f;
		cameraAngle = 0.0f;
		break;
	case VK_ADD:		KEY_NUMADD = value;		break;
	case VK_SUBTRACT:	KEY_NUMMINUS = value;	break;
	}
	return;
}



/*----------------------
perform the rendering
C.Wang 0720, 2006
-----------------------*/
void RenderIt(int skip) {

	for (int i = 0; i < NUMMAX; i++)
		targetFX[i].Play(skip, sID);
	LyubuFX.Play(skip, sID);
	BoomFX.Play(skip, sID);
	LyubudFX.Play(skip, sID);
	dummyFX.Play(skip, sID);
	

	// render the whole scene
	vp.Render3D(cID, TRUE, TRUE);
	if (game_end_flag == 0)	vp2.Render3D(cID2, TRUE, TRUE);
	vp.RenderSprites(sID2, FALSE, TRUE);
	/*
	// show frame rate
	static char string[128];
	if (frame == 0) {
		FyTimerReset(0);
	}

	if ((frame & 15) == 15) {
		sprintf(string, "FPS: %6.2f %s", frame / FyTimerCheckTime(0), titleFlag ? "true" : "false");
	}

	frame += skip;
	if (frame >= 1000) {
		frame = 0;
	}

	FnText text;
	text.ID(textID);

	text.Begin(vID);
	text.Write(string, 20, 20, 255, 0, 0);

	// get camera's data
	char buff[128];
	// sprintf(buff, "CameraPosition: %8.3f %8.3f %8.3f", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	// text.Write(buff, 20, 35, 255, 255, 0);
	sprintf(buff, "ActorPosition: %8.3f %8.3f %8.3f", actorPosition[0], actorPosition[1], actorPosition[2]);
	text.Write(buff, 20, 50, 255, 255, 0);
	// sprintf(buff, "Camera: Zoom%8.3f  Height%8.3f  Angle%8.3f", cameraZoom, cameraHeight, cameraAngle);
	// text.Write(buff, 20, 65, 255, 255, 0);
	sprintf(buff, "movementspeed: %8.3f   dmgmultiplier: %8.3f   span: %d", Lvbu.movementspeed, Lvbu.dmgmultiplier, Lvbu.mono == NULL ? -1 : Lvbu.mono->span);
	text.Write(buff, 20, 80, 222, 111, 222);
	/*
	float pos[3];
	for (int i = 0; i < ZAKOMAX; i++) {
	zako[i].GetPosition(pos);
	char *s = zako[i].GetBodyName(0);
	sprintf(buff, "zako[%d].z= %8.3f, HP= %8.3f, curID= %s", i, pos[2], zako[i].healthpoint, s);
	text.Write(buff, 20, 95 + 15 * i, 255, 0, 0);
	}
	*/
	/*
	int yy = 0;
	for (auto& x : targetpool) {

	sprintf(buff, "donzoHP= %8.3f", x->healthpoint);
	text.Write(buff, 20, 280 + 15 * yy++, 100, 200, 255);
	}
	text.End();
	*/

	// swap buffer
	FySwapBuffers();
}

/*------------------
quit the demo
C.Wang 0327, 2005
-------------------*/
void QuitGame(BYTE code, BOOL4 value) {
	if (code == FY_ESCAPE && value) {
		FyQuitFlyWin32();
	}
}

void initKEY(){
	KEY_UP = false;
	KEY_DOWN = false;
	KEY_LEFT = false;
	KEY_RIGHT = false;
	KEY_W = false;
	KEY_A = false;
	KEY_S = false;
	KEY_D = false;
	KEY_Q = false;
	KEY_E = false;
	KEY_NUM8 = false;
	KEY_NUM2 = false;
	KEY_NUM4 = false;
	KEY_NUM6 = false;
	KEY_NUMADD = false;
	KEY_NUMMINUS = false;
	KEY_R = false;
}
