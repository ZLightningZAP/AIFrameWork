#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"

#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");

	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");

	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);

	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	//playerInfo->AttachCamera(&camera);

	//GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);

	//AI Load Image
	MeshBuilder::GetInstance()->GenerateQuad("Background", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Background")->textureID = LoadTGA("Image//RoomLayout.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Male", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Male")->textureID = LoadTGA("Image//Male.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Female", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Female")->textureID = LoadTGA("Image//Female.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Cat", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Cat")->textureID = LoadTGA("Image//Cat.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Mouse", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("Mouse")->textureID = LoadTGA("Image//Mouse.tga");

	//Status Bar Image
	MeshBuilder::GetInstance()->GenerateQuad("StatusBar", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("StatusBar")->textureID = LoadTGA("Image//StatusBar.tga");

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	//Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
	Create::Sprite2DObject("Background", Vector3(-400.0f, 0.0f, 0.0f), Vector3(800.0f, 600.0f, 0.0f));
	WorldObj[1] = Create::Sprite2DObject("Male", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 1.0f));
	WorldObj[2] = Create::Sprite2DObject("Female", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 1.0f));
	WorldObj[3] = Create::Sprite2DObject("Cat", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 1.0f));
	WorldObj[4] = Create::Sprite2DObject("Mouse", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 1.0f));

	//Creating the status bar for each AI
	for (int i = 0; i < 4; ++i)
	{
		StatusBars[i] = Create::Sprite2DObject("StatusBar", Vector3(0.0f, 0.0f, -1.0f), Vector3(130.0f, 25.0f, 1.0f));
	}

	FSMInit();

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 24.0f;
	float halfFontSize = fontSize / 2.0f;
	//textObj[0] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize + halfFontSize, 1.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	//textObj[1] = Create::Text2DObject("mouse state", Vector3(WorldObj[4]->GetPosition().x, WorldObj[4]->GetPosition().y + 2, WorldObj[4]->GetPosition().z), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < 6; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	}
	//textObj[0]->SetText("HELLO WORLD");
}

//All AI Init Here
void SceneText::FSMInit()
{
	//Day/Night Cycle
	DAY = true;
	NIGHT = false;
	Time = 0;

	//Mouse variables initialize
	MouseOut = false;
	MouseProb = 0;
	MouseBack = false;
	TryAgain = 0;

	//Cat variables initialize
	catgoingtosleep = false;
	catmovetosleep = 0;
	catalreadysleeping = false;

	//Female variables initialize
	reactionTime = 0;
	pattingTime = 0;
	patProb = 0;
	pat = false;
	seeMouse = false;
	femaleaskingfordrinktime = 0;
	femaleaskingsent = false;
	femalesent = false;
	maletakingdrink = 0;
	drinktaken = false;

	//Waypoint Init
	//Starting Position[0]
	wayPoints.push_back(Vector3(1, 1, 1));
	//Male position on the bed [1]
	wayPoints.push_back(Vector3(-400, 250, 1));
	//////////////////////////////////////////////////////////////////
	//Toilet[2]
	wayPoints.push_back(Vector3(-250, 80, 1));
	//Pet Room[3]
	wayPoints.push_back(Vector3(-550, 80, 1));
	//Before Toilet/Pet Room[4]
	wayPoints.push_back(Vector3(-400, 80, 1));
	//////////////////////////////////////////////////////////////////
	//Couch[5]
	wayPoints.push_back(Vector3(-520, -63, 1));
	//Kitchen[6]
	wayPoints.push_back(Vector3(-100, -63, 1));
	//Before Kitchen/Couch[7]
	wayPoints.push_back(Vector3(-400, -63, 1));
	//////////////////////////////////////////////////////////////////
	//Outside[8]
	wayPoints.push_back(Vector3(50, -230, 1));
	//Before going outside[9]
	wayPoints.push_back(Vector3(-400, -230, 1));
	//Mouse Hole [10]
	wayPoints.push_back(Vector3(-780, -150, 1));
	//Infront of TV [11]
	wayPoints.push_back(Vector3(-520, -43, 1));
	//Female side of couch [12]
	wayPoints.push_back(Vector3(-680, -63, 1));
	//Female side of bed [13]
	wayPoints.push_back(Vector3(-500, 250, 1));
	//Female side of couch [14]
	wayPoints.push_back(Vector3(-630, -280, 1));
	//Cat asking for bed [15]
	wayPoints.push_back(Vector3(-580, 220, 1));
	//Cat position on bed [16]
	wayPoints.push_back(Vector3(-450, 190, 1));
	//Cat position on the couch [17]
	wayPoints.push_back(Vector3(-600, -63, 1));
	//Female Patting Position [18]
	wayPoints.push_back(Vector3(-640, -63, 1));

	//Male Init
	WorldObj[1]->SetPosition(wayPoints[5]);
	MaleState = IDLE;

	//Female Init
	WorldObj[2]->SetPosition(wayPoints[12]);
	FemaleState = IDLE;

	//Mouse Init
	WorldObj[4]->SetPosition(wayPoints[10]);
	MouseState = IDLE;
	RanMousePos();

	//Cat Init
	CatState = IDLE;
	WorldObj[3]->SetPosition(wayPoints[17]);
}

