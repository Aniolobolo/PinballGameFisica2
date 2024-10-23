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
		: PhysicEntity(physics->CreateChain(GetMouseX() - 50, GetMouseY() - 100, rick_head, 138), _listener)
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

	if(IsKeyPressed(KEY_THREE))
	{
		entities.emplace_back(new Rick(App->physics, GetMouseX(), GetMouseY(), this, rick));
	}

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
