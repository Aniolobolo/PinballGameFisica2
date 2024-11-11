#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

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

	virtual void ActivateHit() {
	}
	virtual void ActivateRotation() {
	}
	
	CollisionType GetCollisionType() const { return collisionType; }

	Pokemons GetPokemon() const { return pokemons; }

	SensorType GetSensor() const { return sensors; }

	PhysBody* body;

protected:

	Module* listener;
	CollisionType collisionType;
	Pokemons pokemons;
	SensorType sensors;
	bool Hit;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateCircle(_x, _y, 15, DYNAMIC, POKEBALL), _listener)
		, texture(_texture)
	{
		frameCount = 16;
		currentFrame = frameCount - 1; 
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		float rotation = body->GetRotation() * RAD2DEG; //radianes a grados

		currentFrame = frameCount - 1 - (static_cast<int>((rotation / 360.0f) * frameCount) % frameCount);
		
		Rectangle source = { currentFrame * 32.0f, 0.0f, 32.0f, 32.0f };
		Rectangle dest = { position.x, position.y, 32.0f, 32.0f };
		Vector2 origin = { 16.0f, 16.0f };

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}

private:
	Texture2D texture;
	int currentFrame;      
	int frameCount;     
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
		: PhysicEntity(physics->CreateLeftFlipper(_x, _y), _listener)
		, texture(_texture)
		, leftAnchor(nullptr)
		, leftJoint(nullptr)
	{
		// Crear anclaje estático para la pala izquierda
		b2Vec2 anchorLeft(PIXEL_TO_METERS(_x), PIXEL_TO_METERS(_y));
		b2BodyDef anchorDef;
		anchorDef.type = b2_staticBody;
		anchorDef.position = anchorLeft;
		leftAnchor = physics->GetWorld()->CreateBody(&anchorDef);

		// Configurar la revolute joint para la pala izquierda
		b2RevoluteJointDef jointDef;
		jointDef.bodyA = leftAnchor;
		jointDef.bodyB = body->body;
		jointDef.localAnchorA.SetZero();
		jointDef.localAnchorB.Set(-PIXEL_TO_METERS(30), 0); // Punto de anclaje en la pala
		jointDef.enableMotor = true;
		jointDef.maxMotorTorque = 1000.0f;
		jointDef.enableLimit = true;
		jointDef.lowerAngle = -30.0f * b2_pi / 180.0f;
		jointDef.upperAngle = 30.0f * b2_pi / 180.0f;

		leftJoint = (b2RevoluteJoint*)physics->GetWorld()->CreateJoint(&jointDef);
	}

	void Update() override
	{
		// Control de rotación de la pala izquierda con el teclado
		if (IsKeyDown(KEY_A)) {
			leftJoint->SetMotorSpeed(-60.0f); // Rotación en sentido horario
		}
		else {
			leftJoint->SetMotorSpeed(60.0f); // Retorno en sentido antihorario
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
	b2Body* leftAnchor;
	b2RevoluteJoint* leftJoint;
};

class RightPad : public PhysicEntity
{
public:
	RightPad(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateRightFlipper(_x, _y), _listener)
		, texture(_texture)
		, rightAnchor(nullptr)
		, rightJoint(nullptr)
	{
		// Crear anclaje estático para la pala derecha
		b2Vec2 anchorRight(PIXEL_TO_METERS(_x), PIXEL_TO_METERS(_y));
		b2BodyDef anchorDef;
		anchorDef.type = b2_staticBody;
		anchorDef.position = anchorRight;
		rightAnchor = physics->GetWorld()->CreateBody(&anchorDef);

		// Configurar la revolute joint para la pala derecha
		b2RevoluteJointDef jointDef;
		jointDef.bodyA = rightAnchor;
		jointDef.bodyB = body->body;
		jointDef.localAnchorA.SetZero();
		jointDef.localAnchorB.Set(PIXEL_TO_METERS(30), 0); // Ajuste de anclaje para pivote en el lado izquierdo
		jointDef.enableMotor = true;
		jointDef.maxMotorTorque = 1000.0f;
		jointDef.enableLimit = true;
		jointDef.lowerAngle = -30.0f * b2_pi / 180.0f;  // Límite inferior (mirando hacia abajo)
		jointDef.upperAngle = 30.0f * b2_pi / 180.0f; // Límite superior (hacia arriba)

		rightJoint = (b2RevoluteJoint*)physics->GetWorld()->CreateJoint(&jointDef);
	}

	void Update() override
	{
		// Control de rotación de la pala derecha con el teclado
		if (IsKeyDown(KEY_D)) {
			rightJoint->SetMotorSpeed(60.0f); // Rotación en sentido horario (hacia arriba)
		}
		else {
			rightJoint->SetMotorSpeed(-60.0f); // Retorno en sentido antihorario (hacia abajo)
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
	b2Body* rightAnchor;
	b2RevoluteJoint* rightJoint;
};

class Spring : public PhysicEntity
{
public:
	Spring(ModulePhysics* physics, int _x, int _y, int _width, int _height, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateSpringBase(_x, _y, _width, _height), _listener)
		, texture(_texture)
		, springPiston(nullptr)
		, springJoint(nullptr)
	{
		// Crear el pistón dinámico del resorte
		springPiston = physics->CreateRectangle(_x, _y + _height / 2, _width, _height);

		// Crear el prismatic joint que conecta el cuerpo base con el pistón
		b2PrismaticJointDef prismaticJointDef;
		prismaticJointDef.bodyA = body->body;  // Cuerpo base estático
		prismaticJointDef.bodyB = springPiston->body;
		prismaticJointDef.collideConnected = false;
		prismaticJointDef.localAnchorA.Set(0, 0);
		prismaticJointDef.localAnchorB.Set(0, -PIXEL_TO_METERS(_height) / 2);
		prismaticJointDef.localAxisA.Set(0, 1);  // Movimiento en el eje Y

		// Configurar límites de movimiento
		prismaticJointDef.enableLimit = true;
		prismaticJointDef.lowerTranslation = -PIXEL_TO_METERS(_height * 0.3f); // Límite inferior
		prismaticJointDef.upperTranslation = PIXEL_TO_METERS(_height * 0.3f);  // Límite superior

		// Configurar propiedades del motor (resorte)
		prismaticJointDef.enableMotor = true;
		prismaticJointDef.maxMotorForce = 1000.0f; // Fuerza del resorte
		prismaticJointDef.motorSpeed = 0.0f;       // Velocidad inicial

		// Crear el prismatic joint en el mundo de física
		springJoint = (b2PrismaticJoint*)physics->GetWorld()->CreateJoint(&prismaticJointDef);  // Usamos el puntero `world`
	}

	void Update() override
	{
		// Control de movimiento del resorte con el teclado
		if (IsKeyDown(KEY_S)) {
			springJoint->SetMotorSpeed(3.0f); // Comprimir resorte
		}
		else {
			springJoint->SetMotorSpeed(-20.0f);  // Soltar resorte
		}

		// Obtener la posición del pistón del resorte
		int x, y;
		springPiston->GetPhysicPosition(x, y);

		// Dibujar la textura del resorte
		DrawTexturePro(texture,
			Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2, (float)texture.height / 2 },
			springPiston->GetRotation() * RAD2DEG, WHITE);
	}

private:
	Texture2D texture;
	PhysBody* springPiston;
	b2PrismaticJoint* springJoint;
};


class Chinchou : public PhysicEntity {
public:
	Chinchou(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture)
	{
	collisionType = CHINCHOU;
	pokemons = CHINCHOU1;
	frameCountIdle = 2;
	frameCountHit = 2;
  	currentFrame = 0;
	animationSpeed = 0.03f;
	frameTimer = 0.0f;
	scale = 2.5f;
	isHit = false;
	hitTimer = 0.0f;
	}

void ActivateHit() override {
	OnHit();
}

void Update() override
{
	int x, y;
	body->GetPhysicPosition(x, y);
	Vector2 position{ (float)x, (float)y };


	// Animación idle
	if (!isHit) {
		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f) {
			currentFrame = (currentFrame + 1) % frameCountIdle;  // Alterna entre frame 0 y 1
			frameTimer = 0.0f;
		}
	}

	// Animación hit
	if (isHit) {
		hitTimer += GetFrameTime();
		if (hitTimer < 1.4f) {  // Duración de la animación de golpe
			frameTimer += animationSpeed;
			if (frameTimer >= 1.0f) {
				currentFrame = 2 + (currentFrame + 1) % frameCountHit;  // Alterna entre frame 2 y 3
				frameTimer = 0.0f;
			}
		}
		else {  // Después de 0.4 segundos, vuelve a la animación idle
			isHit = false;
			hitTimer = 0.0f;
			currentFrame = 0;  // Reinicia la animación idle al primer frame
		}
	}

	Rectangle source = { currentFrame * 32.0f, 0.0f, 32.0f, 32.0f };
	Rectangle dest = { position.x, position.y, 32.0f * scale, 32.0f * scale };
	Vector2 origin = { 16.0f * scale, 22.0f * scale };

	DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
}

void OnHit()
{
	if (!isHit) {  // Solo activa el golpe si no está ya activo
		isHit = true;
		currentFrame = 2;  // Inicia la animación de golpe en el frame 2
		hitTimer = 0.0f;
	}
}

private:
	Texture2D texture;
	int currentFrame;
	int frameCountIdle;
	int frameCountHit;
	float animationSpeed;
	float frameTimer;
	float scale;
	bool isHit;
	float hitTimer;   //attack timer  
};

class Pikachu : public PhysicEntity {
public:
	Pikachu(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 1, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = DEFAULT;
		frameCount = 9;       
		currentFrame = 0;
		animationSpeed = 0.05f; 
		frameTimer = 0.0f;
		scale = 1.3f;
	}
		void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}
	Rectangle source = { currentFrame * 64.0f, 0.0f, 64.0f, 64.0f };
		Rectangle dest = { position.x, position.y, 64.0f * scale, 64.0f * scale };
		Vector2 origin = { 70.0f , 70.0f  }; 

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
	
	private:
	Texture2D texture;
	int currentFrame;     
	int frameCount;       
	float animationSpeed;
	float frameTimer;    
	float scale;
};

