#include "Weapons/Shotgun.hpp"
#include <System/GameRender.hpp>
#include <System/GameWorld.hpp>
#include "Props/Props.hpp"
#include "System/Utils.hpp"

std::vector<size_t> Shotgun::m_shootSounds;

Shotgun::Shotgun()
{
    m_fireCooldown = Cooldown(0.05f);
    _recoil = 0.0f;

    _shootAnimation = Animation(3, 1, 312, 206, 3, 10);
    _shootAnimation.setTextureID(_shootTextureID);
    _shootAnimation.setCenter(sf::Vector2f(96, 120));

    _moveAnimation = Animation(3, 7, 313, 206, 20, 40);
    _moveAnimation.setTextureID(_moveTextureID);
    _moveAnimation.setCenter(sf::Vector2f(96, 120));

    _idleAnimation = Animation(3, 7, 313, 207, 20, 30);
    _idleAnimation.setTextureID(_idleTextureID);
    _idleAnimation.setCenter(sf::Vector2f(96, 120));

    _fireDist = 2.2f;
}

bool Shotgun::fire(GameWorld* world, WorldEntity* entity)
{
    if (m_fireCooldown.isReady())
    {
        _recoil += 0.15f;
        _recoil = _recoil>1.0f?1.0f:_recoil;

        m_fireCooldown.reset();

        float entityAngle = entity->getAngle();

        int openAngle = 30;
        for (int i(0); i<12; ++i)
        {
            float angle = static_cast<float>(rand()%openAngle-openAngle/2);
            float speed = (rand()%50+125)/100.0f;
            Bullet* newBullet = Bullet::add(angle*DEGRAD, speed*CELL_SIZE, 15.0f, 30);
            newBullet->init(entity->getCoord(), entityAngle);
            world->addEntity(newBullet);

            Vec2 bulletVel(newBullet->getV());
            Vec2 smokePos = newBullet->getCoord()+bulletVel*1.25f;
            float v(rand()%50/1000.0f+0.1f);
            world->addEntity(Smoke::add(smokePos, bulletVel*v, 0.05f, 75.0f));

            Vec2 firePos(newBullet->getCoord()+bulletVel*_fireDist);
            world->addEntity(Fire::add(firePos, entityAngle+PIS2));

            SoundPlayer::playInstanceOf(m_shootSounds[getRandInt(0, 2)]);
            std::cout << "Playing sound\n";  // 添加這行輸出語句
        }

        return true;
    }

    return false;
}

void Shotgun::reload()
{

}

void Shotgun::update()
{
    _recoil -= DT;
    _recoil  = std::max(0.0f, _recoil);

    m_fireCooldown.update(DT);
}

void Shotgun::init()
{
    _shootTextureID = GameRender::registerTexture("data/textures/hunter/hunter_shotgun_shoot.png");
    _moveTextureID  = GameRender::registerTexture("data/textures/hunter/hunter_shotgun_move.png");
    _idleTextureID  = GameRender::registerTexture("data/textures/hunter/hunter_shotgun_idle.png");

    // 添加以下代碼以初始化射擊音效
    m_shootSounds.push_back(SoundPlayer::registerSound("data/fire1.wav"));
    m_shootSounds.push_back(SoundPlayer::registerSound("data/fire2.wav"));
    m_shootSounds.push_back(SoundPlayer::registerSound("data/fire3.wav"));

    // 檢查 `m_shootSounds` 大小
    std::cout << "Number of sounds: " << m_shootSounds.size() << std::endl;
}
