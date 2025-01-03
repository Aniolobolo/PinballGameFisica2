#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"


#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
	
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);
	

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// big static circle as "ground" in the middle of the screen
	int x = (int)(SCREEN_WIDTH / 2);
	int y = (int)(SCREEN_HEIGHT / 1.5f);
	int diameter = SCREEN_WIDTH / 2;

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* big_ball = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;


	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, BodyType bodyType, CircleType circleType)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;

	// Configura el tipo de cuerpo en funci�n del par�metro bodyType
	if (bodyType == STATIC) {
		body.type = b2_staticBody;  // Cuerpo est�tico
	}
	else {
		body.type = b2_dynamicBody; // Cuerpo din�mico
	}


	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = 0.65f;
	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;

	// Asigna el tipo de cuerpo al PhysBody
	pbody->bodyType = bodyType;
	pbody->circleType = circleType;

	return pbody;
}


PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;

	body.type = b2_dynamicBody; 

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = width;
	pbody->height = height;


	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateLeftFlipper(int x,int y)
{

	PhysBody* leftFlipper = new PhysBody();


	b2Vec2 anchorLeft(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));


	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position = anchorLeft;
	b2Body* leftAnchor = world->CreateBody(&anchorDef);


	leftFlipper->body = CreateRectangle(METERS_TO_PIXELS(anchorLeft.x), METERS_TO_PIXELS(anchorLeft.y), 60, 10)->body;


	b2RevoluteJointDef jointDef;
	jointDef.enableMotor = true;
	jointDef.maxMotorTorque = 1000.0f;
	jointDef.enableLimit = true;
	jointDef.lowerAngle = -30.0f * b2_pi / 180.0f;
	jointDef.upperAngle = 30.0f * b2_pi / 180.0f;

	jointDef.bodyA = leftAnchor;
	jointDef.bodyB = leftFlipper->body;
	jointDef.localAnchorA.SetZero();
	jointDef.localAnchorB.Set(-PIXEL_TO_METERS(30), 0);
	lJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);


	leftFlipper->width = 60;
	leftFlipper->height = 10;

	return leftFlipper;
}

PhysBody* ModulePhysics::CreateRightFlipper(int x, int y)
{

	PhysBody* rightFlipper = new PhysBody();


	b2Vec2 anchorRight(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;
	anchorDef.position = anchorRight;
	b2Body* rightAnchor = world->CreateBody(&anchorDef);


	rightFlipper->body = CreateRectangle(METERS_TO_PIXELS(anchorRight.x), METERS_TO_PIXELS(anchorRight.y), 60, 10)->body;

	b2RevoluteJointDef jointDef;
	jointDef.enableMotor = true;
	jointDef.maxMotorTorque = 1000.0f;
	jointDef.enableLimit = true;


	jointDef.lowerAngle = -30.0f * b2_pi / 180.0f;
	jointDef.upperAngle = 30.0f * b2_pi / 180.0f;

	jointDef.bodyA = rightAnchor;
	jointDef.bodyB = rightFlipper->body;
	jointDef.localAnchorA.SetZero();


	jointDef.localAnchorB.Set(PIXEL_TO_METERS(30), 0);

	rJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);


	rightFlipper->width = 60;
	rightFlipper->height = 10;

	return rightFlipper;
}