class Gulpin : public PhysicEntity {
public:
	Gulpin(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = GULPIN;
		frameCount = 8;          // Solo dos cuadros de animaci�n
		currentFrame = 0;
		animationSpeed = 0.11f;   // Control de velocidad de la animaci�n
		frameTimer = 0.0f;
		scale = 1.2f; 
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}

		// Calcula el rect�ngulo de origen para el cuadro actual
		Rectangle source = { currentFrame * 64.0f, 0.0f, 64.0f, 48.0f };
		Rectangle dest = { position.x, position.y, 64.0f * scale, 48.0f * scale };
		Vector2 origin = { 259.0f * scale, -302.0f * scale}; // Centro del cuadro de 48x48

		// Dibuja el cuadro actual sin aplicar rotaci�n
		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
private:
	Texture2D texture;
	int currentFrame;     
	int frameCount;       
	float animationSpeed;
	float frameTimer;    
	float scale;
};

class Pichu : public PhysicEntity {
public:
	Pichu(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 1, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = DEFAULT;
		frameCount = 7;
		currentFrame = 0;
		animationSpeed = 0.05f;
		frameTimer = 0.0f;
		scale = 1.2f;
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}

		Rectangle source = { currentFrame * 64.0f, 0.0f, 64.0f, 64.0f };
		Rectangle dest = { position.x, position.y, 64.0f * scale, 64.0f * scale };
		Vector2 origin = { 70.0f , 70.0f };

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
private:
	Texture2D texture;
	int currentFrame;
	int frameCount;
	float animationSpeed;
	float frameTimer;
	float scale;
};

