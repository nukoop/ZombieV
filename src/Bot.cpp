#include "Bot.hpp"
#include "System/GameWorld.hpp"

Bot::Bot() :
    HunterBase(0, 0),
    m_target(nullptr)
{

}

Bot::Bot(float x, float y) :
    HunterBase(x, y),
    m_target(nullptr),
    m_getTargetCount(0)
{

}

void Bot::update(GameWorld& world)
{
    computeControls(world);
    _currentWeapon->update();

    Vec2 pos(getCoord());
    _time += DT;

    if (_state == SHOOTING)
    {
        if (!_currentWeapon->fire(&world, this))
        {
            _changeState(IDLE);
            if (_currentWeapon->isMagEmpty())
            {
                _changeAnimation(_currentWeapon->getReloadAnimation(), false);
                _state = RELOADING;
            }
        }
    }

    if (_state == RELOADING && _currentAnimation.isDone())
    {
        _currentWeapon->reload();
        _state = IDLE;
    }

    _shootLight->radius = 0;
    if (_state == SHOOTING)
    {
        bool wait = _lastState==SHOOTING;
        _changeAnimation(_currentWeapon->getShootAnimation(), wait);
        _shootLight->radius = 350;
    }
    else if (_state == MOVING)
    {
        bool wait = !(_lastState==IDLE);
        _changeAnimation(_currentWeapon->getMoveAnimation(), wait);
    }
    else
    {
        _changeAnimation(_currentWeapon->getIdleAnimation());
    }

    _shootLight->position = _currentWeapon->getFireOutPosition(this);
    /*_flashlight->position = _shootLight->position;
    _littleLight->position = _shootLight->position;
    _flashlight->angle = getAngle()+PI;*/
}

void Bot::computeControls(GameWorld& world)
{
    if (m_target)
    {
        Vec2 vTarget(m_target->getCoord(), getCoord());
        Vec2 direction(cos(_angle), sin(_angle));
        Vec2 directionNormal(-direction.y, direction.x);

        float dist = vTarget.getNorm();
        float vx = vTarget.x/dist;
        float vy = vTarget.y/dist;

        float dot2 = vx*directionNormal.x + vy*directionNormal.y;
        float coeff = 0.25f;

        float absDot = std::abs(dot2);
        coeff *= absDot;

        if (absDot<0.25f || dist < 100)
        {
            if (dist < 300)
            {
                _changeState(SHOOTING);
                if (dist < 100)
                {
                    _feetTime += DT;
                    _body.stop();
                    _body.accelerate2D(Vec2(vx*_speed, vy*_speed));
                }
            }
            else
            {
                float speedFactor = 0.25f;
                _body.stop();
                _body.accelerate2D(Vec2(-vx*_speed*speedFactor, -vy*_speed*speedFactor));
                _feetTime += DT*speedFactor;
            }
        }
        else
        {
            _changeState(IDLE);
        }

        _angle += dot2>0?-coeff:coeff;

        if (m_target->isDying())
            m_target = nullptr;
    }
    else
    {
        _changeState(IDLE);
        getTarget(&world);
    }
}

void Bot::getTarget(GameWorld* world)
{
    ++m_getTargetCount;
    Vector<Zombie> zombies = Zombie::getObjects();

    Zombie* target = nullptr;
    float minDist  = -1;

    int skip = 2;
    int step(m_getTargetCount%skip);
    int i(0);
    for (Ptr<Zombie>& zombie : zombies)
    {
        ++i;
        if ((i+step)%skip==0)
        {
            Vec2 v(zombie->getCoord(), getCoord());
            float dist = v.getNorm2();

            if (dist < minDist|| minDist < 0)
            {
                minDist = dist;
                target = &(*zombie);
            }
        }
    }

    if (target)
    {
        m_target = target;
    }
}

void Bot::hit(WorldEntity* entity, GameWorld* gameWorld)
{
    switch(entity->getType())
    {
        case(EntityTypes::ZOMBIE) :
        {
            m_target = entity;
            break;
        }
        default:
            break;
    }
}
