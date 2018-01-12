#include "FireAlarmDialog.h"

extern TFireproofMonitoringMessage g_FireproofMonitorInfo;

FireAlarmDialog::FireAlarmDialog(QWidget *parent) :
    QDialog(parent)
{
    createFireProbesStatus();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotSaveParam()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(groupBoxFireProbesStatus);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void FireAlarmDialog::createFireProbesStatus()
{
    groupBoxFireProbesStatus = new QGroupBox(tr("Fire Monitoring State"));
    QGridLayout *layout = new QGridLayout;
    QStringList comboBoxItem, probeStatus;
    probeStatus << tr("offline");
    probeStatus << tr("normal");
    probeStatus << tr("fault");
    probeStatus << tr("pollution");
    probeStatus << tr("alarm");
    probeStatus << tr("separate");
    for (int i = 0; i < NumFirePropes; i++) {
        comboBoxItem.clear();
        comboBoxItem << tr("probe %1 not install").arg(i+1);
        comboBoxItem << tr("probe %1 compound").arg(i+1);
        comboBoxItem << tr("probe %1 high temperature ").arg(i+1);
        comboBoxItem << tr("probe %1 fire").arg(i+1);
        comboBoxItem << tr("probe %1 temperature sensor").arg(i+1);
        comboBoxItem << tr("probe %1 smoke").arg(i+1);
        comboBoxProbesType[i] = new QComboBox();
        comboBoxProbesType[i]->addItems(comboBoxItem);
        layout->addWidget(comboBoxProbesType[i], i % (NumFirePropes / 2), (2 * i / NumFirePropes * 2));

        comboBoxProbesStatus[i] = new QComboBox();
        comboBoxProbesStatus[i]->addItems(probeStatus);
        layout->addWidget(comboBoxProbesStatus[i], i % (NumFirePropes / 2), (2 * i / NumFirePropes * 2 + 1));
    }

    groupBoxFireProbesStatus->setLayout(layout);

    for (int i = 0; i < NumFirePropes; i++) {
        comboBoxProbesStatus[i]->setCurrentIndex((g_FireproofMonitorInfo.probeStatus[i] >> 3) & 0x07);
        comboBoxProbesType[i]->setCurrentIndex(g_FireproofMonitorInfo.probeStatus[i] & 0x07);
    }
}

void FireAlarmDialog::slotSaveParam()
{
//    memset(&g_FireproofMonitorInfo, 0, sizeof(g_FireproofMonitorInfo));
//    QByteArray tempArray1(g_FireproofMonitorInfo.probeStatus, 32);
//    qDebug() << "slotSaveParam1" << QString(tempArray1.toHex());
    g_FireproofMonitorInfo.sync = 0xAAAA;
    g_FireproofMonitorInfo.length = 0x28;
    g_FireproofMonitorInfo.type = 0x22;
    g_FireproofMonitorInfo.busState = 0x01;
    for (int i = 0; i < NumFirePropes; i++) {
        g_FireproofMonitorInfo.probeStatus[i] = 0x38 & (comboBoxProbesStatus[i]->currentIndex() << 3);
        g_FireproofMonitorInfo.probeStatus[i] |= 0x07 & comboBoxProbesType[i]->currentIndex();
        if (0 != comboBoxProbesStatus[i]->currentIndex()) {
            g_FireproofMonitorInfo.busState &= ~0x01;
        }
    }
//    QByteArray tempArray(g_FireproofMonitorInfo.probeStatus, 32);
//    qDebug() << "slotSaveParam2" << QString(tempArray.toHex());
//    qDebug() << comboBoxProbesStatus[0]->currentIndex() << comboBoxProbesType[0]->currentIndex();
    this->accept();
}
