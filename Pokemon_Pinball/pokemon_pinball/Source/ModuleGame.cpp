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

protected:
	PhysBody* body;
	Module* listener;
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
	480, 954,
	520, 954,
	520, 774,
	505, 758,
	480, 755,
	466, 751,
	461, 743,
	461, 690,
	479, 672,
	507, 635,
	525, 599,
	542, 552,
	554, 498,
	552, 418,
	547, 383,
	537, 349,
	532, 334,
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
	489, 229,
	433, 193,
	386, 172,
	334, 158,
	261, 155,
	201, 167,
	137, 198,
	105, 223,
	78, 254,
	52, 300,
	38, 346,
	30, 386,
	24, 444,
	23, 493,
	31, 548,
	42, 583,
	56, 614,
	72, 641,
	83, 657,
	98, 674,
	114, 691,
	114, 743,
	109, 750,
	98, 754,
	73, 756,
	65, 762,
	58, 776,
	58, 952,
	94, 950,
	95, 920,
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
	static constexpr int CollisionSix[110] = {
	183, 449,
	176, 435,
	174, 425,
	170, 409,
	168, 396,
	168, 382,
	167, 318,
	170, 302,
	175, 288,
	183, 273,
	189, 263,
	196, 252,
	206, 241,
	215, 232,
	229, 222,
	239, 216,
	252, 210,
	263, 208,
	263, 254,
	257, 260,
	256, 325,
	260, 342,
	263, 364,
	269, 383,
	273, 396,
	281, 410,
	287, 421,
	287, 426,
	279, 426,
	263, 426,
	253, 426,
	242, 418,
	242, 400,
	246, 373,
	246, 349,
	250, 341,
	249, 325,
	239, 317,
	230, 316,
	230, 307,
	192, 307,
	192, 318,
	179, 325,
	173, 326,
	173, 342,
	178, 344,
	181, 348,
	181, 373,
	184, 391,
	187, 404,
	190, 414,
	192, 423,
	194, 433,
	194, 443,
	192, 449
	};

	Collision6(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionSix, 110), _listener)
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
	299, 393,
	290, 389,
	286, 381,
	288, 374,
	294, 368,
	302, 365,
	313, 365,
	321, 370,
	325, 377,
	324, 386,
	317, 391





	};

	Collision9(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionNine, 22), _listener)
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
	374, 413,
	364, 408,
	359, 400,
	361, 393,
	368, 387,
	383, 385,
	391, 389,
	397, 397,
	396, 406,
	387, 412
	};

	Collision11(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionEleven, 20), _listener)
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

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	App->audio->PlayFx(bonus_fx);
}
