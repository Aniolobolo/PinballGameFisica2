#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}
	
	CollisionType GetCollisionType() const { return collisionType; }

	PhysBody* body;

protected:

	Module* listener;
	CollisionType collisionType;

};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateCircle(_x, _y, 15), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f};
		float rotation = body->GetRotation() * RAD2DEG;
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

private:
	Texture2D texture;

};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 50), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f}, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

private:
	Texture2D texture;

};

class LeftPad : public PhysicEntity
{
public:
	LeftPad(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 20), _listener)
		, texture(_texture)
	{
	}

	void Update() override
	{
		// Rotación controlada por el teclado
		if (IsKeyDown(KEY_A)) {
			body->Rotate(-5.0f * DEG2RAD);
		}
		else {
			body->Rotate(5.0f * DEG2RAD);
		}

		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture,
			Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2, (float)texture.height / 2 },
			body->GetRotation() * RAD2DEG, WHITE);
	}

private:
	Texture2D texture;
};

class RightPad : public PhysicEntity
{
public:
	RightPad(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRectangle(_x, _y, 100, 20), _listener)
		, texture(_texture)
	{
	}

	void Update() override
	{
		// Rotación controlada por el teclado
		if (IsKeyDown(KEY_D)) {
			body->Rotate(-5.0f * DEG2RAD);
		}
		else {
			body->Rotate(5.0f * DEG2RAD);
		}

		// Posición de la pala en el espacio de juego
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture,
			Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2, (float)texture.height / 2 },
			body->GetRotation() * RAD2DEG, WHITE);
	}

private:
	Texture2D texture;
};



class Collision1 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionOne[140] = {
336, 1018,
	337, 1007,
	341, 1000,
	411, 965,
	480, 920,
	503, 906,
	520, 896,
	520, 777,
	509, 758,
	480, 755,
	467, 750,
	462, 740,
	461, 684,
	479, 665,
	501, 631,
	519, 598,
	535, 558,
	548, 496,
	546, 438,
	541, 388,
	533, 350,
	527, 328,
	536, 337,
	550, 373,
	560, 433,
	564, 513,
	563, 981,
	605, 981,
	606, 454,
	600, 386,
	575, 317,
	535, 270,
	483, 225,
	427, 194,
	386, 172,
	334, 158,
	261, 155,
	201, 167,
	137, 198,
	105, 223,
	76, 253,
	51, 298,
	38, 343,
	30, 399,
	29, 437,
	28, 500,
	38, 550,
	49, 581,
	61, 609,
	75, 633,
	85, 650,
	98, 668,
	114, 684,
	114, 743,
	109, 750,
	98, 754,
	73, 756,
	65, 762,
	58, 776,
	58, 895,
	77, 908,
	93, 920,
	106, 930,
	121, 940,
	140, 951,
	170, 969,
	233, 1001,
	239, 1006,
	239, 1019,
	334, 1019



	};

	Collision1(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionOne, 140), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision2 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionTwo[24] = {
	207, 951,
	111, 889,
	103, 880,
	103, 797,
	112, 791,
	120, 800,
	120, 862,
	123, 867,
	129, 874,
	220, 933,
	220, 938,
	209, 949
	};

	Collision2(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionTwo, 24), _listener)
		, texture(_texture)
	{

	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision3 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionTree[24] = {
	369, 950,
	356, 937,
	356, 933,
	443, 877,
	451, 870,
	456, 860,
	456, 800,
	459, 794,
	466, 792,
	472, 797,
	472, 883,
	445, 901
	};

	Collision3(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionTree, 24), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision4 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionFour[38] = {
	364, 861,
	370, 851,
	376, 843,
	383, 834,
	389, 825,
	394, 818,
	401, 808,
	405, 801,
	410, 794,
	416, 794,
	420, 801,
	419, 822,
	419, 846,
	407, 854,
	396, 861,
	385, 868,
	378, 872,
	369, 872,
	365, 866
	};

	Collision4(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionFour, 38), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision5 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionFive[40] = {
	199, 873,
	187, 865,
	176, 858,
	163, 850,
	156, 846,
	156, 834,
	156, 796,
	161, 793,
	167, 796,
	172, 803,
	176, 810,
	183, 820,
	188, 828,
	193, 835,
	198, 842,
	202, 849,
	208, 857,
	210, 862,
	210, 867,
	205, 872
	};

	Collision5(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionFive, 40), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision6 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionSix[62] = {
	192, 448,
	182, 448,
	181, 446,
	178, 427,
	172, 399,
	169, 385,
	169, 349,
	171, 328,
	174, 312,
	178, 294,
	185, 273,
	191, 260,
	199, 249,
	210, 238,
	222, 229,
	235, 221,
	245, 216,
	255, 212,
	263, 211,
	263, 253,
	257, 260,
	257, 324,
	259, 345,
	262, 367,
	264, 379,
	269, 393,
	275, 406,
	282, 420,
	241, 420,
	196, 433,
	196, 443

	};

	Collision6(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionSix, 62), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class GreenEvoD : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int GreenEvoDerechaCollision[52] = {
	162, 650,
	155, 644,
	147, 635,
	140, 625,
	132, 615,
	122, 602,
	113, 584,
	103, 565,
	95, 547,
	89, 528,
	84, 510,
	81, 494,
	78, 477,
	88, 488,
	96, 496,
	105, 505,
	118, 517,
	130, 527,
	140, 533,
	148, 538,
	152, 549,
	160, 569,
	167, 586,
	177, 605,
	188, 620,
	188, 624

	};

	GreenEvoD(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, GreenEvoDerechaCollision, 52), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};
