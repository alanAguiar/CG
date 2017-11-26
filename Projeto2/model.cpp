#include "model.h"

Model::Model(QOpenGLWidget *_glWidget, QString modelPath, QString fShaderPath, QString vShaderPath, QString* texturePath, int numTextures)
{
    glWidget = _glWidget;
    glWidget->makeCurrent();
    initializeOpenGLFunctions();
    this->modelPath = modelPath;
    this->fShaderPath = fShaderPath;
    this->vShaderPath = vShaderPath;
    for(int i=0; i<numTextures; i++)
        this->loadTexture(texturePath[i]);
    readOBJFile();
}

Model::~Model()
{
    destroyVBOs();
    destroyShaders();
}

void Model::destroyVBOs()
{
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboNormals);
    glDeleteVertexArrays(1, &vboTexCoords);
    glDeleteVertexArrays(1, &vao);

    vboVertices = 0;
    vboIndices = 0;
    vboNormals = 0;
    vboTexCoords = 0;
    vao = 0;
}

void Model::destroyShaders()
{
    glDeleteProgram(shaderProgram);
}

void Model::createVBOs()
{
    glWidget->makeCurrent();
    destroyVBOs();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector4D), vertices.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(QVector3D), normals.get(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numFaces*3*sizeof(unsigned int), indices.get(), GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(QVector2D), textCoords.get(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);
    textCoords.reset();
}

void Model::createShaders()
{
    destroyShaders();
    QString vertexShaderFile(vShaderPath);
    QString fragmentShaderFile(fShaderPath);

    QFile vs(vertexShaderFile);
    QFile fs(fragmentShaderFile);

    vs.open(QFile::ReadOnly | QFile::Text);
    fs.open(QFile::ReadOnly | QFile::Text);

    QTextStream streamVs(&vs), streamFs(&fs);

    QString qtStringVs = streamVs.readAll();
    QString qtStringFs = streamFs.readAll();

    std::string stdStringVs = qtStringVs.toStdString();
    std::string stdStringFs = qtStringFs.toStdString();

    GLuint vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const GLchar *source = stdStringVs.c_str();
    glShaderSource(vertexShader, 1, &source, 0);

    glCompileShader(vertexShader);
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);

        glDeleteShader(vertexShader);
        return;
    }


    GLuint fragmentShader = 0;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    source = stdStringFs.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
        qDebug("%s", &infoLog[0]);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        return;
    }

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if(isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        qDebug("%s", &infoLog[0]);
        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vs.close();
    fs.close();
}

void Model::drawModel()
{
    modelMatrix.setToIdentity();
    modelMatrix.translate(position.x(), position.y(), position.z());
    modelMatrix.scale(invDiag * scale , invDiag * scale , invDiag * scale);

    modelMatrix.rotate(rotation.x(), 1, 0, 0);
    modelMatrix.rotate(rotation.y(), 0, 1, 0);
    modelMatrix.rotate(rotation.z(), 0, 0, 1);

    GLuint locModel = 0;
    GLuint locNormalMatrix = 0;
    GLuint locShininess = 0;

    locModel = glGetUniformLocation(shaderProgram, "model");
    locNormalMatrix = glGetUniformLocation(shaderProgram, "normalMatrix");
    locShininess = glGetUniformLocation(shaderProgram, "shininess");

    glBindVertexArray(vao);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(locModel , 1, GL_FALSE , modelMatrix.data());
    glUniformMatrix3fv(locNormalMatrix, 1, GL_FALSE, modelMatrix.normalMatrix().data());
    glUniform1f(locShininess, static_cast<GLfloat>(material.shininess));

    GLuint loc = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(loc, color.x(), color.y(), color.z(), color.w());

    if(!textures.empty())
    {
        GLuint locColorTexture = 0;
        locColorTexture = glGetUniformLocation(shaderProgram, "colorTexture");
        glUniform1i(locColorTexture, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[textureID]);
    }


    glDrawElements(GL_TRIANGLES , numFaces * 3, GL_UNSIGNED_INT , 0);

}

