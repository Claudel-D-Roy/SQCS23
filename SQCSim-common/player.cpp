#include "player.h"
#include "world.h"

Player::Player(const Vector3f& position, float rotX, float rotY) : m_position(position), m_rotX(rotX), m_rotY(rotY) {
	m_velocity = Vector3f(0, 0, 0);
	m_airborne = true;
	m_hp = 1.0f;
	m_sensitivity = 0.5f;
	m_username = "Zelda Bee-Bop56";
}

Player::~Player() {}

void Player::TurnLeftRight(float value, float sensitivity) {
	m_rotY += value * sensitivity;
	if (m_rotY > 360) m_rotY = 0;
	else if (m_rotY < -360) m_rotY = 0;

	float yrotrad = (m_rotY / 57.2957795056f); // 180/Pi = 57.295...
	float xrotrad = (m_rotX / 57.2957795056f);

	m_direction = Vector3f(cos(xrotrad) * sin(yrotrad),
		-sin(xrotrad),
		cos(xrotrad) * -cos(yrotrad));

	m_direction.Normalize();
}

void Player::TurnTopBottom(float value, float sensitivity) {
	m_rotX += value * sensitivity;
	if (m_rotX > 80) m_rotX = 80;
	else if (m_rotX < -80) m_rotX = -80;

	float yrotrad = (m_rotY / 57.2957795056f); // 180/Pi = 57.295...
	float xrotrad = (m_rotX / 57.2957795056f);

	m_direction = Vector3f(cos(xrotrad) * sin(yrotrad),
		-sin(xrotrad),
		cos(xrotrad) * -cos(yrotrad));

	m_direction.Normalize();
}

Vector3f Player::GetInput(bool front, bool back, bool left, bool right, bool jump, bool shoot, float elapsedTime) {
	Vector3f delta = Vector3f(0, 0, 0);

	Vector3f dir = m_direction;

	dir.y = 0;

	if (front) {
		delta += dir;
	}
	else if (back) {
		delta -= dir;
	}

	if (left) {
		delta.x += dir.z;
		delta.z += -dir.x;
	}
	else if (right) {
		delta.x -= dir.z;
		delta.z -= -dir.x;
	}

	delta.Normalize();
	delta.x *= .6f;
	delta.z *= .6f;

	if ((jump || shoot) && !m_airborne) {
		delta.y += jump ? .32f : shoot ? .1f : 0.f;
		m_airborne = true;
	}
	if (boostspeed)
	{
		delta.x += STRENGTH_SPEED_BOOST / 100 * delta.x;
		delta.z += STRENGTH_SPEED_BOOST / 100 * delta.z;
	}

	if (shoot) // Recoil!
		TurnTopBottom(-1, 1.0f);

	return delta;
}

