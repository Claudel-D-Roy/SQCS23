#ifndef BOOSTINFO_H__
#define BOOSTINFO_H__

#include <string>
#include "define.h"

class BoostInfo
{
public:
    BoostInfo(BoostType type, const std::string& name, float u, float v, float s, int dur);
    ~BoostInfo();

    BoostType GetType() const;

    void GetTexture(float& u, float& v, float& s);

    void Show() const;

private:
    BoostType m_type;
    float m_u;
    float m_v;
    float m_s;
    std::string m_name;
    int m_durability;

};

#endif // BOOSTINFO_H__