class Wishcash : public PhysicEntity {
public:
	Wishcash(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = WISHCASH;
		frameCount = 6;          // Solo dos cuadros de animaci�n
		currentFrame = 0;
		animationSpeed = 0.085f;   // Control de velocidad de la animaci�n
		frameTimer = 0.0f;
		scale = 1.25f;
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		// Avanza el temporizador de animaci�n
		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			// Cambia al siguiente cuadro y reinicia el temporizador
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}

		// Calcula el rect�ngulo de origen para el cuadro actual
		Rectangle source = { currentFrame * 96.0f, 0.0f, 96.0f, 96.0f };
		Rectangle dest = { position.x, position.y, 96.0f * scale, 96.0f * scale };
		Vector2 origin = { -32.0f * scale, 182.0f * scale }; // Centro del cuadro de 48x48

		// Dibuja el cuadro actual sin aplicar rotaci�n
		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
private:
	Texture2D texture;
	int currentFrame;       // Cuadro actual de la animaci�n
	int frameCount;         // N�mero total de cuadros en la animaci�n (2 en este caso)
	float animationSpeed;   // Control de velocidad de la animaci�n
	float frameTimer;       // Temporizador para cambiar de cuadro
	float scale;
};

class Nuzleaf : public PhysicEntity {
public:
	Nuzleaf(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = NUZLEAF;
		frameCount = 3;          // Solo dos cuadros de animaci�n
		currentFrame = 0;
		animationSpeed = 0.085f;   // Control de velocidad de la animaci�n
		frameTimer = 0.0f;
		scale = 1.25f;
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		// Avanza el temporizador de animaci�n
		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			// Cambia al siguiente cuadro y reinicia el temporizador
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}

		// Calcula el rect�ngulo de origen para el cuadro actual
		Rectangle source = { currentFrame * 64.0f, 0.0f, 64.0f, 80.0f };
		Rectangle dest = { position.x, position.y, 64.0f * scale, 80.0f * scale };
		Vector2 origin = { -98.0f * scale, -244.0f * scale }; // Centro del cuadro de 48x48