//Ai FSM Update Here
void SceneText::FSMUpdate(double dt)
{
	RunFSM(dt);
	Respond();
}

//Conditions = Logic here
//Run in real time
void SceneText::RunFSM(double dt)
{
	Time += dt;
	if (Time >= 30)
	{
		if (DAY == true)
		{
			DAY = false;
			NIGHT = true;
		}
		else if (NIGHT == true)
		{
			MorningReset();
			DAY = true;
			NIGHT = false;
		}
		Time = 0;
	}

	if (MouseOut == true)
	{
		reactionTime += dt;
	}

	if (reactionTime > 10 && seeMouse == false)
	{
		seeMouse = true;
	}

	if (MouseBack)
	{
		TryAgain += dt;
	}

	if (FemaleState == PAT)
	{
		pattingTime += dt;
	}

	//Timer for cat to move to sleeping to sleep
	if (catgoingtosleep == true)
	{
		catmovetosleep += dt;
		if (catmovetosleep >= 5 && catalreadysleeping == false)
		{
			CatState = SLEEP;
			catmovetosleep = 0;
			catalreadysleeping = true;
			messageboard.Reset();
		}
	}
	if (NIGHT == true)
	{
		if (femalesent == false)
		{
			femaleaskingfordrinktime += dt;
			if (femaleaskingfordrinktime >= 10 && femaleaskingsent == false)
			{
				femaleaskingsent = true;
			}
		}
		if (MaleState == TAKEDRINK)
		{
			maletakingdrink += dt;
			if (maletakingdrink >= 5 && drinktaken == false)
			{
				MaleState = SLEEP;
				drinktaken = true;
				messageboard.Reset();
			}
		}
	}
}

void SceneText::MorningReset()
{
	//Cat variables initialize
	catgoingtosleep = false;
	catmovetosleep = 0;
	catalreadysleeping = false;

	//Female variables initialize
	femaleaskingfordrinktime = 0;
	femaleaskingsent = false;
	femalesent = false;
	maletakingdrink = 0;
	drinktaken = false;

	//Mouse
	MouseState = IDLE;
}

void SceneText::RanMousePos()
{
	//Randomise where mouse goes
	MousePos = rand() % 3;
	switch (MousePos)
	{
	case 0:
		MouseNewPos = wayPoints[0];
		break;
	case 1:
		MouseNewPos = wayPoints[4];
		break;
	case 2:
		MouseNewPos = wayPoints[5];
		break;
	case 3:
		MouseNewPos = wayPoints[9];
		break;
	}
}

void SceneText::MouseComeOut()
{
	MouseProb = rand() % 10;
	if (MouseProb > 4)
	{
		MouseOut = true;
	}
	else
	{
		MouseOut = false;
	}
}

void SceneText::MouseFSMUpdate()
{
	if (DAY)
	{
		if (MouseOut)
		{
			if (messageboard.GetMsg() == "SCREAM")
			{
				MouseState = HIDE;
			}
			else
			{
				MouseState = ROAM;
			}
		}
		if (MouseBack && TryAgain >= 10)
		{
			MouseState = IDLE;
			MouseBack = false;
			TryAgain = 0;
		}
	}
	else
	{
		MouseState = ROAM;
	}
}

void SceneText::MouseRespond()
{
	MouseFSMUpdate();
	switch (MouseState)
	{
	case IDLE:
	{
		MouseComeOut();
	}
	break;

	case ROAM:
	{
		if (WorldObj[4]->ReachPos(MouseNewPos))
		{
			RanMousePos();
		}
		else
		{
			WorldObj[4]->MovePos(MouseNewPos, 1);
		}
	}
	break;

	case HIDE:
	{
		if (WorldObj[4]->ReachPos(wayPoints[10]))
		{
			if (MouseBack == false)
			{
				seeMouse = false;
				MouseOut = false;
				reactionTime = 0;
				messageboard.Reset(); //reset screaming
				MouseBack = true;
			}
		}
		else
		{
			WorldObj[4]->MovePos(wayPoints[10], 3);
		}
	}
	break;
	}
}

void SceneText::RanFemalePat()
{
	patProb = rand() % 10;
	if (patProb >= 7)
	{
		pat = true;
	}
	else
	{
		pat = false;
	}
}

void SceneText::FemaleFSMUpdate()
{
	//First FSM
	if (DAY == true)
	{
		if (MouseState == ROAM && seeMouse == true)
		{
			messageboard.SetMessage("SCREAM");
			messageboard.SetFromLabel("Woman");
			messageboard.SetToLabel("Cat");
		}
		else
		{
			FemaleState = IDLE;
			if (messageboard.GetMsg() == "Meow")
			{
				RanFemalePat();
				if (pat)
				{
					FemaleState = PAT;
				}
				else
				{
					FemaleState = IDLE;
				}
			}
		}
	}
	//Second FSM
	if (NIGHT == true)
	{
		FemaleState = SLEEP;
		if (messageboard.GetMsg() == "I Want To Sleep Here" && catgoingtosleep == false)
		{
			catgoingtosleep = true;
		}

		if (femaleaskingsent == true && femalesent == false)
		{
			messageboard.SetMessage("Honey, i want a drink");
			messageboard.SetFromLabel("Woman");
			messageboard.SetToLabel("Man");
			femalesent = true;
		}
	}
}

