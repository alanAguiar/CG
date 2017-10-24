#include "bullet.h"

Bullet::Bullet(QOpenGLWidget *_glWidget, QVector2D position, QVector2D speed, float size) : Model(_glWidget)
{
    //Set the attibutes with default values
    this->position = position;
    this->speed = speed;    
    this->force = QVector2D(0, 0);
    this->scale = size;
    this->rotation = 0;
    this->color = QVector4D(1, 1, 1, 1);

    //Load the model
    this->loadGeometry();
    this->createVBOs();
    this->createShaders(":/shaders/bulletVShader.glsl", ":/shaders/fshader1.glsl");
}

//Overloaded constructor, with the addition of an 'force' parameter
Bullet::Bullet(QOpenGLWidget *_glWidget, QVector2D position, QVector2D speed, float size, QVector2D force) :  Bullet(_glWidget, position, speed, size)
{
    this->force = force;
}


void Bullet::updatePosition()
{
    //Update the model position, according to the speed
    position.setX(position.x() + speed.x());
    position.setY(position.y() + speed.y());
    //Accelerate the object
    speed += force;
    //Update the hitbox position accordingly
    this->hitbox.moveCenter(QPointF(position.x(), position.y()));

}

//Create a square geometry
void Bullet::loadGeometry()
{
    this->numVertices = 4;
    this->numFaces = 2;

    this->vertices = std::make_unique<QVector4D[]>(numVertices);
    this->indices = std::make_unique<unsigned int[]>(numFaces * 3);

    this->vertices [0] = QVector4D  (-0.5 ,   -0.5   ,0   ,1);
    this->vertices [1] = QVector4D ( 0.5 ,   -0.5   ,0   ,1);
    this->vertices [2] = QVector4D ( 0.5 ,    0.5   ,0   ,1);
    this->vertices [3] = QVector4D  (-0.5 ,    0.5   ,0   ,1);

    this->indices [0] = 0;
    this->indices [1] = 1;
    this->indices [2] = 2;
    this->indices [3] = 2;
    this->indices [4] = 3;
    this->indices [5] = 0;

    this->midPoint = QVector3D(0, 0, 0);
    this->invDiag = 1;

    this->hitbox = QRectF(-this->scale/2, this->scale/2, this->scale, this->scale);
}

//Set object's color
void Bullet::setColor(QVector4D c)
{
    this->color = c;
}

//Overwrite the drawModel method, in order to add 'color' as a IN attribute on shader
void Bullet::drawModel()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position.x(), position.y(), 0);
    modelMatrix.scale(invDiag * scale , invDiag * scale , invDiag * scale);
    modelMatrix.rotate(rotation, 0, 0, 1);

    glBindVertexArray(vao);
    glUseProgram(shaderProgram);

    GLuint locModelMatrix = glGetUniformLocation(shaderProgram , "model");
    glUniformMatrix4fv(locModelMatrix , 1, GL_FALSE , modelMatrix.data());

    GLuint loc = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(loc, color.x(), color.y(), color.z(), color.w());

    glDrawElements(GL_TRIANGLES , numFaces * 3, GL_UNSIGNED_INT,0);
}
