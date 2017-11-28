#include "boss.h"

Boss::Boss(QOpenGLWidget* _glWidget) : GameObject(_glWidget)
{
    srand(time(NULL));

    this->lifes = 5;

    this->position = QVector3D(0, 0.8, 0); //1.3
    this->scale = 0.6; //1.2
    this->rotation = QVector3D(30, 0, 0); //180
    this->healthbar = 100;
    this->currentColor = QVector4D(0, 1, 1, 0);
    this->toNextColor = QVector4D(0, -0.25, -1, 0);

    this->hitbox.setTopLeft(this->hitbox.topLeft()*scale);
    this->hitbox.setBottomRight(this->hitbox.bottomRight()*scale);
    this->hitbox.moveCenter(QPointF(position.x(),position.y()+0.6));

    this->bulletSpawn = new QVector3D[spawnNumber];
    for(int i=0; i<spawnNumber; i++)
        bulletSpawn[i] = QVector3D(this->hitbox.x()+0.085+(i*(this->hitbox.width()-0.17))/spawnNumber, this->hitbox.y()-0.1, 0);
    shootDelay.start();
    waveDelay.start();

    QString textures[3] = {":/textures/bossIdle.png", ":/textures/bossAttack.png", ":/textures/bossHit.png"};
    static Model* m = new Model(_glWidget, ":/models/cromulon.obj", ":/shaders/shipfshader.glsl",
                                            ":/shaders/shipVertexShader.glsl", textures, 3);
    this->model = m;
    this->model->material.shininess = 10;
    this->model->material.diffuse = QVector4D(1, 1, 1, 1);
    this->model->material.specular = QVector4D(0.3, 0.3, 0.3, 1);
}

void Boss::shoot()
{    
    int time = waveDelay.elapsed();
    if(time > 6500)
    {
        shooting = true;
        state = 1;
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
        if(time <= 3000 && lifes<5)
        {
            state = 2;
            if(time<=250 || (time>750 && time<=1250) || (time>1750 && time<=2250) || time > 2750)
                position.setX(position.x() - 0.02);
            else
                position.setX(position.x() + 0.02);
        }
        else if(time > 5000)
        {
            state = 0;
            if((time/100)%2 == 0)
                position.setY(position.y() + 0.003);
            else
                position.setY(position.y() - 0.003);
        }
    }
}

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

void Boss::shootPattern1()
{
    if(healthbar > 0)
    {
        if(shootDelay.elapsed() > 500)
        {
            for(int i= int(pattAux2+0.5); i<spawnNumber; i+=2)
            {
                shootDelay.start();
                Bullet *b = new Bullet(_glWidget, bulletSpawn[i], QVector2D(0, -0.0025), 0.05, QVector2D(bulletSpawn[i].x()/10000, -0.00008), Bullet::BOSS_BULLET);
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
        if(shootDelay.elapsed() > 50*qPow(bulletSpawn[pattAux1].x(), 2))
        {
            shootDelay.start();
            Bullet *b = new Bullet(_glWidget, bulletSpawn[pattAux1]+QVector3D(0, -0.045, 0), QVector2D(0, -0.0025), 0.035, QVector2D((pattAux2/5.0+bulletSpawn[pattAux1].x())/15000, -0.000025), Bullet::BOSS_BULLET);
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
                Bullet *b = new Bullet(_glWidget, bulletSpawn[i], QVector2D(0, -0.0025), 0.02, QVector2D((bulletSpawn[i].x()>0? -0.000025 : 0.000025)+bulletSpawn[i].x()/10000.0, -0.5/(10000*speed)), Bullet::BOSS_BULLET);
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
            Bullet *b = new Bullet(_glWidget, bulletSpawn[pattAux1] + QVector3D(0.05 * pattAux2, 0, 0), QVector2D(0, -0.0033), 0.03, QVector2D(qPow(bulletSpawn[pattAux1].x(), 2)*pattAux2/4000, -qFabs(bulletSpawn[pattAux1].x())/20000), Bullet::BOSS_BULLET);
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
            Bullet *b = new Bullet(_glWidget, bulletSpawn[pattAux1], QVector2D(0, -0.004), 0.025, QVector2D(bulletSpawn[pattAux1].x()/100000, 0), Bullet::BOSS_BULLET);
            b->setColor(currentColor/3 + QVector4D(-0.5, 1, -1, 0)*qFabs(bulletSpawn[pattAux1].x()));
            bulletsShot.push_back(b);
            pattAux1 = rand()%spawnNumber;
        }
    }
    else
        reset();
}

void Boss::shotTaken()
{
    if(shooting)
        this->healthbar -= 0.5*lifes;
}

void Boss::draw()
{

//    GLuint locModelMatrix = glGetUniformLocation(shaderProgram , "model");
//    glUniformMatrix4fv(locModelMatrix , 1, GL_FALSE , modelMatrix.data());

//    GLuint loc = glGetUniformLocation(shaderProgram, "life");
//    glUniform1f(loc, healthbar);

//    loc = glGetUniformLocation(shaderProgram, "currentColor");
//    glUniform4f(loc, currentColor.x(), currentColor.y(), currentColor.z(), currentColor.w());

//    loc = glGetUniformLocation(shaderProgram, "toNextColor");
//    glUniform4f(loc, toNextColor.x(), toNextColor.y(), toNextColor.z(), toNextColor.w());

    this->model->textureID = state;
    GameObject::draw();

}
