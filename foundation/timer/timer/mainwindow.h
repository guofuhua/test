#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer m_timer;
    
private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();
    void slotTimeOut();

    void on_triggerButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
