#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QVector>
#include <QRectF>
#include "model.h"

class GameObject
{
public:
    GameObject(QOpenGLWidget *_glWidget);
    QVector2D speed = QVector2D(0, 0);
    QVector2D force = QVector2D(0, 0);
    QVector4D color = QVector4D(0, 0 , 0, 0);
    QVector3D position = QVector2D(0, 0);
    QVector3D rotation = QVector3D(0, 0, 0);
    QRectF hitbox = QRectF();
    QOpenGLWidget* _glWidget;
    float scale = 0;
    void update();
    void draw();
    Model* model = nullptr;
};

#endif // GAMEOBJECT_H
