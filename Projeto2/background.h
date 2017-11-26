#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "gameobject.h"

class Background : public GameObject
{
public:
    Background(QOpenGLWidget *_glWidget, QVector3D position);
    void update();
};

#endif // BACKGROUND_H