		// Dibuja el cuadro actual sin aplicar rotaci�n
		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
private:
	Texture2D texture;
	int currentFrame;       // Cuadro actual de la animaci�n
	int frameCount;         // N�mero total de cuadros en la animaci�n (2 en este caso)
	float animationSpeed;   // Control de velocidad de la animaci�n
	float frameTimer;       // Temporizador para cambiar de cuadro
	float scale;
};

class Latios : public PhysicEntity {
public:
	Latios(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture, float speedX)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture), speedX(speedX)
	{
		scale = 1.25f;
		isMarkedForDeletion = false;
	}

	bool hasToSpawnBall = false;
	bool pokeballSpawned = false;

	bool ShouldSpawnBall() const {
		return hasToSpawnBall;
	}

	void TriggerBallSpawn() {
		hasToSpawnBall = true;
	}

	void Update() override
	{
		// Actualiza la posición horizontal usando `posX`
		posX += speedX;

		// Comprueba si la posición supera la anchura de la ventana y marca para eliminación
		if (posX > GetScreenWidth() + 50) {
			isMarkedForDeletion = true;
			return; // No seguir dibujando si está marcado para eliminar
		}
		if (posX > GetScreenWidth() - 45) {
			hasToSpawnBall = true;
		}
		else {
			hasToSpawnBall = false;
		}

		// Obtiene la posición física de `y` para mantener el valor actual
		int x, y;
		body->GetPhysicPosition(x, y);

		// Usa `posX` en lugar de `x` para el dibujado
		Vector2 position{ posX, (float)y };

		Rectangle source = { 0.0f, 0.0f, 126.0f, 99.0f };
		Rectangle dest = { position.x, position.y, 126.0f * scale, 99.0f * scale };
		Vector2 origin = { 126.0f * scale / 2, 99.0f * scale / 2 }; // Centro de la imagen

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}

	bool IsMarkedForDeletion() const {
		return isMarkedForDeletion;
	}

private:
	Texture2D texture;
	float scale;
	float speedX;
	float posX;
	bool isMarkedForDeletion;
};

class DeleteSensor : public PhysicEntity {
public:
	DeleteSensor(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateRectangleSensor(x, y, SCREEN_WIDTH,50), listener), texture(texture)
	{
		collisionType = SENSOR;
		sensors = DELETE;

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

class Collision1 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionOne[140] = {
	337, 1058,
	337, 1007,
	341, 1000,
	411, 965,
	480, 920,
	519, 897,
	520, 777,
	511, 761,
	501, 755,
	480, 755,
	467, 750,
	462, 740,
	462, 686,
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
	239, 1058,
	288, 1058




	};

	Collision1(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionOne, 140), _listener)
		, texture(_texture)
	{
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
		frameCountIdle = 2;
		animationSpeed = 0.05f;
		frameTimer = 0.0f;
		scale = 1.2f;
		Hit = false;
		hitTimer = 0.0f;
	}

	void ActivateHit() override {
		OnHit();
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };


		if (Hit) // activate attack anim
		{
			hitTimer += GetFrameTime();
			if (hitTimer >= 0.4f) //after 0,4 second return to idle anim
			{
				Hit = false;
				hitTimer = 0.0f;
			}
		}

		Rectangle source;
		if (Hit)
		{
			source = { 80.0f, 0.0f, 80.0f, 80.0f }; //attack
		}
		else
		{
			source = { 0.0f, 0.0f, 80.0f, 80.0f }; //idle
		}

		
		Rectangle dest = { position.x, position.y, 80.0f * scale, 80.0f * scale };
		Vector2 origin = { -155.0f , -778.0f};
		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}

	void OnHit()
	{
		Hit = true;
		hitTimer = 0.0f;
	}

private:
	Texture2D texture;
	//int currentFrame;
	int frameCountIdle;
	float animationSpeed;
	float frameTimer;
	float scale;
	float hitTimer;   //attack timer  
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
		collisionType = DEFAULT;
		frameCount = 4;          
		currentFrame = 0;
		animationSpeed = 0.09f;   
		frameTimer = 0.0f;
		scale = 1.3f;
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		frameTimer += animationSpeed;
		if (frameTimer >= 1.0f)
		{
			currentFrame = (currentFrame + 1) % frameCount;
			frameTimer = 0.0f;
		}

		Rectangle source = { currentFrame * 80.0f, 0.0f, 80.0f, 80.0f };
		Rectangle dest = { position.x, position.y, 80.0f * scale, 80.0f * scale };
		Vector2 origin = { -165, -360 }; 

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}
private:
	Texture2D texture;
	int currentFrame;    
	int frameCount;      
	float animationSpeed;
	float frameTimer;    
	float scale;
};

