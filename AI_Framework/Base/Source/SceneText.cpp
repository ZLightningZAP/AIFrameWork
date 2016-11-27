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

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	//Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));
	Create::Sprite2DObject("Background", Vector3(0.0f, 0.0f, 0.0f), Vector3(800.0f, 600.0f, 0.0f));
	WorldObj[1] = Create::Sprite2DObject("Male", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 50.0f));
	WorldObj[2] = Create::Sprite2DObject("Female", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 50.0f));
	WorldObj[3] = Create::Sprite2DObject("Cat", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 50.0f));
	WorldObj[4] = Create::Sprite2DObject("Mouse", Vector3(0.0f, 0.0f, 1.0f), Vector3(50.0f, 50.0f, 50.0f));

	FSMInit();

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	//textObj[0] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize + halfFontSize, 1.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	//textObj[1] = Create::Text2DObject("mouse state", Vector3(WorldObj[4]->GetPosition().x, WorldObj[4]->GetPosition().y + 2, WorldObj[4]->GetPosition().z), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < 3; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(0, -halfWindowHeight + fontSize*i + halfFontSize, 1.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
		//textObj[i]->SetColor(Color(0, 0, 0));
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
	TVon = false;
	Time = 0;
	TimePast = 0;

	//Waypoint Init
	//Starting Position[0]
	wayPoints.push_back(Vector3(1, 1, 1));
	//Bedroom[1]
	wayPoints.push_back(Vector3(0, 220, 1));
	//////////////////////////////////////////////////////////////////
	//Toilet[2]
	wayPoints.push_back(Vector3(150, 80, 1));
	//Pet Room[3]
	wayPoints.push_back(Vector3(-150, 80, 1));
	//Before Toilet/Pet Room[4]
	wayPoints.push_back(Vector3(0, 80, 1));
	//////////////////////////////////////////////////////////////////
	//Couch[5]
	wayPoints.push_back(Vector3(-120, -63, 1));
	//Kitchen[6]
	wayPoints.push_back(Vector3(300, -63, 1));
	//Before Kitchen/Couch[7]
	wayPoints.push_back(Vector3(0, -63, 1));
	//////////////////////////////////////////////////////////////////
	//Outside[8]
	wayPoints.push_back(Vector3(450, -230, 1));
	//Before going outside[9]
	wayPoints.push_back(Vector3(0, -230, 1));
	//Mouse Hole [10]
	wayPoints.push_back(Vector3(-380, -150, 1));
	//Infront of TV [11]
	wayPoints.push_back(Vector3(-120, -43, 1));

	//Male Init
	WorldObj[1]->SetPosition(wayPoints[1]);
	Male.m_bowel = 0;
	Male.m_hunger = 100;

	//Female Init
	WorldObj[2]->SetPosition(wayPoints[0]);
	FemaleState = IDLE;
	Female.m_bowel = 0;
	Female.m_entertain = 100;
	Female.m_hunger = 0;

	//Mouse Init
	WorldObj[4]->SetPosition(wayPoints[10]);
	MouseState = HIDE;
	RanMousePos();
	Mouse.m_hunger = 0;

	//Cat Init
	CatState = IDLE;
	WorldObj[3]->SetPosition(wayPoints[5]);
	Cat.m_hunger = 100;
	Cat.m_bowel = 0;
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

	TimePast += dt;

	//Female Stats control
	if (Female.m_bowel >= 100)
	{
		Female.m_bowel = 100;
	}
	else if (Female.m_bowel <= 0)
	{
		Female.m_bowel = 0;
	}

	if (Female.m_entertain >= 100)
	{
		Female.m_entertain = 100;
	}
	else if (Female.m_entertain <= 0)
	{
		Female.m_entertain = 0;
	}

	if (Female.m_hunger >= 100)
	{
		Female.m_hunger = 100;
	}
	else if (Female.m_hunger <= 0)
	{
		Female.m_hunger = 0;
	}

	//Mouse Stats control
	if (Mouse.m_hunger >= 100)
	{
		Mouse.m_hunger = 100;
	}
	else if (Mouse.m_hunger <= 0)
	{
		Mouse.m_hunger = 0;
	}

	//Every 5s
	if (TimePast >= 5)
	{
		//Female stats
		if (FemaleState == WATCH && WorldObj[2]->ReachPos(wayPoints[5]))
		{
			Female.m_entertain += 20;
		}
		else
		{
			Female.m_entertain -= 2;
		}

		if (FemaleState == SHIT && WorldObj[2]->ReachPos(wayPoints[2]))
		{
			Female.m_bowel -= 30;
		}

		if (FemaleState == EAT)
		{
			if (WorldObj[2]->ReachPos(wayPoints[6]))
			{
				Female.m_bowel += 10;
				Female.m_hunger -= 10;
			}
		}
		else
		{
			Female.m_hunger += 2;
		}

		//Mouse stats
		if (MouseState == EAT)
		{
			Mouse.m_hunger -= 10;
		}
		else
		{
			Mouse.m_hunger += 5;
		}

		//Cat stats
		if (CatState == EAT)
			Cat.m_hunger += 20;
		else
			Cat.m_hunger -= 10;

		if (CatState == SHIT)
			Cat.m_bowel -= 45;
		else
			Cat.m_bowel += 15;

		//Male Stats

		TimePast = 0;
	}
}

