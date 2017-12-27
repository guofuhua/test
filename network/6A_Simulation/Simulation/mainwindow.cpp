#include "mainwindow.h"
#include <QtGui>
#include "SimulationDialog.h"

extern void InitUiByLanguage(const QString strLanguage);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();

    createStatusBar();

    setUnifiedTitleAndToolBarOnMac(true);
}


void MainWindow::open()
{
    SimulationDialog dialog;
    qDebug() << dialog.exec();
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                          "write modern GUI applications using Qt, with a menu bar, "
                          "toolbars, and a status bar."));
}

void MainWindow::translateChese()
{
    InitUiByLanguage("chese");
}

void MainWindow::translateEnglish()
{
    InitUiByLanguage("english");
}


void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("&Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

//   "\x4E2D\x6587"
    cheseAct = new QAction(QString::fromUtf8("\xE4\xB8\xAD\xE6\x96\x87"), this);
    cheseAct->setStatusTip(tr("Language choice chese"));
    connect(cheseAct, SIGNAL(triggered()), this, SLOT(translateChese()));

    englishAct = new QAction("English", this);
    englishAct->setStatusTip(tr("Language choice english"));
    connect(englishAct, SIGNAL(triggered()), this, SLOT(translateEnglish()));
}


void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(openAct);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    languageMenu = menuBar()->addMenu(tr("&Language"));
    languageMenu->addAction(cheseAct);
    languageMenu->addSeparator();
    languageMenu->addAction(englishAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
    
}
