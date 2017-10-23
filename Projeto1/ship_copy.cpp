#include "ship.h"

Ship::Ship(QOpenGLWidget *_glWidget) : Model(_glWidget)
{
    this->position = QVector2D(0, -0.75);
    this->scale = 0.1;
    this->readOFFFile(":/models/spaceship.off");
    this->createVBOs();
    this->createShaders(":/shaders/shipVertexShader.glsl", ":/shaders/fshader1.glsl");
}

void Ship::updatePosition()
{
    position.setX(position.x() - leftInput/35 + rightInput/35);
    position.setY(position.y() - downInput/35 + upInput/35);
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