void SceneText::FemaleRespond()
{
	FemaleFSMUpdate();
	switch (FemaleState)
	{
	case IDLE:
		WorldObj[2]->MovePos(wayPoints[12], 2);
		break;
	case SLEEP:
		WorldObj[2]->MovePos(wayPoints[13], 2);
		break;
	case PAT:
	{
		if (WorldObj[2]->ReachPos(wayPoints[18]))
		{
			if (pattingTime >= 2)
			{
				pattingTime = 0;
				messageboard.Reset(); //reset patting
			}
		}
		else
			WorldObj[2]->MovePos(wayPoints[18], 2);
	}
	break;
	default:
		break;
	}
}

void SceneText::CatRespond()
{
	CatFSMUpdate();
	switch (CatState)
	{
	case ASKTOSLEEP:
		//Move beside bed
		WorldObj[3]->MovePos(wayPoints[15], 2);
		break;
	case SLEEP:
		// Move onto bed
		WorldObj[3]->MovePos(wayPoints[16], 2);
		break;
	case WAKEUP:
		catgoingtosleep = false;
		catmovetosleep = 0;
		catalreadysleeping = false;
		//Move to couch
		WorldObj[3]->MovePos(wayPoints[17], 2);
		break;
	case CHASE:
		WorldObj[3]->MovePos(WorldObj[4]->GetPosition(), 1);
		break;
	default:
		break;
	}
}

void SceneText::CatFSMUpdate()
{
	if (DAY == true)
	{
		if (messageboard.GetMsg() == "SCREAM")
		{
			CatState = CHASE;
		}
		else
		{
			CatState = WAKEUP;
			if (MouseBack)
			{
				messageboard.SetMessage("Meow");
				messageboard.SetFromLabel("Cat");
				messageboard.SetToLabel("Female");
			}
		}
	}
	if (NIGHT == true)
	{
		if (catgoingtosleep == false && catalreadysleeping == false)
		{
			CatState = ASKTOSLEEP;
			messageboard.SetMessage("I Want To Sleep Here");
			messageboard.SetFromLabel("Cat");
			messageboard.SetToLabel("Female");
		}
	}
}

void SceneText::ManRespond()
{
	ManFSMUpdate();
	switch (MaleState)
	{
	case IDLE:
		WorldObj[1]->MovePos(wayPoints[5], 2);
		break;
	case SLEEP:
		WorldObj[1]->MovePos(wayPoints[1], 2);
		break;
	case TAKEDRINK:
		WorldObj[1]->MovePos(wayPoints[6], 2);
		break;
	default:
		break;
	}
}

void SceneText::ManFSMUpdate()
{
	// During the day time, male will be sitting on the couch
	if (DAY == true)
	{
		MaleState = IDLE;
	}
	if (NIGHT == true)
	{
		MaleState = SLEEP;
		if (messageboard.GetMsg() == "Honey, i want a drink")
		{
			MaleState = TAKEDRINK;
		}
	}
}

//How the AI should respond + Effects will be seen
void SceneText::Respond()
{
	MouseRespond();
	FemaleRespond();
	CatRespond();
	ManRespond();
}

void SceneText::Update(double dt)
{
	//Day/Night Cycle
	std::ostringstream ss;
	if (DAY == true)
	{
		ss << "Status:DAY";
	}
	else if (NIGHT == true)
	{
		ss << "Status:NIGHT";
	}
	textObj[0]->SetText(ss.str());
	textObj[0]->SetPosition(Vector3(0, 280, 2));

	//Message Board
	std::ostringstream s1;
	s1 << "MessageBoard:" << messageboard.GetMsg();
	textObj[1]->SetText(s1.str());
	textObj[1]->SetPosition(Vector3(StatusBars[2]->GetPosition().x, StatusBars[2]->GetPosition().y + 240, StatusBars[2]->GetPosition().z + 2));

	std::ostringstream s2;
	s2 << "From:" << messageboard.GetFromLabel();
	textObj[2]->SetText(s2.str());
	textObj[2]->SetPosition(Vector3(StatusBars[2]->GetPosition().x, StatusBars[2]->GetPosition().y + 200, StatusBars[2]->GetPosition().z + 2));

	std::ostringstream s3;
	s3 << "To:" << messageboard.GetToLabel();
	textObj[3]->SetText(s3.str());
	textObj[3]->SetPosition(Vector3(StatusBars[2]->GetPosition().x, StatusBars[2]->GetPosition().y + 160, StatusBars[2]->GetPosition().z + 2));

	FSMUpdate(dt);
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE ==1

		cout << "Unable to drop PlayerInfo class" << endl;
#endif
}
	// Delete the lights
	delete lights[0];
}