class GreenEvoD : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int GreenEvoDerechaCollision[78] = {
	188, 621,
	161, 648,
	145, 635,
	134, 621,
	126, 607,
	114, 588,
	102, 563,
	94, 540,
	85, 514,
	79, 495,
	74, 472,
	72, 450,
	73, 425,
	75, 400,
	79, 377,
	84, 354,
	89, 332,
	95, 316,
	102, 300,
	110, 288,
	122, 272,
	137, 260,
	138, 266,
	130, 274,
	121, 295,
	116, 315,
	113, 336,
	112, 361,
	112, 388,
	114, 409,
	118, 430,
	121, 450,
	125, 474,
	130, 499,
	138, 526,
	149, 554,
	160, 581,
	171, 601,
	182, 617

	};

	GreenEvoD(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, GreenEvoDerechaCollision, 78), _listener)
		, texture(_texture)
	{
		collisionType = DEFAULT;
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

class TrianguloIzqColPunt : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int TICP[8] = {
	164, 795,
	210, 860,
	208, 862,
	161, 797

	};

	TrianguloIzqColPunt(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, TICP, 8), _listener)
		, texture(_texture)
	{
		collisionType = TRIANGULOIZQ; 
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

class TrianguloIzq : public PhysicEntity {
public:
	TrianguloIzq(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateCircle(x, y, 10, STATIC, ELSE), listener), texture(texture)
	{
		collisionType = TRIANGULOIZQ;
		/*pokemons = CHINCHOU1;*/
	}
	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
	}
private:
	Texture2D texture;
	  //attack timer  
};

class GreenOneI : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int GreenOneIzquierdaCollision[26] = {
	398, 234,
	414, 243,
	424, 251,
	433, 259,
	442, 267,
	450, 275,
	454, 285,
	450, 289,
	440, 286,
	427, 275,
	412, 266,
	398, 259,
	398, 241

	};

	GreenOneI(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, GreenOneIzquierdaCollision, 26), _listener)
		, texture(_texture)
	{
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
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
		collisionType = DEFAULT;
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
	static constexpr int CollisionTwelve[52] = {
	384, 501,
	371, 492,
	377, 486,
	383, 478,
	388, 469,
	392, 461,
	398, 453,
	403, 442,
	407, 434,
	412, 421,
	416, 412,
	420, 404,
	425, 395,
	429, 386,
	435, 376,
	441, 378,
	440, 385,
	436, 394,
	431, 405,
	426, 417,
	421, 430,
	414, 447,
	407, 461,
	400, 474,
	393, 486,
	387, 497
	};

	Collision12(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionTwelve, 52), _listener)
		, texture(_texture)
	{
		collisionType = DEFAULT;
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTextureEx(texture, Vector2{ (float)x, (float)y }, body->GetRotation() * RAD2DEG, 1.0f, WHITE);
	}

private:
	Texture2D texture;
	int currentFrame;
	int frameCount;
	float animationSpeed;
	float frameTimer;
	float scale;
};

class Collision13 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionThirteen[64] = { 
	415, 544,
	422, 528,
	431, 512,
	437, 502,
	446, 484,
	453, 470,
	461, 458,
	473, 447,
	488, 446,
	493, 451,
	500, 458,
	500, 465,
	497, 478,
	493, 497,
	488, 520,
	481, 542,
	475, 557,
	469, 568,
	462, 581,
	458, 588,
	463, 571,
	469, 554,
	473, 543,
	476, 532,
	477, 524,
	478, 514,
	471, 506,
	459, 501,
	450, 502,
	441, 507,
	430, 521,
	420, 537
	};
	Collision13(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionThirteen, 64), _listener)
		, texture(_texture)

	{
		collisionType = SHARPEDO;
		frameCountIdle = 2;      
		currentFrame = 0;
		animationSpeed = 0.05f;   
		frameTimer = 0.0f;
		scale = 1.2f;
		isHit = false;
		hitTimer = 0.0f;
	}

	void ActivateHit() override {
		OnHit();  
	}

	void Update() override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		
		if (!isHit) {  //idle anim
			frameTimer += animationSpeed;
			if (frameTimer >= 1.0f)
			{
				currentFrame = (currentFrame + 1) % frameCountIdle; 
				frameTimer = 0.0f;
			}
		}

		if (isHit) // activate attack anim
		{
			hitTimer += GetFrameTime();
			if (hitTimer >= 0.4f) //after 0,4 second return to idle anim
			{
				isHit = false;
				hitTimer = 0.0f;
				currentFrame = 0; //reset frame
			}
		}

		Rectangle source;
		if (isHit)
		{
			source = { 2 * 96.0f, 0.0f, 96.0f, 96.0f }; //attack
		}
		else
		{
			source = { currentFrame * 96.0f, 0.0f, 96.0f, 96.0f }; //idle
		}

		Rectangle dest = { position.x, position.y, 96.0f * scale, 96.0f * scale };
		Vector2 origin = { -415, -420 };

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}

	void OnHit()
	{
		isHit = true;
		currentFrame = 0;
		hitTimer = 0.0f;
	}

