#include "background.h"

Background::Background(QOpenGLWidget *_glWidget, QVector3D position) : GameObject(_glWidget)
{
    this->position = position;
    this->speed = QVector2D(0, -0.005);
    this->force = QVector2D(0, 0);
    this->scale = 2.2;
    this->rotation = QVector3D(0, 0, 0);
    this->color = QVector4D(1, 1, 1, 1);

    QString texture = ":/textures/background.png";
    static Model* m = new Model(_glWidget, ":/models/plane.obj", ":/shaders/bulletfshader.glsl",
                            ":/shaders/bulletVShader.glsl", &texture, 1);
    this->model = m;
}

void Background::update(){
    GameObject::update();
    if(this->position.y() <= -3)
        this->position.setY(3);
}
