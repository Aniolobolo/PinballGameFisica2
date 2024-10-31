#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;


class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	std::vector<PhysicEntity*> entities;
	
	PhysBody* sensor;
	bool sensed;

	Texture2D circle;
	Texture2D box;
	Texture2D rick;
	Texture2D rick2;
	Texture2D rick3;
	Texture2D rick4, rick5, rick6; 
	Texture2D GreenEvoDer; //colision de la derecha del evo green

	uint32 bonus_fx;
	Music backgroundMusic;
	vec2<int> ray;
	bool ray_on;
};
