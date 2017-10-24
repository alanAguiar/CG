#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{      
    //Set a timer to call the updateObject method in a fixed frequency
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateObjects()));

    //Set the maxScore initial value as zero
    this->maxScore = 0;
}

void OpenGLWidget::startGame()
{
    //Start the calls to the updateObjects method
    timer->start(30);

    //Set the initial score as zero
    this->score = 0;

    //Change the value to indicate that the game has started
    inGame = true;

    //Instantiate the Ship and Boss objects
    ship = std::make_unique<Ship>(this);
    boss = std::make_unique<Boss>(this);

    //Show the text with the remaining lifes on screen
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
    //The objects will only be painted if the game is on progress
    if(inGame)
    {
        //Create an blinking effect after the player gets hit
        if(!invincible || (delay.elapsed()/100) % 2 == 0)
            ship->drawModel();

        boss->drawModel();

        //Paint all the player's bullets
        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end(); it++)
            (*it)->drawModel();

        //Paint all the enemy's bullets
        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end(); it++)
            (*it)->drawModel();
    }
}

void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    //Gets the player movement input
    if (event->key() == Qt::Key_Up)
        ship->upInput = 1;
    if (event->key() == Qt::Key_Down)
        ship->downInput = 1;
    if (event->key() == Qt::Key_Right)
        ship->rightInput = 1;
    if (event->key() == Qt::Key_Left)
        ship->leftInput = 1;

    //Gets the player shooting input
    if (event->key() == Qt::Key_Space)
        shooting = true;    

    //Activates a 'god mode', in which the player can't be hit
    if (event->key() == Qt::Key_G)
    {
        invincible = true;
        delay.start();
    }
}

//Reset the input variables when the keys are released
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
}

void OpenGLWidget::updateObjects()
{
    //Check if the player has lost
    if(ship->lifes == 0)
        endGame(false);
    //Check if the player has won
    else if(boss->lifes == 0)
        endGame(true);

    else
    {
        //Update the player position
        ship->updatePosition();

        //Start the boss shooting behaviour
        boss->shoot();

        //Disables the invincibility if the player after 1500 ms
        if(invincible && delay.elapsed() > 1500)
            invincible = false;

        //Calls the ship shooting method, if the shoot input is given
        if(shooting)
            ship->shoot();

        for(std::list<Bullet *>::iterator it = ship->bulletsShot.begin(); it!=ship->bulletsShot.end();){
            //Check the position of the player bullets. If it is beyond the screen limits, erase it
            if((*it)->position.y() > 1 || (*it)->position.y() < -1)
                it = ship->bulletsShot.erase(it);

            else{
                //Update the bullet position
                (*it)->updatePosition();

                //Check if the bullet collided with the boss
                if((*it)->hitbox.intersects(boss->hitbox)){
                    //Update the boss attributes if hit
                    boss->shotTaken();

                    //Update the score
                    this->score += 100 * 5/boss->lifes;
                    emit showScore(QString("Pontuação: \%1").arg(this->score));

                    //Remove the bullet
                    it = ship->bulletsShot.erase(it);
                }
                else
                    it++;
            }
        }

        for(std::list<Bullet *>::iterator it = boss->bulletsShot.begin(); it!=boss->bulletsShot.end();){
            //Check the position of the boss bullets. If it is beyond the screen limits, erase it
            if((*it)->position.y() > 1 || (*it)->position.y() < -1 || (*it)->position.x() > 1 || (*it)->position.x() < -1)
                it = boss->bulletsShot.erase(it);

            else{
                //Update the bullet position
                (*it)->updatePosition();

                //Check if the bullet collided with the player
                if((*it)->hitbox.intersects(ship->hitbox) && !invincible){

                    //Update the player attributes if hit
                    ship->shotTaken();

                    //Update the remaining lifes text
                    emit showLifes(QString("Vidas: \%1").arg(ship->lifes));

                    //Remove the bullet
                    it = boss->bulletsShot.erase(it);

                    //Start the player invincibility
                    invincible = true;
                    delay.start();
                }
                else
                    it++;
            }
        }
    }

    //Calls the update method, in order to paint the updated screen
    update();
}

//Method called when the game ends
void OpenGLWidget::endGame(bool won)
{
    //Change the inGame value, to indicate that the game is over
    this->inGame = false;

    //Stop the timer, so the updateObjects isn't called
    timer->stop();

    //Check if the score record was beaten
    if(this->score > this->maxScore)
        this->maxScore = this->score;

    //Set the text message
    if(won)
        emit setText(QString("Você venceu! \n\nSua pontuação: \%1 \n\nMaior Pontuação: \%2").arg(this->score).arg(this->maxScore));
    else
        emit setText(QString("Você perdeu! \n\nSua pontuação: \%1 \n\nMaior Pontuação: \%2").arg(this->score).arg(this->maxScore));

    //Enable the text and buttons visibility
    emit enableText();
    emit enableStartButton();
    emit enableQuitButton();
}
