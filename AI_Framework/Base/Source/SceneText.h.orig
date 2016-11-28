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
	//if TV is on
	bool TVon;
	double TimePast;

	struct Character{
		int m_hunger;
		int m_bowel;
		int m_entertain;
	};

	struct Cleaniness{
		int r_bedroom;
		int r_pet;
		int r_kitchen;
		int r_toilet;
	};

	enum Actions{
		IDLE, //all characters
		EAT, //all characters
		SHIT, //female, male, cat
		SLEEP, //female, male, cat
		WORK, //male
		CLEAN, //female
		WATCH, //female
		HIDE, //mouse
		ROAM //mouse
	};

	Character Female;
	Character Male;
	Character Cat;
	Character Mouse;

	int FemaleState;
	int MaleState;
	int CatState;
	int MouseState;

<<<<<<< HEAD
=======
	bool MaleWork;
	bool MaleSleep;

	static const int IDLE = 0; //all characters
	static const int EAT = 1; //all characters
	static const int SHIT = 2; //female, male, cat
	static const int SLEEP = 3; //female, male, cat
	static const int WORK = 4; //male
	static const int CLEAN = 5; //female
	static const int WATCH = 6; //female
	static const int HIDE = 7; //mouse
	static const int ROAM = 8; //mouse

>>>>>>> 746480ac5855e2cdbdf26d7c68b75ad7c1d071e2
	int MousePos;
	Vector3 MouseNewPos;

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
	SpriteEntity* StatusBars[4];

	void FSMInit();
	void RunFSM(double dt);
	void FSMUpdate(double dt);
	void Respond();

	void RanMousePos();
	void MouseRespond();
	void MouseFSMUpdate();
	void FemaleRespond();
	void FemaleFSMUpdate();

	void CatRespond();
	void CatFSMUpdate();
	void ManRespond();
	void ManFSMUpdate();
};

#endif
