/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "openglwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    OpenGLWidget *openGLWidget;
    QLabel *lifeLabel;
    QLabel *scoreLabel;
    QLabel *textLabel;
    QPushButton *startButton;
    QPushButton *quitButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(646, 720);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new OpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 40, 628, 611));
        openGLWidget->setFocusPolicy(Qt::StrongFocus);
        lifeLabel = new QLabel(centralWidget);
        lifeLabel->setObjectName(QStringLiteral("lifeLabel"));
        lifeLabel->setGeometry(QRect(10, 10, 67, 17));
        scoreLabel = new QLabel(centralWidget);
        scoreLabel->setObjectName(QStringLiteral("scoreLabel"));
        scoreLabel->setGeometry(QRect(490, 10, 141, 17));
        textLabel = new QLabel(centralWidget);
        textLabel->setObjectName(QStringLiteral("textLabel"));
        textLabel->setGeometry(QRect(210, 190, 201, 161));
        textLabel->setAutoFillBackground(true);
        textLabel->setScaledContents(false);
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setWordWrap(true);
        textLabel->setIndent(-1);
        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setGeometry(QRect(320, 360, 89, 25));
        quitButton = new QPushButton(centralWidget);
        quitButton->setObjectName(QStringLiteral("quitButton"));
        quitButton->setGeometry(QRect(210, 360, 89, 25));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 646, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(openGLWidget, SIGNAL(showLifes(QString)), lifeLabel, SLOT(setText(QString)));
        QObject::connect(openGLWidget, SIGNAL(showScore(QString)), scoreLabel, SLOT(setText(QString)));
        QObject::connect(quitButton, SIGNAL(clicked()), MainWindow, SLOT(close()));
        QObject::connect(startButton, SIGNAL(clicked()), openGLWidget, SLOT(startGame()));
        QObject::connect(startButton, SIGNAL(clicked()), textLabel, SLOT(hide()));
        QObject::connect(startButton, SIGNAL(clicked()), startButton, SLOT(hide()));
        QObject::connect(startButton, SIGNAL(clicked()), quitButton, SLOT(hide()));
        QObject::connect(openGLWidget, SIGNAL(enableText()), textLabel, SLOT(show()));
        QObject::connect(openGLWidget, SIGNAL(enableStartButton()), startButton, SLOT(show()));
        QObject::connect(openGLWidget, SIGNAL(enableQuitButton()), quitButton, SLOT(show()));
        QObject::connect(openGLWidget, SIGNAL(setText(QString)), textLabel, SLOT(setText(QString)));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        lifeLabel->setText(QApplication::translate("MainWindow", "Vidas:", Q_NULLPTR));
        scoreLabel->setText(QApplication::translate("MainWindow", "Pontua\303\247\303\243o: 0", Q_NULLPTR));
        textLabel->setText(QApplication::translate("MainWindow", "Instru\303\247\303\265es: \n"
"\n"
" Use as SETAS \n"
" para se mover \n"
"\n"
" Use ESPA\303\207O para atirar", Q_NULLPTR));
        startButton->setText(QApplication::translate("MainWindow", "Jogar", Q_NULLPTR));
        quitButton->setText(QApplication::translate("MainWindow", "Sair", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
