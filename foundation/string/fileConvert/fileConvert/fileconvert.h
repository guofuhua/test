#ifndef FILECONVERT_H
#define FILECONVERT_H

#include <QDialog>

namespace Ui {
class FileConvert;
}

class FileConvert : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileConvert(QWidget *parent = 0);
    ~FileConvert();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::FileConvert *ui;
};

#endif // FILECONVERT_H
