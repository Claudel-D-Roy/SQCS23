#ifndef BOOSTER_H__
#define BOOSTER_H__
#include "define.h"
#include "vector3.h"

class Booster {
public:
	Booster();
	Booster(Vector3f tpos, BOOST_TYPE ttype, uint64_t id);
	~Booster();
	Vector3f GetPosition();
	BOOST_TYPE GetType();
	uint64_t GetId() const;
	bool GetAvailability();
	void SetAvailability(bool value);
	bool modified = true;
private:
	Vector3f pos;
	BOOST_TYPE type;
	uint64_t m_id;
	bool available = true;
};
#endif
