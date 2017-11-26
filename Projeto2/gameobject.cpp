#include "gameobject.h"

GameObject::GameObject(QOpenGLWidget* _glWidget)
{
    this->_glWidget = _glWidget;
}

void GameObject::update()
{
    position.setX(position.x() + speed.x());
    position.setY(position.y() + speed.y());
    speed += force;
    this->hitbox.moveCenter(QPointF(position.x(), position.y()));
}


void GameObject::draw()
{
    model->position = this->position;
    model->color = this->color;
    model->scale = this->scale;
    model->rotation = this->rotation;
    model->drawModel();
}
