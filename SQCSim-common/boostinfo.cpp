#include "boostinfo.h"
#include <iostream>

BoostInfo::BoostInfo(BoostType type, const std::string& name, float u, float v, float s, int dur) : m_type(type), m_name(name), m_u(u), m_v(v), m_s(s), m_durability(dur)
{
}

BoostInfo::~BoostInfo()
{
}

BoostType BoostInfo::GetType() const
{
    return m_type;
}

void BoostInfo::GetTexture(float& u, float& v, float& s)
{
    u = m_u;
    v = m_v;
    s = m_s;
}

void BoostInfo::Show() const
{
    std::cout << "Type: " << m_type << std::endl;
    std::cout << "Nom: " << m_name << std::endl;
    std::cout << "Durabilite: " << m_durability << std::endl;
    std::cout << "Coordonnees Texture: " << m_u << ", " << m_v << ", " << m_s << std::endl;
}


