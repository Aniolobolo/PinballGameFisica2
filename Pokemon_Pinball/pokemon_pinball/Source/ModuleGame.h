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

	int suma = 0;

	Texture2D circle;
	Texture2D box;
	Texture2D leftPad;
	Texture2D rightPad;
	Texture2D collision1;
	Texture2D collision2;
	Texture2D collision3;
	Texture2D collision4, collision5, collision6; 
	Texture2D GreenEvoDer; //colision de la derecha del evo green
	Texture2D GreenOneIzq;
	Texture2D collision7;
	Texture2D collision8;
	Texture2D collision9;
	Texture2D collision10;
	Texture2D collision11;
	Texture2D collision12;
	Texture2D collision13;

	uint32 bonus_fx;
	Music backgroundMusic;
	vec2<int> ray;
	bool ray_on;
};