class GreenOneI : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int GreenOneIzquierdaCollision[50] = {
	126, 458,
	116, 449,
	107, 440,
	100, 431,
	94, 420,
	88, 409,
	83, 398,
	78, 385,
	82, 366,
	87, 343,
	92, 323,
	98, 310,
	105, 297,
	115, 281,
	124, 269,
	133, 260,
	138, 260,
	138, 264,
	131, 277,
	123, 301,
	118, 335,
	118, 360,
	118, 391,
	120, 418,
	126, 447

	};

	GreenOneI(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, GreenOneIzquierdaCollision, 50), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};
class Collision7 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionSeven[16] = {
	302, 254,
	298, 248,
	298, 213,
	302, 207,
	309, 207,
	314, 213,
	314, 251,
	311, 254
	};

	Collision7(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionSeven, 16), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision8 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionEight[16] = {
	352, 257,
	348, 254,
	348, 220,
	354, 214,
	359, 214,
	365, 220,
	365, 253,
	361, 257
	};

	Collision8(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionEight, 16), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision9 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionNine[22] = {
295, 392,
	285, 389,
	282, 381,
	279, 371,
	275, 348,
	288, 362,
	304, 367,
	316, 369,
	323, 377,
	323, 386,
	313, 389






	};

	Collision9(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionNine, 22), _listener), texture(_texture)
	{
		collisionType = CHINCHOU;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};
class Collision10 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionTen[22] = {
	352, 340,
	343, 337,
	340, 328,
	343, 319,
	351, 313,
	363, 313,
	372, 316,
	377, 323,
	377, 330,
	371, 337,
	364, 340
	};

	Collision10(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionTen, 22), _listener)
		, texture(_texture)
	{
		collisionType = CHINCHOU;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};