void Model::readOBJFile(){
    QFile file(modelPath);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    QString line;

    stream >> numVertices >> numFaces;

    indices = std::make_unique<unsigned int[]>(numFaces * 3);

    std::unique_ptr<QVector4D []>  v = std::make_unique<QVector4D []>(numVertices);
    std::vector<QVector2D> vt;
    std::map<std::pair<int, int>, int> v_vt;

    if(numVertices > 0)
    {
        float minLim = std::numeric_limits<float>::lowest();
        float maxLim = std::numeric_limits<float>::max();
        QVector4D max(minLim, minLim, minLim, 1.0);
        QVector4D min(maxLim, maxLim, maxLim, 1.0);
        for(unsigned int i = 0; i < numVertices; ++i)
        {
            float x, y, z;
            stream >> line >> x >> y >> z;
            max.setX(std::max(max.x(), x));
            max.setY(std::max(max.y(), y));
            max.setZ(std::max(max.z(), z));
            min.setX(std::min(min.x(), x));
            min.setY(std::min(min.y(), y));
            min.setZ(std::min(min.z(), z));
            v[i] = QVector4D(x, y, z, 1.0);
        }

        this->midPoint = QVector3D((min + max) * 0.5);
        this->invDiag = 2.0 / (max - min).length();

        this->hitbox = QRectF(min.x()*invDiag, max.y()*invDiag,(max.x()-min.x())*invDiag, (max.y()-min.y())*invDiag);

        stream >> line;
        while(line == "vt")
        {
           float x, y;
            stream >> x >> y;
            vt.push_back(QVector2D(x, 1-y));
            stream >> line;
        }

        int index = 0;
        for (unsigned  int i = 0; i < numFaces; ++i)
        {
            unsigned int va, ta, vb, tb, vc, tc;
            char slash;
            stream >> line  >> va >> slash >> ta
                            >> vb >> slash >> tb
                            >> vc >> slash >> tc;
            va -= 1;
            ta -= 1;
            vb -= 1;
            tb -= 1;
            vc -= 1;
            tc -= 1;

            if(!v_vt.count(std::pair<int, int>(va, ta)))
            {
                v_vt.insert(std::make_pair(std::make_pair(va, ta), index));
                index++;
            }
            if(!v_vt.count(std::pair<int, int>(vb, tb)))
            {
                v_vt.insert(std::make_pair(std::make_pair(vb, tb), index));
                index++;
            }
            if(!v_vt.count(std::pair<int, int>(vc, tc)))
            {
                v_vt.insert(std::make_pair(std::make_pair(vc, tc), index));
                index++;
            }

            indices[i*3 + 0] = v_vt.at(std::make_pair(va, ta));
            indices[i*3 + 1] = v_vt.at(std::make_pair(vb, tb));
            indices[i*3 + 2] = v_vt.at(std::make_pair(vc, tc));
        }

        file.close();

        this->numVertices = v_vt.size();

        vertices = std::make_unique<QVector4D[]>(numVertices);
        textCoords = std::make_unique<QVector2D[]>(numVertices);

        for(std::map<std::pair<int, int>, int>::iterator it=v_vt.begin(); it!=v_vt.end(); it++){
            vertices[it->second]    = v[it->first.first];
            textCoords[it->second]  = vt[it->first.second];
        }

        this->createNormals();
        this->createShaders();
        this->createVBOs();
    }
}

void Model::createNormals()
{
    normals = std::make_unique <QVector3D []>(numVertices);
    for (unsigned int i = 0; i < numFaces; ++i)
    {
        QVector3D a = QVector3D(vertices[indices[i * 3 + 0]]);
        QVector3D b = QVector3D(vertices[indices[i * 3 + 1]]);
        QVector3D c = QVector3D(vertices[indices[i * 3 + 2]]);
        QVector3D faceNormal = QVector3D::crossProduct((b - a), (c - b));
        normals[indices[i * 3 + 0]] += faceNormal;
        normals[indices[i * 3 + 1]] += faceNormal;
        normals[indices[i * 3 + 2]] += faceNormal;
    }
    for (unsigned  int i = 0; i < numVertices; ++i)
    {
        normals[i].normalize();
    }
}

void Model::loadTexture(QString texturePath)
{
    textureID++;
    QImage image;
    image.load(texturePath);
    image = image.convertToFormat(QImage::Format_RGBA8888);
    textures.push_back(0);
    if(textures[textureID])
        glDeleteTextures(1, &textures[textureID]);

    glGenTextures(1 , &textures[textureID]);
    glBindTexture(GL_TEXTURE_2D , textures[textureID]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width() ,image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_WRAP_S ,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_WRAP_T ,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MAG_FILTER ,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D ,GL_TEXTURE_MIN_FILTER ,GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

