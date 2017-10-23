#ifndef SHIP_H
#define SHIP_H

#include "model.h"
#include "bullet.h"

#include <list>

class Ship : public Model
{
public:
    Ship(QOpenGLWidget *_glWidget);
    void updatePosition();
    float leftInput = 0, rightInput = 0, upInput = 0, downInput = 0;
    void shoot();
    void shotTaken();
    int lifes = 3;
    std::list<Bullet *> bulletsShot;
private:
    QTime shootDelay;
};

#endif // SHIP_H
