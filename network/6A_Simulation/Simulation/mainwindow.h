#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
    void about();
    void translateChese();
    void translateEnglish();

private:
    void createActions();
    void createMenus();
    void createStatusBar();

    QMenu *fileMenu;
    QMenu *languageMenu;
    QMenu *helpMenu;
    QAction *cheseAct;
    QAction *englishAct;
    QAction *openAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif // MAINWINDOW_H
