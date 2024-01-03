#include "booster.h"

Booster::Booster()
{
}

Booster::Booster(Vector3f tpos, BOOST_TYPE ttype, uint64_t id): m_id(id), pos(tpos), type(ttype){}

Booster::~Booster()
{

}

Vector3f Booster::GetPosition()
{
	return pos;
}

BOOST_TYPE Booster::GetType()
{
	return type;
}

uint64_t Booster::GetId() const
{
	return m_id;
}

bool Booster::GetAvailability()
{
	return available;
}

void Booster::SetAvailability(bool value)
{
	available = value;
	modified = true;
}

