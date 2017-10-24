#include "ship.h"

Ship::Ship(QOpenGLWidget *_glWidget) : Model(_glWidget)
{
    //Set the attributes with default value
    this->position = QVector2D(0, -0.75);
    this->scale = 0.1;
    this->rotation = 0;

    //Load the model
    this->readOFFFile(":/models/spaceship.off");
    this->createVBOs();
    this->createShaders(":/shaders/shipVertexShader.glsl", ":/shaders/fshader1.glsl");
    this->shootDelay.start();

    //Adjust the hitbox to fit the model better
    this->hitbox.adjust(0.025, 0.025, -0.025, -0.025);
}

void Ship::updatePosition()
{
    //Update the position accordingly with the user's input
    position.setX(position.x() - leftInput/50 + rightInput/50);
    position.setY(position.y() - downInput/50 + upInput/50);

    //Set the boundaries of movement
    if(position.x() < -0.9)
        position.setX(-0.9);
    if(position.x() >0.9)
        position.setX(0.9);
    if(position.y() < -0.9)
        position.setY(-0.9);
    if(position.y() > 0.1)
        position.setY(0.1);

    //Update the hitbox position
    this->hitbox.moveCenter(QPointF(position.x(), position.y()));
}

void Ship::shoot()
{
    //Sets a delay between shots
    if(this->shootDelay.elapsed() > 300)
    {
        shootDelay.start();
        //Instantiate a new bullet and push it to the bullets list
        Bullet *b = new Bullet(glWidget, this->position, QVector2D(0, 0.1), 0.01);
        bulletsShot.push_back(b);
    }
}

//Decrease the lifes if a shot is taken
void Ship::shotTaken()
{
    this->lifes -= 1;
}
