#ifndef SHOTGUN_HPP_INCLUDED
#define SHOTGUN_HPP_INCLUDED

#include "WeaponStatics.hpp"

class Shotgun : public WeaponStatics<Shotgun>
{
public:
    Shotgun();

    bool fire(GameWorld* world, WorldEntity* entity);
    void reload();
    void update();

    static void init();
private:
    // 在 Shotgun 類別中的 private 成員變數區域添加以下變數
    static std::vector<size_t> m_shootSounds;
};

#endif // SHOTGUN_HPP_INCLUDED
