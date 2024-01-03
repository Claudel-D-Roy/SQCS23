#ifndef PLAYER_H__
#define PLAYER_H__

#include <cmath>
#include <unordered_map>
#include "transformation.h"
#include "vector3.h"
#include "booster.h"
#include "define.h"


class World; 

class Player {
public:
	enum Sound { NOSOUND, STEP, FALL };
	//enum BoosterType { SPEED, HEAL, DAMAGE, INVINCIBLE };

	Player(const Vector3f& position, float rotX = 0, float rotY = 0);
	~Player();

	void TurnLeftRight(float value, float sensitivity);
	void TurnTopBottom(float value, float sensitivity);
	Vector3f GetInput(bool front, bool back, bool left, bool right, bool jump, bool dash, float elapsedTime);
	Sound ApplyPhysics(Vector3f input, World* world, float elapsedTime);
	uint64_t TakeBooster(std::unordered_map<uint64_t, Booster*> booster_table, float elapsedTime);
	void GetBooster(BOOST_TYPE boosttype, float elapsedTime);
	void RemoveBooster(float elapsedtime);
	void ApplyTransformation(Transformation& transformation, bool rel = true, bool rot = true) const;

	void SetDirection(Vector3f dir);
	void Move(Vector3f diff);
	Vector3f GetPosition() const;
	Vector3f GetPositionAbs() const;
	Vector3f GetDirection() const;
	Vector3f GetVelocity() const;
	Vector3f GetPOV() const;
	std::string GetUsername() const;
	void SetUsername(std::string username);
	float GetSensitivity() const;
	void SetSensitivity(float sensitivity);
	float GetHP() const;
	void SetHP(float hp);
	void Teleport(int& x, int& z);

	bool GetIsAirborne() const;
	bool AmIDead();
	void InflictDamage(float hitPoints);
	int getScore() const;
	void addPoint();
	uint64_t Killer = 0;
	std::string m_username;
	bool m_hit = false;

	bool Eulogy = false;

	bool boostspeed = false;
	bool boostdamage = false;
	bool boostinvincible = false;

	void Put(Vector3f pos);

private:
	uint64_t getId() const;

protected:
	Vector3f m_position;
	Vector3f m_velocity;
	Vector3f m_direction;

	uint64_t id = 0;
	int m_score = 0;

	float m_rotX = 0; 
	float m_rotY = 0;
	float m_POV;
	float timeboostspeed;
	float timeboostdamage;
	float timeboostinvincible;
	float m_sensitivity;

	float m_hp;

	bool m_airborne;

	Vector3f InterpolatePosition(const Vector3f& vec1, const Vector3f& vec2, const Timestamp& tim1, const Timestamp& tim2, const Timestamp& now);
};
#endif //_PLAYER_H__

