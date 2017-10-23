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

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    std::shared_ptr<Model> model = nullptr;
    std::shared_ptr<Ship> ship = nullptr;
    std::shared_ptr<Boss> boss = nullptr;
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
private:
    QTime delay;
    bool inGame;
    bool invincible = false;
    bool shooting = false;
    int score, maxScore;
    QTimer* timer;
    void endGame(bool);
signals:
    void showLifes(QString);
    void showScore(QString);
    void setText(QString);
    void enableText();
    void enableStartButton();
    void enableQuitButton();
public slots:    
    void startGame();
    void updateObjects();    
};

#endif // OPENGLWIDGET_H
