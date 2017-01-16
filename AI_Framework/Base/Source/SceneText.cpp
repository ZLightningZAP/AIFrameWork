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
	Create::Sprite2DObject("Background", Vector3(-300.0f, 0.0f, 0.0f), Vector3(800.0f, 600.0f, 0.0f));
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
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	//textObj[0] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize + halfFontSize, 1.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	//textObj[1] = Create::Text2DObject("mouse state", Vector3(WorldObj[4]->GetPosition().x, WorldObj[4]->GetPosition().y + 2, WorldObj[4]->GetPosition().z), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < 6; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize*i + halfFontSize, 1.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	}
	//textObj[0]->SetText("HELLO WORLD");
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

//All AI Init Here
void SceneText::FSMInit()
{
	//Day/Night Cycle
	DAY = true;
	NIGHT = false;
	Time = 0;

	//Waypoint Init
	//Starting Position[0]
	wayPoints.push_back(Vector3(1, 1, 1));
	//Bedroom[1]
	wayPoints.push_back(Vector3(-300, 220, 1));
	//////////////////////////////////////////////////////////////////
	//Toilet[2]
	wayPoints.push_back(Vector3(-150, 80, 1));
	//Pet Room[3]
	wayPoints.push_back(Vector3(-450, 80, 1));
	//Before Toilet/Pet Room[4]
	wayPoints.push_back(Vector3(-300, 80, 1));
	//////////////////////////////////////////////////////////////////
	//Couch[5]
	wayPoints.push_back(Vector3(-420, -63, 1));
	//Kitchen[6]
	wayPoints.push_back(Vector3(0, -63, 1));
	//Before Kitchen/Couch[7]
	wayPoints.push_back(Vector3(-300, -63, 1));
	//////////////////////////////////////////////////////////////////
	//Outside[8]
	wayPoints.push_back(Vector3(150, -230, 1));
	//Before going outside[9]
	wayPoints.push_back(Vector3(-300, -230, 1));
	//Mouse Hole [10]
	wayPoints.push_back(Vector3(-680, -150, 1));
	//Infront of TV [11]
	wayPoints.push_back(Vector3(-420, -43, 1));
	//Female side of couch [12]
	wayPoints.push_back(Vector3(-580, -63, 1));
	//Female side of bed [13]
	wayPoints.push_back(Vector3(-420, 220, 1));
	//Female side of couch [14]
	wayPoints.push_back(Vector3(-530, -280, 1));

	//Male Init
	WorldObj[1]->SetPosition(wayPoints[1]);
	MaleState = IDLE;

	//Female Init
	WorldObj[2]->SetPosition(wayPoints[12]);
	FemaleState = IDLE;

	//Mouse Init
	WorldObj[4]->SetPosition(wayPoints[10]);
	MouseState = HIDE;
	RanMousePos();

	//Cat Init
	CatState = IDLE;
	WorldObj[3]->SetPosition(wayPoints[5]);
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
			DAY = true;
			NIGHT = false;
		}
		Time = 0;
	}
}

void SceneText::MouseFSMUpdate()
{
}

void SceneText::MouseRespond()
{
	MouseFSMUpdate();

	//StatusBars[2]->SetPosition(Vector3(WorldObj[4]->GetPosition().x, WorldObj[4]->GetPosition().y + 45, WorldObj[4]->GetPosition().z));
}

void SceneText::FemaleFSMUpdate()
{
}

void SceneText::FemaleRespond()
{
	FemaleFSMUpdate();

	//StatusBars[3]->SetPosition(Vector3(WorldObj[2]->GetPosition().x, WorldObj[2]->GetPosition().y + 45, WorldObj[2]->GetPosition().z));
}

void SceneText::CatRespond()
{
	CatFSMUpdate();

	//StatusBars[0]->SetPosition(Vector3(WorldObj[3]->GetPosition().x, WorldObj[3]->GetPosition().y + 45, WorldObj[3]->GetPosition().z));
}

void SceneText::CatFSMUpdate()
{
}

void SceneText::ManRespond()
{
	ManFSMUpdate();

	//StatusBars[1]->SetPosition(Vector3(WorldObj[1]->GetPosition().x, WorldObj[1]->GetPosition().y + 45, WorldObj[1]->GetPosition().z));
}

void SceneText::ManFSMUpdate()
{
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
	// Update our entities
	//EntityManager::GetInstance()->Update(dt);

	// Update the player position and other details based on keyboard and mouse inputs
	//playerInfo->Update(dt);

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)
	//GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values.
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	//std::ostringstream ss;
	//ss.precision(5);
	//float fps = (float)(1.f / dt);
	//ss << "FPS: " << fps;
	//textObj[1]->SetText(ss.str());

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
	textObj[0]->SetPosition(Vector3(-275, -285, 1));

	//Message Board
	/*std::ostringstream s1;
	s1 << "Message Board : " << messageboard.GetMsg();
	textObj[1]->SetText(s1.str());
	textObj[1]->SetPosition(Vector3(StatusBars[2]->GetPosition().x - 35, StatusBars[2]->GetPosition().y, StatusBars[2]->GetPosition().z + 1));*/

	//Mouse stats
	//std::ostringstream s1;
	//if (MouseState == ROAM)
	//	s1 << "Roam";

	//textObj[1]->SetText(s1.str());
	//textObj[1]->SetPosition(Vector3(StatusBars[2]->GetPosition().x - 35, StatusBars[2]->GetPosition().y, StatusBars[2]->GetPosition().z + 1));

	////Female stats
	//std::ostringstream s4;
	//if (FemaleState == IDLE)
	//	s4 << "Idle";

	//textObj[4]->SetText(s4.str());
	//textObj[4]->SetPosition(Vector3(StatusBars[3]->GetPosition().x - 45, StatusBars[3]->GetPosition().y, StatusBars[3]->GetPosition().z + 1));

	////Cat Stats
	//std::ostringstream s2;
	//if (CatState == IDLE)
	//	s2 << "Idle";
	////Update position of the string with the status bar
	//textObj[2]->SetText(s2.str());
	//textObj[2]->SetPosition(Vector3(StatusBars[0]->GetPosition().x - 45, StatusBars[0]->GetPosition().y, StatusBars[0]->GetPosition().z + 1));

	////Male Stats
	//std::ostringstream s3;
	//if (MaleState == IDLE)
	//	s3 << "Idle";
	////Update position of the string with the status bar
	//textObj[3]->SetText(s3.str());
	//textObj[3]->SetPosition(Vector3(StatusBars[1]->GetPosition().x - 45, StatusBars[1]->GetPosition().y, StatusBars[1]->GetPosition().z + 1));

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