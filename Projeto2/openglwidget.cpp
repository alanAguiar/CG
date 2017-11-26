#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateObjects()));
    this->maxScore = 0;
    QPushButton* s = new QPushButton(this);
    s->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));
    s->setText("f");
    s->resize(100, 100);

}

void OpenGLWidget::startGame()
{
    timer->start(15);
    this->score = 0;
    inGame = true;

    ship = std::make_unique<Ship>(this);
    boss = std::make_unique<Boss>(this);
    bg1 = std::make_unique<Background>(this, QVector3D(0, 0, -0.5));
    bg2 = std::make_unique<Background>(this, QVector3D(0, 3, -0.5));
    emit showLifes(QString("Vidas: \%1").arg(ship->lifes));
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qDebug("OpenGL  version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    camera.resizeViewport(width, height);
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(inGame)
    {
        paintModel(boss.get());
        boss->drawModel();

        if(!invincible || (delay.elapsed()/100) % 2 == 0){
            paintModel(ship->model);
            ship->draw();

            for(std::list<Bullet *>::iterator it = ship->fireParticles.begin(); it!=ship->fireParticles.end(); it++){
                paintModel((*it)->model);
                (*it)->draw();
            }
        }        

        paintModel(bg1->model);
        bg1->draw();
        bg2->draw();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end(); it++){
            (*it)->draw();
        }

        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end(); it++){
            (*it)->draw();
        }
    }
}

void OpenGLWidget::paintModel(Model* model)
{
    QVector4D  ambientProduct = light.ambient*model->material.ambient;
    QVector4D  diffuseProduct = light.diffuse*model->material.diffuse;
    QVector4D  specularProduct = light.specular*model->material.specular;

    GLuint  locProjection = glGetUniformLocation(model->shaderProgram , "projection");
    GLuint  locView = glGetUniformLocation(model->shaderProgram , "view");
    GLuint  locLightPosition = glGetUniformLocation(model->shaderProgram , "lightPosition");
    GLuint  locAmbientProduct = glGetUniformLocation(model->shaderProgram , "ambientProduct");
    GLuint  locDiffuseProduct = glGetUniformLocation(model->shaderProgram , "diffuseProduct");
    GLuint  locSpecularProduct = glGetUniformLocation(model->shaderProgram , "specularProduct");
    GLuint  locShininess = glGetUniformLocation(model->shaderProgram , "shininess");

    glUseProgram(model->shaderProgram);
    glUniformMatrix4fv(locProjection , 1, GL_FALSE, camera.projectionMatrix.data());
    glUniformMatrix4fv(locView , 1, GL_FALSE , camera.viewMatrix.data());
    glUniform4fv(locLightPosition , 1, &(light.position [0]));
    glUniform4fv(locAmbientProduct , 1, &(ambientProduct [0]));
    glUniform4fv(locDiffuseProduct , 1, &(diffuseProduct [0]));
    glUniform4fv(locSpecularProduct , 1, &(specularProduct [0]));
    glUniform1f(locShininess , model->material.shininess);
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
   // qDebug("Ship:%d Fire:%d Boss:%d \n", ship->bulletsShot.size(), ship->fireParticles.size(), boss->bulletsShot.size());

    if(ship->lifes == 0)
        endGame(false);

    else if(boss->lifes == 0)
        endGame(true);
    else
    {
        ship->update();
        boss->shoot();
        bg1->update();
        bg2->update();

        if(invincible && delay.elapsed() > 1500)
            invincible = false;

        if(shooting)
            ship->shoot();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end();){
            if((*it)->position.y() > 1 || (*it)->position.y() < -1){
                delete(*it);
                it = ship->bulletsShot.erase(it);
            }
            else{
                (*it)->update();
                if((*it)->hitbox.intersects(boss->hitbox)){
                    boss->shotTaken();
                    this->score += 100 * 5/boss->lifes;
                    emit showScore(QString("Pontuação: \%1").arg(this->score));
                    delete(*it);
                    it = ship->bulletsShot.erase(it);
                }
                else
                    it++;
            }
        }

        for(std::list<Bullet *>::iterator it = ship->fireParticles.begin(); it!=ship->fireParticles.end();){
            (*it)->update();
            if((*it)->timeToLive <= 0 || (*it)->scale <= 0){
                delete(*it);
                it = ship->fireParticles.erase(it);
            }
            else
                it++;
        }

        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end();){
            if((*it)->position.y() > 1.2 || (*it)->position.y() < -1.2 || (*it)->position.x() > 1.2 || (*it)->position.x() < -1.2){
                delete(*it);
                it = boss->bulletsShot.erase(it);
            }
            else{
                (*it)->update();
                if((*it)->hitbox.intersects(ship->hitbox) && !invincible){
                    qDebug("PLAYER: %f %f\n", (*it)->position.y(), (float)ship->hitbox.center().y());
                    ship->shotTaken();
                    emit showLifes(QString("Vidas: \%1").arg(ship->lifes));
                    delete(*it);
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
