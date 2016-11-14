#include "SpriteEntity.h"
#include "MeshBuilder.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

SpriteEntity::SpriteEntity(Mesh* _modelMesh) :
modelMesh(_modelMesh),
position(0.0f, 0.0f, 0.0f),
scale(1.0f, 1.0f, 1.0f),
mode(MODE_2D)
{
}

SpriteEntity::~SpriteEntity()
{
}

void SpriteEntity::Update(double _dt)
{
	// Does nothing here, can inherit & override or create your own version of this class :D
}

void SpriteEntity::Render()
{
	if (mode == MODE_2D)
		return;

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(scale.x, scale.y, scale.z);
	RenderHelper::RenderMesh(modelMesh);
	modelStack.PopMatrix();
}

void SpriteEntity::RenderUI()
{
	if (mode == MODE_3D)
		return;

	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(scale.x, scale.y, scale.z);
	RenderHelper::RenderMesh(modelMesh);
	modelStack.PopMatrix();
}

SpriteEntity* Create::Sprite2DObject(const std::string& _meshName, const Vector3& _position, const Vector3& _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	SpriteEntity* result = new SpriteEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetTextRenderMode(SpriteEntity::MODE_2D);
	EntityManager::GetInstance()->AddEntity(result);
	return result;
}

SpriteEntity* Create::Sprite3DObject(const std::string& _meshName, const Vector3& _position, const Vector3& _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	SpriteEntity* result = new SpriteEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetTextRenderMode(SpriteEntity::MODE_3D);
	EntityManager::GetInstance()->AddEntity(result);
	return result;
}

float GetDistance(float x1, float y1, float x2, float y2) { return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); } // OK

void SpriteEntity::MovePos(Vector3 finalpos, float speed)
{
	float distance = GetDistance(this->GetPosition().x, this->GetPosition().y, finalpos.x, finalpos.y);
	if (distance > 0.01f)
	{
		Vector3 currentPos;
		currentPos = Vector3(this->GetPosition().x, this->GetPosition().y, 1);
		Vector3 direction;
		direction = (finalpos - currentPos).Normalized();
		direction = Vector3(direction.x, direction.y, 0);
		this->SetPosition(currentPos + direction * speed);
	}
}

bool SpriteEntity::ReachPos(Vector3 finalpos)
{
	float distance = GetDistance(this->GetPosition().x, this->GetPosition().y, finalpos.x, finalpos.y);
	if (distance < 0.01f)
	{
		return true;
	}
	return false;
}
