#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <list>
#include <time.h>

#include "model.h"
#include "ship.h"
#include "bullet.h"
#include "boss.h"
#include "background.h"

#include "light.h"
#include "camera.h"

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    std::shared_ptr<Model> particlesModel = nullptr;
    std::shared_ptr<Ship> ship = nullptr;
    std::shared_ptr<Boss> boss = nullptr;
    std::shared_ptr<Background> bg1 = nullptr, bg2 = nullptr;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);    
    void paintModel(Model* model);
    Light light;
    Camera camera;

    QPushButton* startButton;
    QPushButton* quitButton;
    QPushButton* instructionsButton;    
    QPushButton* backButton;
    QLabel* instructionsText;
    QLabel* scoreText;
    QLabel* lifeText;
    QLabel* startText;
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
private:
    QTime delay, start;
    bool inGame = false, inMenu = true;
    bool invincible = false;
    bool shooting = false;
    int score, maxScore;
    QTimer* timer;
    void endGame(bool);
    void initializeUI();
signals:
    void showLifes(QString);
    void showScore(QString);
    void setText(QString);
    void setStartText(QString);
public slots:    
    void startGame();
    void updateObjects();    
    void closeWindow();
    void showInstructions();
    void showMenu();
};

#endif // OPENGLWIDGET_H
