#include "boss.h"

Boss::Boss(QOpenGLWidget* _glWidget) : Model(_glWidget)
{
    srand(time(NULL));

    //Set the attributes with default values
    this->lifes = 5;
    this->position = QVector2D(0, 1.4); //1.3
    this->scale = 0.8; //1.2
    this->rotation = 180; //180
    this->healthbar = 100;
    this->currentColor = QVector4D(0, 1, 1, 0);
    this->toNextColor = QVector4D(0, -0.25, -1, 0);

    //Read the object model
    this->readOFFFile(":/models/enemyShip.off");
    this->createVBOs();
    this->createShaders(":/shaders/bossVShader.glsl", ":/shaders/fshader1.glsl");

    //Adjust the hitbox properly
    this->hitbox.moveCenter(QPointF(position.x(), position.y()+0.6));

    //Create the bullet spawns, defining the coordinates from where the bullets will be shot
    this->bulletSpawn = new QVector2D[spawnNumber];
    for(int i=0; i<spawnNumber; i++)
        bulletSpawn[i] = QVector2D(this->hitbox.x()+0.085+(i*(this->hitbox.width()-0.17))/spawnNumber, this->hitbox.y()-0.1);

    //Start timers
    shootDelay.start();
    waveDelay.start();
}

void Boss::shoot()
{    
    int time = waveDelay.elapsed();
    //Sets a delay time between the shooting waves. The shooting pattern is selected according to the remaining lifes
    if(time > 6500)
    {
        shooting = true;
        switch (lifes) {
        case 5:
            shootPattern1();
            break;
        case 4:
            shootPattern2();
            break;
        case 3:
            shootPattern3();
            break;
        case 2:
            shootPattern4();
            break;
        case 1:
            shootPattern5();
            break;
        default:
            break;
        }
    }
    else
    {
        shooting = false;
        pattAux1 = 0;
        pattAux2 = 1;

        //Creates an horizontal movement of the model when the healthbar hits 0.
        if(time <= 3000 && lifes<5)
        {
            if(time<=250 || (time>750 && time<=1250) || (time>1750 && time<=2250) || time > 2750)
                position.setX(position.x() - 0.02);
            else
                position.setX(position.x() + 0.02);
        }

        //Creates an vertical movement before starting a new shooting wave
        else if(time > 5000)
        {
            if((time/100)%2 == 0)
                position.setY(position.y() + 0.003);
            else
                position.setY(position.y() - 0.003);
        }
    }
}

//Method called when the healthbar hits 0. Decrement the lifes, reset the healthbar and set the colors accordingly
void Boss::reset()
{
    this->lifes --;
    this->healthbar = 100;
    currentColor += toNextColor;
    switch (lifes) {
    case 4:
        toNextColor = QVector4D(1, 0.25, 0, 0);
        break;
    case 3:
        toNextColor = QVector4D(0, -1, 0, 0);
        break;
    case 2:
        toNextColor = QVector4D(0, 0, 1, 0);
        break;
    case 1:
        toNextColor = QVector4D(0, 1, 0, 0);
        break;
    }
    waveDelay.start();
}


//When hit, updates the healthbar if the object is in a shooting state
void Boss::shotTaken()
{
    if(shooting)
        this->healthbar -= 0.5*lifes;
}

//Overwrites the drawModel method, in order to add IN parameters to the shader
void Boss::drawModel()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position.x(), position.y(), 0);
    modelMatrix.scale(invDiag * scale , invDiag * scale , invDiag * scale);
    modelMatrix.rotate(rotation, 0, 0, 1);

    glBindVertexArray(vao);
    glUseProgram(shaderProgram);

    GLuint locModelMatrix = glGetUniformLocation(shaderProgram , "model");
    glUniformMatrix4fv(locModelMatrix , 1, GL_FALSE , modelMatrix.data());

    GLuint loc = glGetUniformLocation(shaderProgram, "life");
    glUniform1f(loc, healthbar);

    loc = glGetUniformLocation(shaderProgram, "currentColor");
    glUniform4f(loc, currentColor.x(), currentColor.y(), currentColor.z(), currentColor.w());

    loc = glGetUniformLocation(shaderProgram, "toNextColor");
    glUniform4f(loc, toNextColor.x(), toNextColor.y(), toNextColor.z(), toNextColor.w());

    glDrawElements(GL_TRIANGLES , numFaces * 3, GL_UNSIGNED_INT,0);

}

