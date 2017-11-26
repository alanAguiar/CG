#ifndef SHIP_H
#define SHIP_H

#include "gameobject.h"
#include "bullet.h"

#include <list>

class Ship : public GameObject
{
public:
    Ship(QOpenGLWidget *_glWidget);
    void update();
    float leftInput = 0, rightInput = 0, upInput = 0, downInput = 0;
    void shoot();
    void shotTaken();
    int lifes = 3;
    std::list<Bullet *> bulletsShot;
    std::list<Bullet *> fireParticles;
    QVector3D tilt;

private:
    QTime shootDelay;
    bool aux = false;
};

#endif // SHIP_H