void SceneText::MouseFSMUpdate()
{
	switch (MouseState)
	{
	case ROAM:
		if (DAY)
			MouseState = HIDE;
		else
		{
			if (Mouse.m_hunger >= 80)
				MouseState = EAT;
		}
		break;

	case HIDE:
		if (NIGHT)
			MouseState = ROAM;
		break;

	case EAT:
		if (DAY)
			MouseState = HIDE;
		else
		{
			if (Mouse.m_hunger < 50)
				MouseState = ROAM;
		}
		break;
	}
}

void SceneText::MouseRespond()
{
	MouseFSMUpdate();
	switch (MouseState)
	{
	case ROAM:
		if (WorldObj[4]->ReachPos(MouseNewPos))
		{
			RanMousePos();
		}
		else
		{
			WorldObj[4]->MovePos(MouseNewPos, 1);
		}
		break;

	case HIDE:
		WorldObj[4]->MovePos(wayPoints[10], 3);
		break;

	case EAT:
		WorldObj[4]->MovePos(wayPoints[6], 2);
	}
}

void SceneText::FemaleFSMUpdate()
{
	//importance of action: SHIT->EAT->CLEAN->WATCH
	switch (FemaleState)
	{
	case IDLE:
		if (DAY)
		{
			if (Female.m_bowel >= 80 && Female.m_hunger < 100) //Female not dying of hunger
			{
				FemaleState = SHIT;
			}

			if (Female.m_hunger >= 80 && Female.m_bowel < 100) //Female not exploding with shit
			{
				FemaleState = EAT;
			}

			if (Female.m_entertain <= 40 && Female.m_bowel < 80 && Female.m_hunger < 80) //Female's hunger and bowel are normal
			{
				FemaleState = WATCH;
			}
		}
		else
			FemaleState = SLEEP;
		break;

	case EAT:
		if (DAY)
		{
			if (Female.m_bowel >= 80 && Female.m_bowel > Female.m_hunger) //Female needs to go to the toilet more than she needs to eat
			{
				FemaleState = SHIT;
			}

			if (Female.m_entertain <= 40 && Female.m_hunger <= 20 && Female.m_bowel < 80) //Female's hunger and bowel are normal, needs entertainment
			{
				FemaleState = WATCH;
			}

			if (Female.m_bowel < 80 && Female.m_entertain > 40 && Female.m_hunger < 80) //Female's stats are normal
			{
				FemaleState = IDLE;
			}
		}
		else
			if (Female.m_hunger < 100) //Female not dying from hunger
				FemaleState = SLEEP;
		break;

	case SHIT:
		if (Female.m_bowel < 80) //Female's bowel is normal
		{
			if (DAY)
			{
				if (Female.m_hunger >= 80) //Hungry
				{
					FemaleState = EAT;
				}
				else //Not hungry
				{
					if (Female.m_entertain <= 40) //Bored
					{
						FemaleState = WATCH;
					}
				}

				if (Female.m_bowel < 80 && Female.m_entertain > 40) //Female's stats are normal
				{
					FemaleState = IDLE;
				}
			}

			else
			{
				FemaleState = SLEEP;
			}
		}
		break;

	case WATCH:
		if (DAY)
		{
			if (Female.m_hunger >= 80 || Female.m_bowel >= 80)
			{
				if (Female.m_bowel > Female.m_hunger) //see if Female is more urgent to eat or go to the toilet
					FemaleState = SHIT;
				else
					FemaleState = EAT;
			}
			else
			{
				if (Female.m_entertain > 40) // Female's stats are back to normal
					FemaleState = IDLE;
			}
		}
		else
			FemaleState = SLEEP;
		break;

	case SLEEP:
		if (DAY)
			FemaleState = IDLE;
		break;
	}
}