Player::Sound Player::ApplyPhysics(Vector3f input, World* world, float elapsedTime) {
	Player::Sound snd = Player::Sound::NOSOUND;
	static float timing = 0.f;
	/* Gestion de collisions */
	BlockType bt1, bt2, bt3;

	bt1 = world->BlockAt(GetPosition().x, GetPosition().y + input.y, GetPosition().z);
	bt2 = world->BlockAt(GetPosition().x, GetPosition().y + input.y - 0.9f, GetPosition().z);
	bt3 = world->BlockAt(GetPosition().x, GetPosition().y + input.y - 1.7f, GetPosition().z);
	if ((bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR) && m_position.y < 129.7f) {
		bt1 = world->BlockAt(GetPosition().x, GetPosition().y + .3f, GetPosition().z);
		if (bt1 == BTYPE_AIR) m_position.y = (int)m_position.y + .7f;
		m_velocity.y = input.y = 0;
		m_airborne = false;
	}
	else {
		if (abs(m_velocity.y) < 1.1f) m_velocity.y += input.y - 1.1f * elapsedTime;
		bt3 = world->BlockAt(GetPosition().x, GetPosition().y + m_velocity.y - 1.7f, GetPosition().z);
		bt1 = world->BlockAt(GetPosition().x, GetPosition().y + .3f, GetPosition().z);
		if (bt3 != BTYPE_AIR) {
			m_velocity.y = 0;
			if (timing == 0.f) {
				if (m_airborne) snd = Player::Sound::FALL;
				timing = .3f;
			}
			m_airborne = false;
		}
		else if (bt1 != BTYPE_AIR) {
			m_velocity.y = -.1f;
		}
		else m_airborne = true;
	}

	if (timing > 0.f) timing -= elapsedTime;
	if (timing < 0.f) timing = 0.f;

	bt1 = world->BlockAt(GetPosition().x + input.x, GetPosition().y, GetPosition().z);
	bt2 = world->BlockAt(GetPosition().x + input.x, GetPosition().y - 0.9f, GetPosition().z);
	bt3 = world->BlockAt(GetPosition().x + input.x, GetPosition().y - 1.7f, GetPosition().z);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR) {
		m_velocity.y += .04f;
		m_velocity.z *= .5f;
		m_velocity.x *= .5f;
	}

	bt1 = world->BlockAt(GetPosition().x, GetPosition().y, GetPosition().z + input.z);
	bt2 = world->BlockAt(GetPosition().x, GetPosition().y - 0.9f, GetPosition().z + input.z);
	bt3 = world->BlockAt(GetPosition().x, GetPosition().y - 1.7f, GetPosition().z + input.z);
	if (bt1 != BTYPE_AIR || bt2 != BTYPE_AIR || bt3 != BTYPE_AIR) {
		m_velocity.y += .04f;
		m_velocity.z *= .5f; 
		m_velocity.x *= .5f;
	}

	/* Fin gestion de collisions */
	/* Gestion de la friction */

	if (!m_airborne) {
		m_velocity.x += input.x * (boostspeed ? 2.5f : 2.f) * elapsedTime;
		m_velocity.z += input.z * (boostspeed ? 2.5f : 2.f) * elapsedTime;

		if (input.x == 0.f)
			m_velocity.x *= .8f;

		if (input.z == 0.f)
			m_velocity.z *= .8f;
	}
	else {
		m_velocity.x += input.x * .4f * elapsedTime; // Techniquement contre les lois de la physique, mais c'est beaucoup moins chiant pour grimper sur les blocs.
		m_velocity.z += input.z * .4f * elapsedTime;
		m_velocity.x *= .99f;
		m_velocity.z *= .99f;
	}

	/* Fin gestion de la friction */

	float vy = m_velocity.y;
	m_velocity.y = boostspeed ? 0.f: 1.f; // Padding pour limiter le x et z lors du Normalize().
	if (m_velocity.Length() >= 1.f) m_velocity.Normalize(); // Limiteur de vitesse en x/z.
	m_velocity.y = 0;
	if (m_velocity.Length() < .005f) m_velocity.Zero(); // Threshold en x/z.
	m_velocity.y = vy;

	m_position += m_velocity;

	static float bobbingtime = 0; // Gestion de la cam�ra
	static bool leftright = false;
	static bool isStep = false;
	if (bobbingtime <= 360.f)
		bobbingtime += elapsedTime * 20.f; else bobbingtime = 0;

	if ((sin(bobbingtime) - 0.5f) * (abs(m_velocity.x) + abs(m_velocity.z)) < -.2f && !m_airborne) {
		if (!isStep) {
			snd = Player::Sound::STEP;
		}
		isStep = true;
	}
	else isStep = false;
	m_POV = m_position.y;
	m_POV += m_airborne ? 0 : (sin(bobbingtime) - 0.5f) * (abs(m_velocity.x) + abs(m_velocity.z)) * .2f;
	RemoveBooster(elapsedTime);
	return snd;
}

void Player::Put(Vector3f pos) {
	m_position = pos;
}