class Collision11 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionEleven[20] = {
	372, 410,
	364, 408,
	359, 400,
	361, 393,
	368, 387,
	380, 385,
	387, 388,
	391, 394,
	388, 402,
	380, 409
	};

	Collision11(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionEleven, 20), _listener)
		, texture(_texture)
	{
		collisionType = CHINCHOU;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision12 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionTwelve[66] = {
	399, 234,
	410, 240,
	426, 252,
	438, 262,
	452, 277,
	454, 292,
	456, 303,
	455, 319,
	451, 341,
	447, 363,
	440, 396,
	430, 418,
	417, 446,
	400, 478,
	386, 501,
	370, 493,
	370, 489,
	376, 483,
	388, 468,
	400, 450,
	411, 429,
	421, 408,
	432, 383,
	441, 360,
	449, 336,
	452, 323,
	453, 309,
	452, 297,
	446, 287,
	435, 278,
	422, 270,
	412, 264,
	399, 259
	};

	Collision12(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionTwelve, 66), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

class Collision13 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionThirteen[70] = {
	416, 547,
	416, 539,
	434, 509,
	456, 470,
	471, 434,
	481, 408,
	486, 385,
	489, 367,
	487, 339,
	496, 365,
	500, 393,
	503, 418,
	505, 447,
	503, 481,
	496, 516,
	486, 545,
	479, 560,
	467, 583,
	459, 594,
	459, 584,
	466, 572,
	475, 550,
	481, 533,
	487, 511,
	491, 496,
	492, 483,
	486, 473,
	478, 467,
	470, 467,
	460, 469,
	453, 479,
	445, 500,
	439, 513,
	429, 530,
	423, 540
	};

	Collision13(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionThirteen, 70), _listener)
		, texture(_texture)
	{

	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
};

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{ 
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = LoadTexture("Assets/pokeball.png"); 

	box = LoadTexture("Assets/crate.png");
	//rick = LoadTexture("Assets/rick_head.png");
	
	bonus_fx = App->audio->LoadFx("Assets/Po.wav");
	backgroundMusic = LoadMusicStream("Assets/19-Red-Table.ogg");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	PlayMusicStream(backgroundMusic);

	entities.emplace_back(new Collision1(App->physics, 0, 0, this, collision1));
	entities.emplace_back(new Collision2(App->physics, 0, 0, this, collision2));
	entities.emplace_back(new Collision3(App->physics, 0, 0, this, collision3));
	entities.emplace_back(new Collision4(App->physics, 0, 0, this, collision4));
	entities.emplace_back(new Collision5(App->physics, 0, 0, this, collision5));
	entities.emplace_back(new Collision6(App->physics, 0, 0, this, collision6));
	entities.emplace_back(new GreenEvoD(App->physics, 0, 0, this, GreenEvoDer)); 
	entities.emplace_back(new GreenOneI(App->physics, 0, 0, this, GreenOneIzq));
	entities.emplace_back(new Collision7(App->physics, 0, 0, this, collision7));
	entities.emplace_back(new Collision8(App->physics, 0, 0, this, collision8));
	entities.emplace_back(new Collision9(App->physics, 0, 0, this, collision9));
	entities.emplace_back(new Collision10(App->physics, 0, 0, this, collision10));
	entities.emplace_back(new Collision11(App->physics, 0, 0, this, collision11));
	entities.emplace_back(new Collision12(App->physics, 0, 0, this, collision12));
	entities.emplace_back(new Collision13(App->physics, 0, 0, this, collision13));
	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleGame::Update()
{

	UpdateMusicStream(backgroundMusic);
	if(IsKeyPressed(KEY_SPACE))
	{
		ray_on = !ray_on;
		ray.x = GetMouseX();
		ray.y = GetMouseY();
	}

	if(IsKeyPressed(KEY_ONE))
	{
		entities.emplace_back(new Circle(App->physics, GetMousePosition().x, GetMousePosition().y, this, circle));
		
	}


	//if(IsKeyPressed(KEY_TWO))
	//{
	//	entities.emplace_back(new Box(App->physics, GetMouseX(), GetMouseY(), this, box));
	//}


	// Prepare for raycast ------------------------------------------------------
	
	vec2i mouse;
	mouse.x = GetMouseX();
	mouse.y = GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	vec2f normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------


	for (PhysicEntity* entity : entities)
	{
		entity->Update();
		if (ray_on)
		{
			int hit = entity->RayHit(ray, mouse, normal);
			if (hit >= 0)
			{
				ray_hit = hit;
			}
		}
	}
	

	// ray -----------------
	if(ray_on == true)
	{
		vec2f destination((float)(mouse.x-ray.x), (float)(mouse.y-ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}

	DrawText(TextFormat("Score: %d",suma), 10, 10, 30, WHITE);

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	bool hascollisioned = false;

	int length = entities.size();
	for (int i = 0; i < length; ++i) {
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == CHINCHOU ){
			hascollisioned = true;
		    break;
		}
	}
	if (hascollisioned) suma += 500;

	


    App->audio->PlayFx(bonus_fx);  // Reproduce el sonido
}