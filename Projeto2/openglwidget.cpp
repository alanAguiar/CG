#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    timer = new QTimer(this);
    this->maxScore = 0;

    initializeUI();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateObjects()));
}

void OpenGLWidget::startGame()
{
    camera.up = QVector3D(0, 1, 0);
    camera.center = QVector3D(0, 0, 0);
    camera.eye = QVector3D(0, 0, 2);
    camera.computeViewMatrix();

    this->score = 0;
    inGame = true;

    ship = std::make_unique<Ship>(this);
    boss = std::make_unique<Boss>(this);
    bg1 = std::make_unique<Background>(this, QVector3D(0, 0, -0.5));
    bg2 = std::make_unique<Background>(this, QVector3D(0, 3, -0.5));
    emit showScore(QString("\%1").arg(0, 10, 10, QChar('0')));
    emit showLifes(QString("Lifes: \%1").arg(ship->lifes));
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qDebug("OpenGL  version: %s", glGetString(GL_VERSION));
    qDebug("GLSL %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    showMenu();
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
        paintModel(boss->model);
        boss->draw();

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
    else if(inMenu)
    {
        camera.up = QVector3D(0, 0, 1);
        camera.center = QVector3D(0.05, 0, 0);
        camera.eye = QVector3D(-0.1, 0.2, 0.1);
        camera.computeViewMatrix();

        paintModel(ship->model);
        ship->draw();

        for(std::list<Bullet *>::iterator it = ship->fireParticles.begin(); it!=ship->fireParticles.end(); it++){
            (*it)->rotation = QVector3D(0, -45, 0);
            (*it)->position.setZ(0.01);
            paintModel((*it)->model);
            (*it)->draw();
        }
        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end(); it++){
            (*it)->rotation = QVector3D(0, -90, 0);
            (*it)->scale = 0.02;
            (*it)->draw();
        }
        paintModel(bg1->model);
        bg1->draw();
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
    if(inMenu || inGame)
    {
        ship->update();
        for(std::list<Bullet *>::iterator it = ship->fireParticles.begin(); it!=ship->fireParticles.end();){
            (*it)->update();
            if((*it)->timeToLive <= 0 || (*it)->scale <= 0){
                delete(*it);
                it = ship->fireParticles.erase(it);
            }
            else
                it++;
        }
        if(shooting)
            ship->shoot();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end();){
            if((*it)->position.y() > 1 || (*it)->position.y() < -1){
                delete(*it);
                it = ship->bulletsShot.erase(it);
            }
            else{
                (*it)->update();
                it++;
            }
        }
    }

    if(inGame)
    {
        if(ship->lifes == 0)
            endGame(false);

        else if(boss->lifes == 0)
            endGame(true);
        else
        {
            boss->shoot();
            boss->update();
            bg1->update();
            bg2->update();

            if(invincible && delay.elapsed() > 1500)
                invincible = false;

            for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end();)
            {
                if((*it)->hitbox.intersects(boss->hitbox)){
                    boss->shotTaken();
                    this->score += 100 * 5/boss->lifes;
                    emit showScore(QString("\%1").arg(this->score, 10, 10, QChar('0')));
                    delete(*it);
                    it = ship->bulletsShot.erase(it);
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
                        ship->shotTaken();
                        emit showLifes(QString("Lifes: \%1").arg(ship->lifes));
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
    }
    update();
}

void OpenGLWidget::endGame(bool won)
{
    this->inGame = false;
    this->inMenu = false;
    timer->stop();
    emit showScore(QString(""));
    emit showLifes(QString(""));
    if(this->score > this->maxScore)
        this->maxScore = this->score;

    if(won)
       instructionsText->setText(QString("I LIKE WHAT YOU GOT! \n\nScore: \%1 \n\nMax Score: \%2").arg(this->score).arg(this->maxScore));
    else
       instructionsText->setText(QString("DISQUALIFIED! \n\nScore: \%1 \n\nMax Score: \%2").arg(this->score).arg(this->maxScore));
    instructionsText->show();
    backButton->show();
}

void OpenGLWidget::closeWindow(){
    qApp->quit();
}

void OpenGLWidget::showInstructions()
{
    startButton->hide();
    quitButton->hide();
    instructionsButton->hide();

    instructionsText->setText("Use the ARROWS to move\nUse the SPACEBAR to shoot");
    instructionsText->show();
    backButton->show();
}

void OpenGLWidget::showMenu()
{
    timer->start(15);
    this->inMenu = true;
    ship = std::make_unique<Ship>(this);
    ship->position = QVector3D(0, 0, 0);
    bg1 = std::make_unique<Background>(this, QVector3D(0.6, -1.6, 0));
    bg1->scale = 3.3;
    bg1->rotation = QVector3D(105, 45, -5);

    instructionsText->hide();
    backButton->hide();

    startButton->show();
    quitButton->show();
    instructionsButton->show();
}

void OpenGLWidget::initializeUI()
{
    startButton = new QPushButton("Start", this);
    instructionsButton = new QPushButton("Instructions", this);
    quitButton = new QPushButton("Quit", this);
    backButton = new QPushButton("Back", this);
    instructionsText = new QLabel(this);
    lifeText = new QLabel(this);
    scoreText = new QLabel(this);

    startButton->setGeometry(20, 200, 200, 100);
    startButton->setStyleSheet("QPushButton{border:0px; background: transparent; text-align:left; font:italic 40pt; font-weight: bold; color: #ffffff}QPushButton:hover{border:0px; background: transparent; text-align:left; font:italic 48pt; font-weight: bold; color: #ffffff}");
    connect(startButton, SIGNAL(clicked(bool)), this, SLOT(startGame()));
    connect(startButton, SIGNAL(clicked(bool)), instructionsButton, SLOT(hide()));
    connect(startButton, SIGNAL(clicked(bool)), startButton, SLOT(hide()));
    connect(startButton, SIGNAL(clicked(bool)), quitButton, SLOT(hide()));

    instructionsButton->setGeometry(20, 300, 400, 100);
    instructionsButton->setStyleSheet("QPushButton{border:0px; background: transparent; text-align:left; font:italic 32pt; font-weight: bold; color: #ffffff}QPushButton:hover{border:0px; background: transparent; text-align:left; font:italic 38pt; font-weight: bold; color: #ffffff}");
    connect(instructionsButton, SIGNAL(clicked(bool)), this, SLOT(showInstructions()));

    quitButton->setGeometry(20, 400, 400, 100);
    quitButton->setStyleSheet("QPushButton{border:0px; background: transparent; text-align:left; font:italic 32pt; font-weight: bold; color: #ffffff}QPushButton:hover{border:0px; background: transparent; text-align:left; font:italic 38pt; font-weight: bold; color: #ffffff}");
    connect(quitButton, SIGNAL(clicked(bool)), this, SLOT(closeWindow()));

    backButton->setGeometry(20, 400, 200, 100);
    backButton->setStyleSheet("QPushButton{border:0px; background: transparent; text-align:left; font:italic 28pt; font-weight: bold; color: #ffffff}QPushButton:hover{border:0px; background: transparent; text-align:left; font:italic 32pt; font-weight: bold; color: #ffffff}");
    connect(backButton, SIGNAL(clicked(bool)), this, SLOT(showMenu()));

    instructionsText->setText("Use the ARROWS to move\nUse the SPACEBAR to shoot");
    instructionsText->setGeometry(20, 100, 800, 300);
    instructionsText->setStyleSheet("QLabel{border:0px; background: transparent; text-align:left; font:italic 32pt; font-weight: bold; color: #ffffff}");

    lifeText->setGeometry(20, 10, 200, 100);
    lifeText->setStyleSheet("QLabel{border: 0px; background: transparent; text-align: left; font: 18pt; font-weight: bold; color: #ffffff}");
    connect(this, SIGNAL(showLifes(QString)), lifeText, SLOT(setText(QString)));

    scoreText->setGeometry(20, 5, 200, 30);
    scoreText->setStyleSheet("QLabel{border: 0px; background: transparent; text-align: right; font: 18pt; font-weight: bold; color: #ffffff}");
    connect(this, SIGNAL(showScore(QString)), scoreText, SLOT(setText(QString)));
}
