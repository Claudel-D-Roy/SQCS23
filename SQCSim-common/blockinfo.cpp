#include "blockinfo.h"
#include <iostream>

BlockInfo::BlockInfo(BlockType type, const std::string& name, float u, float v, float s, int dur) : m_type(type), m_name(name), m_u(u), m_v(v), m_s(s), m_durability(dur)
{
}

BlockInfo::~BlockInfo()
{
}

BlockType BlockInfo::GetType() const
{
    return m_type;
}

void BlockInfo::SetDurability(int durability)
{
    m_durability = durability;
}

int BlockInfo::GetDurability() const
{
    return m_durability;
}

void BlockInfo::GetTexture(float& u, float& v, float& s)
{
    u = m_u;
    v = m_v;
    s = m_s;
}

void BlockInfo::Show() const
{
    std::cout << "Type: " << m_type << std::endl;
    std::cout << "Nom: " << m_name << std::endl;
    std::cout << "Durabilite: " << m_durability << std::endl;
    std::cout << "Coordonnees Texture: " << m_u << ", " << m_v << ", " << m_s << std::endl;
}


