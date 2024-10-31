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

class Rick : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int rick_head[138] = {
		337, 1019,
	336, 1006,
	346, 1000,
	401, 971,
	480, 921,
	523, 895,
	523, 776,
	518, 762,
	505, 757,
	489, 756,
	472, 753,
	465, 750,
	461, 743,
	461, 684,
	479, 666,
	496, 641,
	513, 612,
	527, 583,
	535, 553,
	543, 520,
	548, 476,
	547, 436,
	542, 392,
	529, 331,
	534, 336,
	542, 350,
	552, 380,
	559, 421,
	564, 509,
	564, 981,
	605, 981,
	606, 452,
	603, 412,
	591, 351,
	571, 308,
	548, 275,
	513, 243,
	472, 214,
	417, 184,
	369, 166,
	323, 155,
	256, 155,
	217, 162,
	176, 177,
	135, 201,
	95, 235,
	64, 276,
	40, 335,
	28, 407,
	28, 502,
	37, 546,
	55, 599,
	82, 645,
	115, 685,
	115, 742,
	110, 750,
	98, 755,
	74, 757,
	65, 763,
	58, 774,
	58, 896,
	101, 927,
	129, 945,
	165, 966,
	213, 991,
	228, 998,
	238, 1005,
	238, 1020,
	336, 1020

	};

	Rick(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, rick_head, 138), _listener)
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

class Rick2 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int down_left_collision[24] = {
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

	Rick2(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, down_left_collision, 24), _listener)
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

class Rick3 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int down_right_collision[24] = {
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

	Rick3(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, down_right_collision, 24), _listener)
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

class Rick4 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int down_up_right_collision[38] = {
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

	Rick4(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, down_up_right_collision, 38), _listener)
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

class Rick5 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int down_up_left_collision[40] = {
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

	Rick5(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, down_up_left_collision, 40), _listener)
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

class Rick6 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int hatch[110] = {
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

	Rick6(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, hatch, 110), _listener)
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
	static constexpr int GreenEvoDerechaCollision[82] = {
	162, 650,
	154, 642,
	145, 634,
	138, 625,
	129, 611,
	121, 601,
	113, 587,
	106, 572,
	100, 559,
	94, 545,
	88, 528,
	83, 510,
	77, 489,
	58, 462,
	59, 442,
	69, 460,
	76, 470,
	84, 481,
	92, 489,
	100, 497,
	109, 507,
	123, 520,
	128, 524,
	134, 529,
	137, 531,
	142, 534,
	148, 537,
	151, 547,
	154, 554,
	158, 564,
	161, 571,
	167, 580,
	171, 590,
	177, 600,
	182, 609,
	186, 615,
	189, 623,
	184, 629,
	179, 635,
	172, 642,
	166, 647
	};

	GreenEvoD(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, GreenEvoDerechaCollision, 82), _listener)
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

	entities.emplace_back(new Rick(App->physics, 0, 0, this, rick));
	entities.emplace_back(new Rick2(App->physics, 0, 0, this, rick2));
	entities.emplace_back(new Rick3(App->physics, 0, 0, this, rick3));
	entities.emplace_back(new Rick4(App->physics, 0, 0, this, rick4));
	entities.emplace_back(new Rick5(App->physics, 0, 0, this, rick5));
	entities.emplace_back(new Rick6(App->physics, 0, 0, this, rick6));
	entities.emplace_back(new GreenEvoD(App->physics, 0, 0, this, GreenEvoDer)); 
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
	//583, 850,
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