//The following methods define all the shooting patterns
void Boss::shootPattern1()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 500)
        {
            for(int i= int(pattAux2+0.5); i<spawnNumber; i+=2)
            {
                shootDelay.start();
                Bullet *b = new Bullet(glWidget, bulletSpawn[i], QVector2D(0, -0.005), 0.05, QVector2D(bulletSpawn[i].x()/10000, -0.00015));
                b->setColor(currentColor + toNextColor*qFabs(bulletSpawn[i].x()));
                bulletsShot.push_back(b);
            }
            pattAux2 *= -1;
        }
    }
    else
        reset();
}

void Boss::shootPattern2()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 60*qPow(bulletSpawn[pattAux1].x(), 2))
        {
            shootDelay.start();
            Bullet *b = new Bullet(glWidget, bulletSpawn[pattAux1]+QVector2D(0, -0.045), QVector2D(0, -0.005), 0.035, QVector2D((pattAux2/5.0+bulletSpawn[pattAux1].x())/8000, -0.00005));
            b->setColor(currentColor + QVector4D(0, -1, 0, 0)*qFabs(bulletSpawn[pattAux1].x()));
            bulletsShot.push_back(b);
            pattAux1 += pattAux2;
            if(pattAux1 <= 0 || pattAux1 >= spawnNumber-1)
              pattAux2 *= -1;
        }
    }
    else
        reset();
}


void Boss::shootPattern3()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 800)
        {
            int speed = rand()%100 + 1;
            for(int i= 1; i<spawnNumber; i++)
            {
                Bullet *b = new Bullet(glWidget, bulletSpawn[i], QVector2D(0, -0.005), 0.02, QVector2D((bulletSpawn[i].x()>0? -0.00005 : 0.00005)+bulletSpawn[i].x()/10000.0, -1.0/(10000*speed)));
                b->setColor(currentColor + QVector4D(-0.5, -1, 1  , 0)*speed/100);
                bulletsShot.push_back(b);
            }
            shootDelay.start();
        }
    }
    else
        reset();
}

void Boss::shootPattern4()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 15)
        {
            shootDelay.start();
            Bullet *b = new Bullet(glWidget, bulletSpawn[pattAux1] + QVector2D(0.05 * pattAux2, 0), QVector2D(0, -0.0065), 0.03, QVector2D(qPow(bulletSpawn[pattAux1].x(), 2)*pattAux2/2000, -qFabs(bulletSpawn[pattAux1].x())/10000));
            b->setColor(currentColor/2 + toNextColor*qFabs(bulletSpawn[pattAux1].x()));
            bulletsShot.push_back(b);
            pattAux1 = pattAux1 + pattAux2;

            if(pattAux1 <= 0 || pattAux1 >= spawnNumber - 1)
              pattAux2 *= -1;
        }
    }
    else
        reset();
}

void Boss::shootPattern5()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 80)
        {
            shootDelay.start();
            shootDelay.addMSecs(rand()%50);
            Bullet *b = new Bullet(glWidget, bulletSpawn[pattAux1], QVector2D(0, -0.008), 0.025, QVector2D(bulletSpawn[pattAux1].x()/50000, 0));
            b->setColor(currentColor/3 + QVector4D(-0.5, 1, -1, 0)*qFabs(bulletSpawn[pattAux1].x()));
            bulletsShot.push_back(b);
            pattAux1 = rand()%spawnNumber;
        }
    }
    else
        reset();
}
