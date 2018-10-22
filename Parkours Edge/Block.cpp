#include "Block.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "Mesh.h"
#include "CollisionComponent.h"



Block::Block(Game* game) : Actor(game)
{
	mMesh = new MeshComponent(this);
	mMesh->SetMesh(mGame->GetRenderer()->GetMesh("Assets/Cube.gpmesh"));

	mCollision = new CollisionComponent(this);
	mCollision->SetSize(1.0f, 1.0f, 1.0f); // Width, height, depth

	mScale = 64.0f;

	mGame->AddBlock(this);
}

Block::~Block() {
	mGame->RemoveBlock(this);
}
