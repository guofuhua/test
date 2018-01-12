#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QLabel *imageLabel;
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QAction *openAct;
    QAction *newAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    double scaleFactor;
    QScrollArea *scrollArea;

private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();
    void newRect();
};

#endif // MAINWINDOW_H