void SceneText::FemaleRespond()
{
	FemaleFSMUpdate();

	if (FemaleState == WATCH)
	{
		if (TVon)
		{
			WorldObj[2]->MovePos(wayPoints[5], 1); //tv is on, go to couch
		}
		else
		{
			WorldObj[2]->MovePos(wayPoints[11], 1); //on tv
			TVon = true;
		}
	}
	else //not watching tv
	{
		if (TVon) //tv is on
		{
			WorldObj[2]->MovePos(wayPoints[11], 1); //off tv
			TVon = false;
		}
		else //tv is off
		{
			switch (FemaleState)
			{
			case IDLE:
				if (WorldObj[2]->GetPosition() != wayPoints[5])
				{
					//move to couch to wait for something to do
					WorldObj[2]->MovePos(wayPoints[5], 1);
				}
				break;

			case EAT:
				WorldObj[2]->MovePos(wayPoints[6], 1);
				break;

			case SHIT:
				WorldObj[2]->MovePos(wayPoints[2], 1);
				break;

			case SLEEP:
				WorldObj[2]->MovePos(wayPoints[1], 1);
				break;
			}
		}
	}
}

void SceneText::CatRespond()
{
	CatFSMUpdate();
	switch (CatState)
	{
	case IDLE:
		//Go to couch
		WorldObj[3]->MovePos(wayPoints[5], 2);
		break;
	case EAT:
		//Go to kitchen
		WorldObj[3]->MovePos(wayPoints[6], 2);
		break;
	case SLEEP:
		//Go to pet room
		WorldObj[3]->MovePos(wayPoints[3], 2);
		break;
	case SHIT:
		//Go to toilet
		WorldObj[3]->MovePos(wayPoints[2], 2);
		break;
	}
}

void SceneText::CatFSMUpdate()
{
	if (DAY == true)
	{
		switch (CatState)
		{
		case IDLE:
			if (Cat.m_hunger <= 90)
				CatState = EAT;
			else if (Cat.m_bowel >= 45)
				CatState = SHIT;
			break;
		case EAT:
			if (Cat.m_hunger >= 100)
				CatState = IDLE;
			break;
		case SLEEP:
			//Dont do anything while sleeping
			break;
		case SHIT:
			if (Cat.m_bowel <= 0)
				CatState = IDLE;
			break;
		}
	}
	else if (NIGHT == true)
	{
		CatState = SLEEP;
		Cat.m_hunger = 100;
		Cat.m_bowel = 0;
	}
}

void SceneText::ManRespond()
{
	ManFSMUpdate();
	switch (MaleState)
	{
	case IDLE:
		//Before Kitchen/Couch
		WorldObj[1]->MovePos(wayPoints[7], 2);
		break;
	case EAT:
		//Go to kitchen
		WorldObj[1]->MovePos(wayPoints[6], 2);
		break;
	case SLEEP:
		//Go to bedroom
		WorldObj[1]->MovePos(wayPoints[1], 2);
		break;
	case SHIT:
		//Go to toilet
		WorldObj[1]->MovePos(wayPoints[2], 2);
		break;
	case WORK:
		//Go to work
		WorldObj[1]->MovePos(wayPoints[2], 2);
		break;
	}
}

void SceneText::ManFSMUpdate()
{
	//If its in the morning
	if (DAY == true)
	{
		//#Logic
		switch (MaleState)
		{
		case IDLE:
			break;
		case EAT:
			break;
		case SLEEP:
			break;
		case SHIT:
			break;
		case WORK:
			break;
		}
	}
	//If it's at night
	else if (NIGHT == true)
	{
		switch (MaleState)
		{
		case IDLE:
			break;
		case EAT:
			break;
		case SLEEP:
			break;
		case SHIT:
			break;
		case WORK:
			//AI came back from work
			MaleState = IDLE;
			break;
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
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

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

	//Mouse stats
	std::ostringstream s1;
	if (MouseState == ROAM)
	{
		textObj[1]->SetColor(Color(0.0f, 0.0f, 0.0f));
		s1 << "Roam";
	}
	else if (MouseState == EAT)
		s1 << "Eat";

	else if (MouseState == HIDE)
		s1 << "Hide";

	textObj[1]->SetText(s1.str());
	//textObj[1]->SetPosition(Vector3(WorldObj[4]->GetPosition().x, WorldObj[4]->GetPosition().y + 2, WorldObj[4]->GetPosition().z));

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