void Player::ApplyTransformation(Transformation& transformation, bool rel, bool rot) const {

	transformation.ApplyRotation(-m_rotX, 1, 0, 0);
	transformation.ApplyRotation(-m_rotY, 0, 1, 0);
	if (rel) transformation.ApplyTranslation(-GetPOV());
}

uint64_t Player::TakeBooster(std::unordered_map<uint64_t, Booster*> booster_table, float elapsedtime)
{
	uint64_t boostid = 0;
	Vector3f playerpos = GetPosition();
	for (auto& [key, booster]: booster_table) {
		Vector3f pos = booster->GetPosition();
		if (booster->GetAvailability() && abs(playerpos.x - pos.x) <= 0.5f && abs(playerpos.y - pos.y) <= 1.0f && abs(playerpos.z - pos.z) <= 0.5f)
		{
			boostid = booster->GetId();
			GetBooster(booster->GetType(), elapsedtime);
			booster->SetAvailability(false);
			break;
		}
	}
	return boostid;
}

void Player::GetBooster(BOOST_TYPE boosttype, float elapsedtime)
{
	if (boosttype == BTYPE_SPEED)
	{
		boostspeed = true;
		timeboostspeed = 0;
	}
	if (boosttype == BTYPE_HEAL)
	{
		m_hp = 1.0f;
	}
	if (boosttype == BTYPE_DAMAGE)
	{
		boostdamage = true;
		timeboostdamage = 0;
	}
	if (boosttype == BTYPE_INVINCIBLE)
	{
		boostinvincible = true;
		boostinvincible = 0;
	}
}
void Player::RemoveBooster(float elapsedtime)
{
	if (boostspeed)
	{
		timeboostspeed += elapsedtime;
		if (timeboostspeed >= TIME_SPEED_BOOST && boostspeed == true)
			boostspeed = false;
	}
	if (boostdamage)
	{
		if (timeboostdamage >= TIME_DAMAGE_BOOST && boostdamage == true)
			boostdamage = false;
	}
	if (boostinvincible)
	{
		if (timeboostinvincible >= TIME_INVINCIBLE_BOOST && boostinvincible == true)
			boostinvincible = false;
	}
}
void Player::SetDirection(Vector3f dir) { m_direction = dir; }

void Player::Move(Vector3f diff) { m_position -= diff; }

Vector3f Player::GetPosition() const { return Vector3f(m_position.x + CHUNK_SIZE_X * WORLD_SIZE_X / 2, m_position.y, m_position.z + CHUNK_SIZE_Z * WORLD_SIZE_Y / 2); }

Vector3f Player::GetPositionAbs() const { return m_position; }

Vector3f Player::GetVelocity() const { return m_velocity; }

Vector3f Player::GetPOV() const { return Vector3f(GetPosition().x, m_POV, GetPosition().z); }

Vector3f Player::GetDirection() const { return m_direction; }

std::string Player::GetUsername() const { return m_username; }

void Player::SetUsername(std::string username) {
	m_username = username;
}

float Player::GetSensitivity() const {
	return m_sensitivity;
}

void Player::SetSensitivity(float sensitivity) {
	m_sensitivity = sensitivity;
}

float Player::GetHP() const { return m_hp; }

void Player::SetHP(float hp) {
	m_hp = hp;
}

void Player::Teleport(int& x, int& z) {
	m_position.x -= x * CHUNK_SIZE_X;
	m_position.z -= z * CHUNK_SIZE_Z;
}
bool Player::GetIsAirborne() const { return m_airborne; }

bool Player::AmIDead() { return m_hp <= 0; }

void Player::InflictDamage(float hitPoints) {
	m_hp -= hitPoints;

	if (m_hp < 0)
		m_hp == 0;
}

int Player::getScore() const { return m_score; }

void Player::addPoint() { ++m_score; }


uint64_t Player::getId() const { return id; }

Vector3f Player::InterpolatePosition(const Vector3f& vec1, const Vector3f& vec2, const Timestamp& tim1, const Timestamp& tim2, const Timestamp& now) {
	return Vector3f();
}
