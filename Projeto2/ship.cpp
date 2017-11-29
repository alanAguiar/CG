#include "ship.h"

Ship::Ship(QOpenGLWidget *_glWidget):GameObject(_glWidget)
{
    this->position = QVector3D(0, -0.75, 0);
    this->scale = 0.12;
    this->rotation = QVector3D(0, 0, 0);
    this->shootDelay.start();

    this->hitbox.setTopLeft(this->hitbox.topLeft()*scale);
    this->hitbox.setBottomRight(this->hitbox.bottomRight()*scale);
    this->hitbox.adjust(0.025, 0.025, -0.025, -0.025);

    QString textures = ":/textures/shiptexture.png";
    static Model* m = new Model(_glWidget, ":/models/spaceship.obj", ":/shaders/shipfshader.glsl",
                                        ":/shaders/shipVertexShader.glsl", &textures, 1);


    this->hitbox.setTopLeft(QPointF(-0.5*scale, 0.5 * scale));
    this->hitbox.setBottomRight(QPointF(0.5*scale,-0.5*scale));
    this->hitbox.moveCenter(QPointF(position.x(), position.y()));

    this->model = m;
    this->model->material.diffuse = QVector4D(1, 1, 1, 1);
    this->model->material.shininess = 200;
}

void Ship::update()
{
    position.setX(position.x() - leftInput/100 + rightInput/100);
    position.setY(position.y() - downInput/100 + upInput/100);

    if(position.x() < -0.9)
        position.setX(-0.9);
    if(position.x() >0.9)
        position.setX(0.9);
    if(position.y() < -0.9)
        position.setY(-0.9);
    if(position.y() > 0.1)
        position.setY(0.1);

    this->hitbox.moveCenter(QPointF(position.x(), position.y()));

    if(leftInput == 1 && rightInput == 0)
    {
        if(this->rotation.y() > -40)
            this->rotation.setY(this->rotation.y() - 10);
    }
    else if(rightInput == 1 && leftInput == 0)
    {
        if(this->rotation.y() < 40)
            this->rotation.setY(this->rotation.y() + 10);
    }
    else if(this->rotation.y() < 0)
        this->rotation.setY(this->rotation.y() + 10);
    else if(this->rotation.y() > 0)
        this->rotation.setY(this->rotation.y() - 10);

    Bullet *f = new Bullet(_glWidget, this->position+QVector3D(0, -0.09, 0), QVector2D(0, -0.02), (aux?0.05:0.03), QVector2D(0, 0), 8, 0.2, Bullet::FIRE);
    this->fireParticles.push_back(f);
    aux = !aux;

}

void Ship::shoot()
{
    if(this->shootDelay.elapsed() > 300)
    {
        shootDelay.start();
        Bullet *b = new Bullet(_glWidget, this->position, QVector2D(0, 0.05), 0.03, Bullet::PLAYER_BULLET);
        bulletsShot.push_back(b);
    }
}

void Ship::shotTaken()
{
    this->lifes -= 1;
}
