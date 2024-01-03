#ifndef BLOCKINFO_H__
#define BLOCKINFO_H__

#include <string>
#include "define.h"

class BlockInfo
{
    public:
    BlockInfo(BlockType type, const std::string& name, float u, float v, float s, int dur);
    ~BlockInfo();

    BlockType GetType() const;

    void SetDurability(int durability);
    int GetDurability() const;

    void GetTexture(float& u, float& v, float& s);

    void Show() const;

    private:
    BlockType m_type;
    float m_u;
    float m_v;
    float m_s;
    std::string m_name;
    int m_durability;

};

#endif // BLOCKINFO_H__
