#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include "Scene.h"
#include "Mtx44.h"
#include "PlayerInfo/PlayerInfo.h"
#include "GroundEntity.h"
#include "FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "GenericEntity.h"
#include <vector>
#include "MyMath.h"
#include "MessageBoard.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class SpriteEntity;
class Light;

using namespace std;
class SceneText : public Scene
{
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	//AI Variables
	//Day Night Cycle
	bool DAY;
	bool NIGHT;
	double Time;

	struct Character {
	};

	enum Actions {
		IDLE, //all characters
		SLEEP, //female, male, cat
		SCREAM, //female
		PAT, //female
		CHASE, //cat
		ASKTOSLEEP, //cat
		WAKEUP, //cat
		HIDE, //mouse
		ROAM, //mouse
		TAKEDRINK // Male
	};

	Character Female;
	Character Male;
	Character Cat;
	Character Mouse;

	int FemaleState;
	int MaleState;
	int CatState;
	int MouseState;

	MessageBoard messageboard;

	int MousePos;
	Vector3 MouseNewPos;
	int MouseProb;
	bool MouseOut;
	bool MouseBack;
	float TryAgain;

	bool catgoingtosleep;
	double catmovetosleep;
	bool catalreadysleeping;

	float reactionTime;
	int patProb;
	bool pat;
	float pattingTime;
	bool seeMouse;
	double femaleaskingfordrinktime;
	bool femaleaskingsent;
	bool femalesent;
	double maletakingdrink;
	bool drinktaken;

private:
	SceneText(SceneManager* _sceneMgr); // This is used to register to SceneManager

	ShaderProgram* currProg;
	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera camera;
	TextEntity* textObj[4];
	Light* lights[2];

	vector <Vector3> wayPoints;

	GenericEntity* theCube;

	static SceneText* sInstance; // The pointer to the object that gets registered

	SpriteEntity* WorldObj[5];
	SpriteEntity* StatusBars[5];

	void FSMInit();
	void RunFSM(double dt);
	void FSMUpdate(double dt);
	void Respond();

	void RanMousePos();
	void MouseComeOut();
	void MouseRespond();
	void MouseFSMUpdate();

	void RanFemalePat();
	void FemaleRespond();
	void FemaleFSMUpdate();

	void CatRespond();
	void CatFSMUpdate();
	void ManRespond();
	void ManFSMUpdate();

	void MorningReset();
};

#endif
