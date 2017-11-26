#ifndef BULLET_H
#define BULLET_H

#include "gameobject.h"

class Bullet : public GameObject
{
public:
    Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, int textureID);
    Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, QVector2D force, int textureID);
    Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, QVector2D force, int ttl, float decay, int textureID);
    void update();
    void setColor(QVector4D);
    int timeToLive;
    float decay;
    int textureID;
    static const int PLAYER_BULLET = 0;
    static const int BOSS_BULLET = 1;
    static const int FIRE = 2;
    void draw();
private:

};

#endif // BULLET_H