private:
	Texture2D texture;
	int currentFrame;
	int frameCountIdle;
	float animationSpeed;
	float frameTimer;
	float scale;
	bool isHit;       
	float hitTimer;   //attack timer  
};
class Collision14 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionThirteen[8] = {
	168, 650,
	182, 636,
	185, 643,
	175, 652
	};

	Collision14(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionThirteen, 8), _listener)
		, texture(_texture)
	{
		collisionType = BOTTON1;
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

class Collision18 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionThirteen[8] = {
	370, 492,
	386, 499,
	385, 503,
	370, 497
	};

	Collision18(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionThirteen, 8), _listener)
		, texture(_texture)
	{
		collisionType = BOTTONDERECHO;
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

class Collision17 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionCentralBotton[8] = {
	259, 423,
	283, 423,
	283, 428,
	260, 428
	};

	Collision17(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionCentralBotton, 8), _listener)
		, texture(_texture)
	{
		collisionType = BOTTONCENTRAL;
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

class Collision16 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionPuntuacionCyndaquil[22] = {
	199, 448,
	208, 440,
	216, 437,
	226, 435,
	235, 433,
	241, 433,
	243, 439,
	232, 443,
	219, 447,
	209, 450,
	203, 451
	};

	Collision16(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture)
		: PhysicEntity(physics->CreateChain(0, 0, CollisionPuntuacionCyndaquil, 22), _listener)
		, texture(_texture)
	{
		collisionType = CYNDAQUIL;
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

class Collision15 : public PhysicEntity
{
public:
	// Pivot 0, 0
	static constexpr int CollisionThirteen[8] = {
	168, 650,
	182, 636,
	185, 643,
	175, 652
	};

	Collision15(ModulePhysics* physics, int x, int y, Module* listener, Texture2D texture)
		: PhysicEntity(physics->CreateRectangleSensor(x, y, 40, 0), listener), texture(texture)
	{
		collisionType = PUERTAROTANTE;
		frameCountIdle = 1;      
		frameCountRotating = 15; 
		currentFrame = 0;
		animationSpeed = 0.3f;   
		frameTimer = 0.0f;
		isRotating = false;
		scale = 1.3f;
	}

	void ActivateRotation() override {
		isRotating = true;
		currentFrame = 0; 
	}

	void Update() override {
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };

		if (isRotating) {

			frameTimer += animationSpeed;
			if (frameTimer >= 1.0f) {
				currentFrame++;
				frameTimer = 0.0f;

				if (currentFrame >= frameCountRotating) {
					isRotating = false;
					currentFrame = 0;
				}
			}
		}


		Rectangle source;
		if (isRotating) {
			source = { currentFrame * 48.0f, 0.0f, 48.0f, 48.0f };
		}
		else {
			source = { 0.0f, 0.0f, 48.0f, 48.0f };  
		}

		Rectangle dest = { position.x, position.y, 48.0f * scale, 48.0f * scale };
		Vector2 origin = { 24.0f * scale, 24.0f * scale };

		DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
	}

private:
	Texture2D texture;
	int currentFrame;
	int frameCountIdle;
	int frameCountRotating;
	float animationSpeed;
	float frameTimer;
	bool isRotating;
	float scale;
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

	circle = LoadTexture("Assets/pokeballAnim.png"); 

	spring = LoadTexture("Assets/muelle1.png");

	leftPad=LoadTexture("Assets/leftFlipper.png");

	rightPad = LoadTexture("Assets/RightFlipper.png");

	chinchou = LoadTexture("Assets/chinchouAnim2.png");

	trianguloizq = LoadTexture("Assets/TrianguloIzqAnim.png");  

	gulpin = LoadTexture("Assets/GulpinAnim.png");

	nuzleaf = LoadTexture("Assets/nuzleaf.png");

	wishcash = LoadTexture("Assets/wishcash.png");

	cyndaquil = LoadTexture("Assets/cyndaquil.png");

	sharpedo = LoadTexture("Assets/sharpedo.png");

	pikachu = LoadTexture("Assets/pikachu.png");

	pichu = LoadTexture("Assets/pichu.png");

	latios = LoadTexture("Assets/Latios.png");

	puertarotante = LoadTexture("Assets/puertarotante.png");
	
	bonus_fx = App->audio->LoadFx("Assets/Po.wav");
	backgroundMusic = LoadMusicStream("Assets/19-Red-Table.ogg");

	App->fontsModule->LoadFontTexture("Assets/fonts16x32.png", '0',16);

	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+25, SCREEN_WIDTH, 50);
	PlayMusicStream(backgroundMusic);
	entities.emplace_back(new LeftPad(App->physics,215,940, this, leftPad));
	entities.emplace_back(new RightPad(App->physics,360,940, this, rightPad));
	entities.emplace_back(new Spring(App->physics,SCREEN_WIDTH - 30, SCREEN_HEIGHT - 100, 30, 50, this, spring));
	entities.emplace_back(new Chinchou(App->physics, 358, 320, this, chinchou));
	entities.emplace_back(new Chinchou(App->physics, 306, 371, this, chinchou));
	entities.emplace_back(new Chinchou(App->physics, 377, 392, this, chinchou));
	entities.emplace_back(new TrianguloIzq(App->physics, 358, 320, this, botton1));
	entities.emplace_back(new Pikachu(App->physics, 528, 940, this, pikachu));
	entities.emplace_back(new Pichu(App->physics, 107, 944, this, pichu));
	entities.emplace_back(new Gulpin(App->physics, 358, 320, this, gulpin)); 
	entities.emplace_back(new Wishcash(App->physics, 358, 320, this, wishcash));
	entities.emplace_back(new Nuzleaf(App->physics, 358, 320, this, nuzleaf));
	entities.emplace_back(new DeleteSensor(App->physics, SCREEN_WIDTH / 2, SCREEN_HEIGHT + 35, this, collision1));


	entities.emplace_back(new Collision15(App->physics, 520, 500, this, puertarotante));
	entities.emplace_back(new TrianguloIzqColPunt(App->physics, 520, 500, this, botton1)); 


	entities.emplace_back(new Collision1(App->physics, 0, 0, this, collision1)); //Mapa
	entities.emplace_back(new Collision2(App->physics, 0, 0, this, collision2)); //L azul abajo izquierda
	entities.emplace_back(new Collision3(App->physics, 0, 0, this, collision3)); //L azul abajo derecha
	entities.emplace_back(new Collision4(App->physics, 0, 0, this, collision4)); //Triangulo der rojo
	entities.emplace_back(new Collision5(App->physics, 0, 0, this, trianguloizq)); //Triangulo izq rojo
	entities.emplace_back(new Collision6(App->physics, 0, 0, this, cyndaquil)); //Piedra grande y pokemon verde
	entities.emplace_back(new GreenEvoD(App->physics, 0, 0, this, GreenEvoDer)); 
	entities.emplace_back(new GreenOneI(App->physics, 0, 0, this, GreenOneIzq)); // Arriba derecha chinchous
	entities.emplace_back(new Collision7(App->physics, 0, 0, this, collision7)); // Columna derecha arriba chinchous
	entities.emplace_back(new Collision8(App->physics, 0, 0, this, collision8)); //Columna derecha arriba chinchous
	entities.emplace_back(new Collision12(App->physics, 0, 0, this, collision12)); //Abajo derecha chinchous
	entities.emplace_back(new Collision13(App->physics, 0, 0, this, sharpedo)); //Tiburon
	entities.emplace_back(new Collision14(App->physics, 0, 0, this, botton1));
	entities.emplace_back(new Collision16(App->physics, 0, 0, this, botton1)); // Para meter colision puntuacion cyndaquil
	entities.emplace_back(new Collision17(App->physics, 0, 0, this, botton1)); // Para meter colision puntuacion boton central
	entities.emplace_back(new Collision18(App->physics, 0, 0, this, botton1)); // Para meter colision puntuacion boton derecho
	
	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");
	UnloadTexture(circle);
	UnloadTexture(chinchou);
	UnloadTexture(gulpin);
	UnloadTexture(nuzleaf);
	UnloadTexture(wishcash);
	UnloadTexture(cyndaquil);
	UnloadTexture(sharpedo);
	UnloadTexture(pikachu);
	UnloadTexture(pichu);
	UnloadTexture(puertarotante);
	UnloadTexture(latios);
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

	if (IsKeyPressed(KEY_ONE)) {
		entities.emplace_back(new Circle(App->physics, GetMousePosition().x, GetMousePosition().y, this, circle));
	}

	std::vector<PhysicEntity*> newEntities;

	for (auto it = entities.begin(); it != entities.end(); ++it) {
		Latios* latios = dynamic_cast<Latios*>(*it);

		if (latios != nullptr) {
			if (latios->hasToSpawnBall && !latios->pokeballSpawned) {
				newEntities.emplace_back(new Circle(App->physics, SCREEN_WIDTH - 45, 790, this, circle));

				// Marcar como creada para evitar más spawns
				latios->pokeballSpawned = true;
			}
		}
	}
	entities.insert(entities.end(), newEntities.begin(), newEntities.end());



	if (IsKeyPressed(KEY_TWO))
	{
		for (auto it = entities.begin(); it != entities.end(); )
		{
			// Verificamos si el tipo es Circle usando dynamic_cast
			if (dynamic_cast<Circle*>(*it) != nullptr)
			{
				it = entities.erase(it); // Eliminar instancia de Circle
			}
			else
			{
				++it; // Continuar con el siguiente elemento
			}
		}
	}

	if (IsKeyPressed(KEY_THREE)) {
		entities.emplace_back(new Latios(App->physics, 0, 740, this, latios, 5));
	}

	if (lives > 0) {
		if (deleteCircles) {
			lives--;
			entities.emplace_back(new Latios(App->physics, 0, 740, this, latios, 5));
		}

		if (lives <= 0) {
			lives = 0;
		}
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
	if (ray_on == true)
	{
		vec2f destination((float)(mouse.x - ray.x), (float)(mouse.y - ray.y));
		destination.Normalize();
		destination *= (float)ray_hit;

		DrawLine(ray.x, ray.y, (int)(ray.x + destination.x), (int)(ray.y + destination.y), RED);

		if (normal.x != 0.0f)
		{
			DrawLine((int)(ray.x + destination.x), (int)(ray.y + destination.y), (int)(ray.x + destination.x + normal.x * 25.0f), (int)(ray.y + destination.y + normal.y * 25.0f), Color{ 100, 255, 100, 255 });
		}
	}
	/*App->fontsModule->DrawText(10, 10, TextFormat("%d", suma), WHITE);*/
	/*App->fontsModule->DrawText(10, 30, TextFormat("Lives: %d", lives), WHITE);*/

	DrawText(TextFormat("Score: %d", suma), 10, 10, 30, BLACK);
	DrawText(TextFormat("Lives: %d", lives), 10, 40, 30, BLACK);

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	bool hascollisionedwithchinchou = false;
	bool hascollisionedwithbotton1 = false;
	bool hascollisionedwithsharpedos = false;
	bool hascollisionedwithpuertarotante = false;
	bool hascollisionedwithcyndaquil = false;
	bool hascollisionedwithbotoncentral = false;
	bool hascollisionedwithbotonderecho = false; 
	bool hascollisionedwithtrianguloizq = false;

	deleteCircles = false;

	int length = entities.size();
	for (int i = 0; i < length; ++i) {
		

		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == DEFAULT) {
			App->audio->PlayFx(bonus_fx);
			break;
		}

		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == CHINCHOU ){
			hascollisionedwithchinchou = true;
			entities[i]->ActivateHit();
		    break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == BOTTON1) {
			hascollisionedwithbotton1 = true;
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == BOTTONDERECHO) { 
			hascollisionedwithbotonderecho = true;
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == BOTTONCENTRAL) {
			hascollisionedwithbotoncentral = true; 
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == CYNDAQUIL) {
			hascollisionedwithcyndaquil = true; 
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == TRIANGULOIZQ) {
			hascollisionedwithtrianguloizq = true; 
			entities[i]->ActivateHit();
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == SHARPEDO) {
			hascollisionedwithsharpedos = true;
			entities[i]->ActivateHit();
			break;
		}
		if (bodyA == entities[i]->body && entities[i]->GetCollisionType() == PUERTAROTANTE) {
			hascollisionedwithpuertarotante = true;
			entities[i]->ActivateRotation();
			break;
		}
		if (bodyA == entities[i]->body &&  entities[i]->GetCollisionType() == SENSOR && entities[i]->GetSensor() == DELETE) {
			deleteCircles = true;
			break;
		}

	}
	if (deleteCircles) {

		for (auto it = entities.begin(); it != entities.end(); )
		{
			// Verificamos si el tipo es Circle usando dynamic_cast
			if (dynamic_cast<Circle*>(*it) != nullptr)
			{
				it = entities.erase(it); // Eliminar instancia de Circle
			}
			else
			{
				++it; // Continuar con el siguiente elemento
			}
		}
	}
	if (hascollisionedwithtrianguloizq) {
		for (int i = 0; i < entities.size(); ++i) {
			Collision5* collision5 = dynamic_cast<Collision5*>(entities[i]);
			if (collision5 != nullptr) {
				collision5->ActivateHit();  
			}
		}
	}
	if (hascollisionedwithchinchou) suma += 500;
	if (hascollisionedwithbotton1) suma += 1000;
	if (hascollisionedwithbotonderecho) suma += 2000; 
	if (hascollisionedwithtrianguloizq) suma += 2000; 
	if (hascollisionedwithbotoncentral) suma += 1000; 
	if (hascollisionedwithcyndaquil) suma += 5000; 
	if (hascollisionedwithsharpedos) suma += 2000;
	if (hascollisionedwithpuertarotante) suma += 100;

  // Reproduce el sonido
}