#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{      
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateObjects()));
    this->maxScore = 0;
}

void OpenGLWidget::startGame()
{
    timer->start(30);
    this->score = 0;
    inGame = true;
    ship = std::make_unique<Ship>(this);
    boss = std::make_unique<Boss>(this);
    emit showLifes(QString("Vidas: \%1").arg(ship->lifes));
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qDebug("OpenGL  version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(inGame)
    {
        if(!invincible || (delay.elapsed()/100) % 2 == 0)
            ship->drawModel();
        boss->drawModel();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end(); it++)
            (*it)->drawModel();

        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end(); it++)
            (*it)->drawModel();
    }
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        ship->upInput = 1;
    if (event->key() == Qt::Key_Down)
        ship->downInput = 1;
    if (event->key() == Qt::Key_Right)
        ship->rightInput = 1;
    if (event->key() == Qt::Key_Left)
        ship->leftInput = 1;
    if (event->key() == Qt::Key_Space)
        shooting = true;
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        ship->upInput = 0;
    if (event->key() == Qt::Key_Down)
        ship->downInput = 0;
    if (event->key() == Qt::Key_Right)
        ship->rightInput = 0;
    if (event->key() == Qt::Key_Left)
        ship->leftInput = 0;
    if (event->key() == Qt::Key_Space)
        shooting = false;
    if (event->key() == Qt::Key_G)
    {
        invincible = true;
        delay.start();
    }
}

void OpenGLWidget::updateObjects()
{
    if(ship->lifes == 0)
        endGame(false);

    else if(boss->lifes == 0)
        endGame(true);

    else
    {
        ship->updatePosition();
        boss->shoot();

        if(invincible && delay.elapsed() > 1500)
            invincible = false;

        if(shooting)
            ship->shoot();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end();){
            if((*it)->position.y() > 1 || (*it)->position.y() < -1)
                it = ship->bulletsShot.erase(it);
            else{
                (*it)->updatePosition();
                if((*it)->hitbox.intersects(boss->hitbox)){
                    boss->shotTaken();
                    this->score += 100 * 5/boss->lifes;
                    emit showScore(QString("Pontuação: \%1").arg(this->score));
                    it = ship->bulletsShot.erase(it);
                }
                else
                    it++;
            }
        }

        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end();){
            if((*it)->position.y() > 1 || (*it)->position.y() < -1 || (*it)->position.x() > 1 || (*it)->position.x() < -1)
                it = boss->bulletsShot.erase(it);
            else{
                (*it)->updatePosition();
                if((*it)->hitbox.intersects(ship->hitbox) && !invincible){
                    ship->shotTaken();
                    emit showLifes(QString("Vidas: \%1").arg(ship->lifes));
                    it = boss->bulletsShot.erase(it);
                    invincible = true;
                    delay.start();
                }
                else
                    it++;
            }
        }
    }

    update();
}

void OpenGLWidget::endGame(bool won)
{
    this->inGame = false;
    timer->stop();
    if(this->score > this->maxScore)
        this->maxScore = this->score;
    if(won)
        emit setText(QString("Você venceu! \n\nSua pontuação: \%1 \n\nMaior Pontuação: \%2").arg(this->score).arg(this->maxScore));
    else
        emit setText(QString("Você perdeu! \n\nSua pontuação: \%1 \n\nMaior Pontuação: \%2").arg(this->score).arg(this->maxScore));
    emit enableText();
    emit enableStartButton();
    emit enableQuitButton();
}
