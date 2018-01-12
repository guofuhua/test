#ifndef FIREALARMDIALOG_H
#define FIREALARMDIALOG_H

#include <QDialog>
#include <QtGui>
#include "comment.h"

class FireAlarmDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FireAlarmDialog(QWidget *parent = 0);
    void createFireProbesStatus();
    QComboBox *comboBoxProbesStatus[NumFirePropes];
    QComboBox *comboBoxProbesType[NumFirePropes];
    QGroupBox *groupBoxFireProbesStatus;
    QDialogButtonBox *buttonBox;
    
signals:
    
public slots:
    void slotSaveParam();
};

#endif // FIREALARMDIALOG_H
