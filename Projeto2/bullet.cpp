#include "bullet.h"

Bullet::Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, int textureID) : GameObject(_glWidget)
{
    this->position = position;
    this->speed = speed;    
    this->force = QVector2D(0, 0);
    this->scale = size;
    this->rotation = QVector3D(0, 0, 0);
    this->color = QVector4D(1, 1, 1, 1);
    this->timeToLive = 1000000;
    this->decay = 0;
    this->textureID = textureID;

    this->hitbox.setTopLeft(QPointF(-0.5*scale, 0.5 * scale));
    this->hitbox.setBottomRight(QPointF(0.5*scale,-0.5*scale));
    this->hitbox.adjust(0.025, 0.025, -0.025, -0.025);


    QString textures[3] = {":/textures/playerBullet.png", ":/textures/bullet.png", ":/textures/fire.png"};
    static Model* m = new Model(_glWidget, ":/models/plane.obj", ":/shaders/bulletfshader.glsl",
                            ":/shaders/bulletVShader.glsl", textures, 3);
    this->model = m;
}

Bullet::Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, QVector2D force, int textureID) :  Bullet(_glWidget, position, speed, size, textureID)
{
    this->force = force;
}

Bullet::Bullet(QOpenGLWidget *_glWidget, QVector3D position, QVector2D speed, float size, QVector2D force, int ttl, float decay, int textureID) :  Bullet(_glWidget, position, speed, size, textureID)
{
    this->force = force;
    this->timeToLive = ttl;
    this->decay = decay;
}

void Bullet::update()
{
    this->timeToLive--;
    this->scale -= this->scale*this->decay;
    if(this->textureID == BOSS_BULLET){
        this->rotation.setZ(((int)this->rotation.z()+20) % 360);
        this->color.setW(1);
    }
    GameObject::update();
}

void Bullet::setColor(QVector4D c)
{
    this->color = c;
}

void Bullet::draw()
{
    this->model->textureID = textureID;
    GameObject::draw();
}

