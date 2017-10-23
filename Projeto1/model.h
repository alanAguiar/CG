#ifndef MODEL_H
#define MODEL_H

#include <QtOpenGL>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QTextStream>
#include <QFile>

#include <fstream>
#include <limits>
#include <iostream>
#include <memory>

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget);
    ~Model();

    void createVBOs();
    void createShaders(QString vertexShaderPath, QString fragmentShaderPath);

    void destroyVBOs();
    void destroyShaders();

    void drawModel();

    void readOFFFile(QString fileName);

    std::unique_ptr<QVector4D []> vertices;
    std::unique_ptr<unsigned int[]> indices;

    QOpenGLWidget *glWidget;

    GLuint vao = 0;

    GLuint vboVertices = 0;
    GLuint vboIndices = 0;

    unsigned int numVertices;
    unsigned int numFaces;

    GLuint shaderProgram = 0;

    QMatrix4x4 modelMatrix;
    QVector3D midPoint;

    QVector2D position;
    float rotation;
    QRectF hitbox;
    double invDiag;

    float scale = 1;
};

#endif // MODEL_H
