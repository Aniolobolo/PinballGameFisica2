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
	debug = true;
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
	//big_ball->CreateFixture(&fixture);

	// Crear el cuerpo base para las `joints`
	ground = world->CreateBody(&bd);

	lFlipper = nullptr;
	rFlipper = nullptr;
	lJoint = nullptr;
	rJoint = nullptr;

	leftFlipperTexture = LoadTexture("Assets/leftFlipper.png");
	rightFlipperTexture = LoadTexture("Assets/rightFlipper.png");
	muelle = LoadTexture("Assets/muelle1.png");
	// Llama a la función de creación de las palas
	CreateFlippers();

	CreateSpring(SCREEN_WIDTH - 30, SCREEN_HEIGHT - 100, 30, 50);


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

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, BodyType bodyType)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;

	// Configura el tipo de cuerpo en función del parámetro bodyType
	if (bodyType == STATIC) {
		body.type = b2_staticBody;  // Cuerpo estático
	}
	else {
		body.type = b2_dynamicBody; // Cuerpo dinámico
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

void ModulePhysics::CreateFlippers()
{
	// Posiciones de los anclajes de las palas
	b2Vec2 anchorLeft(PIXEL_TO_METERS(215), PIXEL_TO_METERS(940));  // Anclaje de la pala izquierda
	b2Vec2 anchorRight(PIXEL_TO_METERS(360), PIXEL_TO_METERS(940)); // Anclaje de la pala derecha

	// Crear anclajes estáticos para las palas
	b2BodyDef anchorDef;
	anchorDef.type = b2_staticBody;

	// Crear cuerpos de los anclajes
	anchorDef.position = anchorLeft;
	b2Body* leftAnchor = world->CreateBody(&anchorDef);

	anchorDef.position = anchorRight;
	b2Body* rightAnchor = world->CreateBody(&anchorDef);

	// Crear las palas
	leftFlipper = CreateRectangle(METERS_TO_PIXELS(anchorLeft.x), METERS_TO_PIXELS(anchorLeft.y), 60, 10);
	rightFlipper = CreateRectangle(METERS_TO_PIXELS(anchorRight.x), METERS_TO_PIXELS(anchorRight.y), 60, 10);

	// Configurar las revolute joints para las palas
	b2RevoluteJointDef jointDef;
	jointDef.enableMotor = true;
	jointDef.maxMotorTorque = 1000.0f;  // Torque máximo del motor
	jointDef.enableLimit = true;
	jointDef.lowerAngle = -30.0f * b2_pi / 180.0f;
	jointDef.upperAngle = 30.0f * b2_pi / 180.0f;

	// Conectar la pala izquierda
	jointDef.bodyA = leftAnchor;
	jointDef.bodyB = leftFlipper->body;
	jointDef.localAnchorA.SetZero();
	jointDef.localAnchorB.Set(-PIXEL_TO_METERS(30), 0);  // Punto de anclaje en la pala
	lJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);

	// Conectar la pala derecha
	jointDef.bodyA = rightAnchor;
	jointDef.bodyB = rightFlipper->body;
	jointDef.localAnchorA.SetZero();
	jointDef.localAnchorB.Set(PIXEL_TO_METERS(30), 0);  // Punto de anclaje en la pala
	rJoint = (b2RevoluteJoint*)world->CreateJoint(&jointDef);
}

PhysBody* ModulePhysics::CreateSpring(int x, int y, int width, int height)
{
	// Crear el cuerpo base estático del muelle
	b2BodyDef baseDef;
	baseDef.type = b2_staticBody;
	baseDef.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	springBase = world->CreateBody(&baseDef);

	// Crear el cuerpo dinámico (el "pistón" del muelle)
	springPiston = CreateRectangle(x, y + height / 2, width, height);

	// Configurar el prismatic joint
	b2PrismaticJointDef prismaticJointDef;
	prismaticJointDef.bodyA = springBase;
	prismaticJointDef.bodyB = springPiston->body;
	prismaticJointDef.collideConnected = false;
	prismaticJointDef.localAnchorA.Set(0, 0);
	prismaticJointDef.localAnchorB.Set(0, -PIXEL_TO_METERS(height) / 2);

	// Configurar la dirección de movimiento (solo en el eje Y)
	prismaticJointDef.localAxisA.Set(0, 1);

	// Configurar los límites del movimiento
	prismaticJointDef.enableLimit = true;
	prismaticJointDef.lowerTranslation = -PIXEL_TO_METERS(height * 0.3f); // Distancia máxima hacia abajo
	prismaticJointDef.upperTranslation = PIXEL_TO_METERS(height * 0.3f);  // Distancia máxima hacia arriba

	// Configurar el resorte para simular el muelle
	prismaticJointDef.enableMotor = true;
	prismaticJointDef.maxMotorForce = 1000.0f; // Fuerza del motor
	prismaticJointDef.motorSpeed = 0.0f;       // Velocidad inicial del motor

	// Crear el muelle
	springJoint = (b2PrismaticJoint*)world->CreateJoint(&prismaticJointDef);

	return springPiston;
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
		return UPDATE_CONTINUE;
	}

	if (IsKeyDown(KEY_A))
	{
		lJoint->SetMotorSpeed(-20.0f); // Rotar a la izquierda
	}
	else {
		lJoint->SetMotorSpeed(20.0f); // Detener
	}

	if (IsKeyDown(KEY_D))
	{
		rJoint->SetMotorSpeed(20.0f); // Rotar a la derecha
	}
	else
	{
		rJoint->SetMotorSpeed(-20.0f); // Detener
	}

	if (IsKeyDown(KEY_S))
	{
		springJoint->SetMotorSpeed(1.0f); // Usa una velocidad menor si es necesario
		springJoint->SetMaxMotorForce(1500.0f); // Ajusta la fuerza según lo necesario
	}
	else
	{
		springJoint->SetMotorSpeed(-500.0f); // Usa una velocidad positiva para regresar
	}
	DrawSpring();


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

			b2Vec2 pos = b->GetPosition();
			float angle = b->GetAngle() * 180.0f / b2_pi;

			Vector2 texturePosition = { (float)(METERS_TO_PIXELS(pos.x) - 30), (float)(METERS_TO_PIXELS(pos.y) - 5) };


			if (b == leftFlipper->body)
			{
				DrawFlipper(leftFlipperTexture, leftFlipper, lJoint);
				
			}
			else if (b == rightFlipper->body)
			{
				DrawFlipper(rightFlipperTexture, rightFlipper, rJoint);
			}

			
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property
	

	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points

	// TODO 4: If the player releases the mouse button, destroy the joint

	return UPDATE_CONTINUE;
}

void ModulePhysics::DrawFlipper(Texture2D flipperTexture, PhysBody* flipper, b2RevoluteJoint* joint)
{
	// Obtiene el ángulo de la pala en radianes y lo convierte a grados
	float angleDegrees = RADTODEG * joint->GetJointAngle();

	// Obtiene la posición de la pala en pixeles
	b2Vec2 position = flipper->body->GetPosition();
	int posX = METERS_TO_PIXELS(position.x);
	int posY = METERS_TO_PIXELS(position.y);

	// Dibuja la textura en la posición y rotación actual de la pala
	DrawTexturePro(
		flipperTexture,
		Rectangle{ 0, 0, (float)flipperTexture.width, (float)flipperTexture.height },
		Rectangle{ (float)posX, (float)posY, (float)flipperTexture.width, (float)flipperTexture.height },
		Vector2{ (float)(flipperTexture.width / 2), (float)(flipperTexture.height / 2) }, // Centro de rotación de la textura
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

		// Ajusta las dimensiones según la textura
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