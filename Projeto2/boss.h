#ifndef BOSS_H
#define BOSS_H

#include "gameobject.h"
#include "bullet.h"
#include <list>

class Boss : public GameObject
{
public:
    Boss(QOpenGLWidget *_glWidget);
    std::list<Bullet *> bulletsShot;    
    int lifes;
    int state = 0;
    void shoot();
    void shotTaken();
    void draw();
private:
    QVector3D* bulletSpawn;
    float healthbar;
    int pattAux1 = 0, pattAux2 = 1;
    const int spawnNumber = 12;
    bool shooting = false;
    QTime shootDelay;
    QTime waveDelay;
    QVector4D currentColor;
    QVector4D toNextColor;
    void reset();
    void shootPattern1();
    void shootPattern2();
    void shootPattern3();
    void shootPattern4();
    void shootPattern5();
};

#endif // BOSS_H
