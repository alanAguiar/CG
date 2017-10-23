#ifndef BULLET_H
#define BULLET_H

#include "model.h"

class Bullet : public Model
{
public:
    Bullet(QOpenGLWidget *_glWidget, QVector2D position, QVector2D speed, float size);
    Bullet(QOpenGLWidget *_glWidget, QVector2D position, QVector2D speed, float size, QVector2D force);
    void updatePosition();
    void setColor(QVector4D);
    void drawModel();
private:
    QVector2D speed;
    QVector2D force;
    QVector4D color;
    void loadGeometry();
};

#endif // BULLET_H
