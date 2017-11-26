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

#include "material.h"

class Model : public QOpenGLExtraFunctions
{
public:
    Model(QOpenGLWidget *_glWidget, QString modelPath, QString fShaderPath, QString vShaderPath, QString* texturePath, int numTextures);
    ~Model();
    QString modelPath, fShaderPath, vShaderPath;
    void createVBOs();
    void createShaders();

    void destroyVBOs();
    void destroyShaders();

    void drawModel();

    void readOBJFile();

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

    QVector3D position;
    QVector3D rotation;
    QVector4D color;
    QRectF hitbox;
    double invDiag;

    float scale = 1;

    Material material;
    GLuint vboNormals = 0;
    std :: unique_ptr < QVector3D [] > normals;
    int shaderIndex;
    int numShaders;
    void createNormals();

    std::unique_ptr<QVector2D []>textCoords;
    GLuint vboTexCoords = 0;
    std::vector<GLuint> textures;
    int textureID = -1;
    void loadTexture(QString texturePath);
};

#endif // MODEL_H
