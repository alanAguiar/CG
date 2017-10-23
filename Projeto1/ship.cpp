#include "ship.h"

Ship::Ship(QOpenGLWidget *_glWidget) : Model(_glWidget)
{
    this->position = QVector2D(0, -0.75);
    this->scale = 0.1;
    this->rotation = 0;
    this->readOFFFile(":/models/spaceship.off");
    this->createVBOs();
    this->createShaders(":/shaders/shipVertexShader.glsl", ":/shaders/fshader1.glsl");
    this->shootDelay.start();

    this->hitbox.adjust(0.025, 0.025, -0.025, -0.025);
}

void Ship::updatePosition()
{
    position.setX(position.x() - leftInput/50 + rightInput/50);
    position.setY(position.y() - downInput/50 + upInput/50);
    if(position.x() < -0.9)
        position.setX(-0.9);
    if(position.x() >0.9)
        position.setX(0.9);
    if(position.y() < -0.9)
        position.setY(-0.9);
    if(position.y() > 0.1)
        position.setY(0.1);

    this->hitbox.moveCenter(QPointF(position.x(), position.y()));
}

void Ship::shoot()
{
    if(this->shootDelay.elapsed() > 300)
    {
        shootDelay.start();
        Bullet *b = new Bullet(glWidget, this->position, QVector2D(0, 0.1), 0.01);
        bulletsShot.push_back(b);
    }
}

void Ship::shotTaken()
{
    this->lifes -= 1;
}