PhysBody* ModulePhysics::CreateSpringBase(int x, int y, int width, int height)
{
	b2BodyDef baseDef;
	baseDef.type = b2_staticBody;
	baseDef.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* springBase = new PhysBody();
	springBase->body = world->CreateBody(&baseDef);

	return springBase;
}
// 
update_status ModulePhysics::PostUpdate()
{
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		if (App->scene_intro->deleteCircles) //si es true
		{
			std::vector<b2Body*> bodiesToDestroy;

			for (b2Body* b = world->GetBodyList(); b != nullptr; b = b->GetNext())
			{
				bool isCirclePOKEBALL = false;

				for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
				{
					if (f->GetType() == b2Shape::e_circle)
					{
						// Verificamos si es del tipo POKEBALL
						PhysBody* pbody = (PhysBody*)b->GetUserData().pointer;

						if (pbody && pbody->circleType == POKEBALL)
						{
							isCirclePOKEBALL = true;
						}
						break; 
					}
				}

				if (isCirclePOKEBALL)
				{
					bodiesToDestroy.push_back(b); 
				}
			}

			for (b2Body* b : bodiesToDestroy)
			{
				world->DestroyBody(b);
			}

			App->scene_intro->deleteCircles = false;
		}
		return UPDATE_CONTINUE;
	}

	if (App->scene_intro->deleteCircles) // Si es true
	{
		if (mouse_joint != nullptr)
		{
			world->DestroyJoint(mouse_joint);
			mouse_joint = nullptr;
		}

		std::vector<b2Body*> bodiesToDestroy;

		for (b2Body* b = world->GetBodyList(); b != nullptr; b = b->GetNext())
		{
			bool isCirclePOKEBALL = false;

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->GetType() == b2Shape::e_circle)
				{

					PhysBody* pbody = (PhysBody*)b->GetUserData().pointer;

					if (pbody && pbody->circleType == POKEBALL)
					{
						isCirclePOKEBALL = true;
					}
					break;
				}
			}

			if (isCirclePOKEBALL)
			{
				bodiesToDestroy.push_back(b);
			}
		}

		for (b2Body* b : bodiesToDestroy)
		{
			world->DestroyBody(b);
		}

		App->scene_intro->deleteCircles = false;
	}

	if (IsKeyPressed(KEY_TWO))
	{

		std::vector<b2Body*> bodiesToDestroy;

		for (b2Body* b = world->GetBodyList(); b != nullptr; b = b->GetNext())
		{
			bool isCirclePOKEBALL = false; 

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->GetType() == b2Shape::e_circle)
				{

					PhysBody* pbody = (PhysBody*)b->GetUserData().pointer;

					if (pbody && pbody->circleType == POKEBALL)
					{
						isCirclePOKEBALL = true;
					}
					break;
				}
			}

			if (isCirclePOKEBALL)
			{
				bodiesToDestroy.push_back(b);
			}
		}

		for (b2Body* b : bodiesToDestroy)
		{
			world->DestroyBody(b);
		}
	}



	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					
					DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{0, 0, 0, 10});
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->m_count;
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->m_vertices[i]);
						if(i > 0)
							DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->m_vertices[0]);
					DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
				}
				break;
			}
			b2Body* mouseSelect = nullptr;
			Vector2 mousePosition = GetMousePosition();
			b2Vec2 pMousePosition = b2Vec2(PIXEL_TO_METERS(mousePosition.x), PIXEL_TO_METERS(mousePosition.y));
			b2Vec2 pos = b->GetPosition();

			float angle = b->GetAngle() * 180.0f / b2_pi;

			Vector2 texturePosition = { (float)(METERS_TO_PIXELS(pos.x) - 30), (float)(METERS_TO_PIXELS(pos.y) - 5) };

			if (mouse_joint == nullptr && mouseSelect == nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

				if (f->TestPoint(pMousePosition)) {
					mouseSelect = b;
				
				}
			}
			if (mouseSelect) {
				b2MouseJointDef def;

				def.bodyA = ground;
				def.bodyB = mouseSelect;
				def.target = pMousePosition;
				def.damping = 0.5f;
				def.stiffness = 20.f;
				def.maxForce = 100.f * mouseSelect->GetMass();

				mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
			}

			// TODO 3: If the player keeps pressing the mouse button, update
			// target position and draw a red line between both anchor points
			else if (mouse_joint && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				mouse_joint->SetTarget(pMousePosition);
				b2Vec2 anchorPosition = mouse_joint->GetBodyB()->GetPosition();
				anchorPosition.x = METERS_TO_PIXELS(anchorPosition.x);
				anchorPosition.y = METERS_TO_PIXELS(anchorPosition.y);

				DrawLine(anchorPosition.x, anchorPosition.y, mousePosition.x, mousePosition.y, RED);
			}

			// TODO 4: If the player releases the mouse button, destroy the joint
			else if (mouse_joint && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				world->DestroyJoint(mouse_joint);
				mouse_joint = nullptr;
			}
		}

	}


	

	return UPDATE_CONTINUE;
}

			
			
			

void ModulePhysics::DrawFlipper(Texture2D flipperTexture, PhysBody* flipper, b2RevoluteJoint* joint)
{
	// Obtiene el �ngulo de la pala en radianes y lo convierte a grados
	float angleDegrees = RADTODEG * joint->GetJointAngle();

	// Obtiene la posici�n de la pala en pixeles
	b2Vec2 position = flipper->body->GetPosition();
	int posX = METERS_TO_PIXELS(position.x);
	int posY = METERS_TO_PIXELS(position.y);

	// Dibuja la textura en la posici�n y rotaci�n actual de la pala
	DrawTexturePro(
		flipperTexture,
		Rectangle{ 0, 0, (float)flipperTexture.width, (float)flipperTexture.height },
		Rectangle{ (float)posX, (float)posY, (float)flipperTexture.width, (float)flipperTexture.height },
		Vector2{ (float)(flipperTexture.width / 2), (float)(flipperTexture.height / 2) }, // Centro de rotaci�n de la textura
		angleDegrees,
		WHITE
	);
}
void ModulePhysics::DrawSpring()
{
	if (springPiston != nullptr)
	{
		int posX, posY;
		springPiston->GetPhysicPosition(posX, posY);

		// Ajusta las dimensiones seg�n la textura
		DrawTexture(muelle, posX - (muelle.width / 2), posY - (muelle.height / 2), WHITE);
	}
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");
	UnloadTexture(leftFlipperTexture);
	UnloadTexture(rightFlipperTexture);
	UnloadTexture(muelle);
	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

void PhysBody::Rotate(float angle)
{
	if (body) {
		b2Vec2 position = body->GetPosition();
		float currentAngle = body->GetAngle();
		body->SetTransform(position, currentAngle + angle);
	}
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}