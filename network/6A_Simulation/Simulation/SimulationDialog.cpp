#include "SimulationDialog.h"
#include "comment.h"

extern void InitUiByLanguage(const QString strLanguage);
extern STM32Data g_taxData;

SimulationDialog::SimulationDialog(QWidget *parent) :
    QDialog(parent)
{
    createMenu();
    createPublicInfo();
    createAudioTestGroupBox();
    createCameraSettingGroupBox();
    createDeviceStateGroupBox();
    createImagePreviewGroupBox();
    createSwitchTestGroupBox();
    createStatusBar();
    createTimer();
    setWhatsThis(tr("Simulation Software"));

    bigEditor = new QTextEdit;
    bigEditor->setPlainText(tr("This widget takes up all the remaining space "
                               "in the top-level layout."));

    buttonBox = new QDialogButtonBox();

    buttonStart = buttonBox->addButton(tr("Start"), QDialogButtonBox::ActionRole);
    buttonStart->setCheckable(true);
    buttonStop = buttonBox->addButton(tr("Stop"), QDialogButtonBox::ActionRole);
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(slotStart()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(slotStop()));

    QVBoxLayout *statusLayout = new QVBoxLayout;
    statusLayout->addWidget(deviceStateGroupBox);
    statusLayout->addWidget(audioTestGroupBox);
    statusLayout->addWidget(switchTestGroupBox);
    statusLayout->addWidget(cameraSettingGroupBox);
    statusLayout->addWidget(imagePreviewGroupBox);

    QVBoxLayout *publicInfoLayout = new QVBoxLayout;

    publicInfoLayout->addWidget(groupBoxPublicInfo);
    publicInfoLayout->addWidget(bigEditor);
    publicInfoLayout->addWidget(buttonBox);

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setMenuBar(menuBar);
//    Layout->setSpacing(50);
//    statusLayout->setSpacing(25);
//    mainLayout->setSpacing(25);
    mainLayout->addLayout(statusLayout, 0, 0);
    mainLayout->addLayout(publicInfoLayout, 0, 1);


    mainLayout->addWidget(labelCompany, 1, 0);
    mainLayout->addWidget(labelStatus, 1,1);
    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,1);

    setLayout(mainLayout);

    setWindowTitle(tr("Simulate 6A communication V1.0"));

    translateUI();
    prePublicPctl = QPrePublicPctl::getInstance();
    udpEntry = QUdpEntry::getInstance();
    udpReceiveThread = new QUdpRecieveThread();
    connect(prePublicPctl, SIGNAL(signalSendUdpData(QByteArray&)), udpEntry, SLOT(slotSendUdpData(QByteArray&)));
}

void SimulationDialog::about()
{
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates how to "
                          "write modern GUI applications using Qt, with a menu bar, "
                          "toolbars, and a status bar."));
}

void SimulationDialog::changeEvent(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::LanguageChange:
        qDebug() << "--------------";
        translateUI();
        break;
    default:
        QDialog::changeEvent(event);
    }
}

void SimulationDialog::createAudioTestGroupBox()
{
    audioTestGroupBox = new QGroupBox(tr("Audio Test"));

    QGridLayout *layout = new QGridLayout;

    for (int i = 0; i < NumAudioCannels; ++i) {
        buttonRecordSound[i] = new QPushButton(tr("Channel %1 Record").arg(i + 1));
        buttonRecordSound[i]->setFlat(true);
        layout->addWidget(buttonRecordSound[i], 0, i);
    }
    for (int i = 0; i < NumAudioCannels; ++i) {
        buttonPlaySound[i] = new QPushButton(tr("Channel %1 Play").arg(i + 1));
        buttonPlaySound[i]->setFlat(true);
        layout->addWidget(buttonPlaySound[i], 1, i);
    }
    audioTestGroupBox->setLayout(layout);
}

void SimulationDialog::createCameraSettingGroupBox()
{
    cameraSettingGroupBox = new QGroupBox(tr("Camera Settings"));

    QGridLayout *layout = new QGridLayout;

    for (int i = 0; i < NumCameras; ++i) {
        labelCameraSetting[i] = new QLabel(tr("Camera %1:").arg(i + 1));
        layout->addWidget(labelCameraSetting[i], (i / 3), (i % 3));
    }
    cameraSettingGroupBox->setLayout(layout);
}

void SimulationDialog::createDeviceStateGroupBox()
{
    deviceStateGroupBox = new QGroupBox(tr("Device State"));

    QGridLayout *layout = new QGridLayout;

    for (int i = 0; i < NumAVBoards; ++i) {
        labelAVBoardsState[i] = new QLabel(tr("AV%1 Board State").arg(i + 1));
        labelAVBoardsStateValue[i] = new QLabel(tr("NULL"));
        layout->addWidget(labelAVBoardsState[i], i, 0);
        layout->addWidget(labelAVBoardsStateValue[i], i, 1);
    }
    labelSoftwareVersion = new QLabel(tr("Software Version"));
    labelHardwareVersion = new QLabel(tr("Hardware Version"));
    labelSoftwareVersionValue = new QLabel(tr("NULL"));
    labelHardwareVersionValue = new QLabel(tr("NULL"));
    layout->addWidget(labelSoftwareVersion, 0, 2);
    layout->addWidget(labelSoftwareVersionValue, 0, 3);
    layout->addWidget(labelHardwareVersion, 0, 4);
    layout->addWidget(labelHardwareVersionValue, 0, 5);
    for (int i = 0; i < NumCameras; ++i) {
        labelCamerasState[i] = new QLabel(tr("Camera %1 State").arg(i + 1));
        labelCamerasStateValue[i] = new QLabel(tr("NULL"));
        layout->addWidget(labelCamerasState[i], (i / 3 + NumAVBoards), (i % 3 * 2));
        layout->addWidget(labelCamerasStateValue[i], (i / 3 + NumAVBoards), (i % 3 * 2 + 1));
    }
    deviceStateGroupBox->setLayout(layout);
}

void SimulationDialog::createFireResistanceLinkage()
{
    buttonGroupFireProof = new QButtonGroup();
    QGridLayout *layout = new QGridLayout;
    for (int i = 0; i < NumFirePropes; i++) {
        buttonFireAlarmProbe[i] = new QPushButton(tr("prope %1").arg(i));
        buttonFireAlarmProbe[i]->setCheckable(true);
        buttonGroupFireProof->addButton(buttonFireAlarmProbe[i], i);
        layout->addWidget(buttonFireAlarmProbe[i], i / 6, i % 6);
    }
    buttonGroupFireProof->setExclusive(true);
}

void SimulationDialog::createImagePreviewGroupBox()
{
    imagePreviewGroupBox = new QGroupBox(tr("Image Preview"));
    radioFourPicture    = new QRadioButton(tr("Four Picture"));
    radioEightPicture   = new QRadioButton(tr("Eight Picture"));
    QHBoxLayout *hbox   = new QHBoxLayout;
    hbox->addWidget(radioFourPicture);
    hbox->addWidget(radioEightPicture);
    imagePreviewGroupBox->setLayout(hbox);
}

void SimulationDialog::createMenu()
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cheseAct = new QAction(QString::fromUtf8("\xE4\xB8\xAD\xE6\x96\x87"), this);
    cheseAct->setStatusTip(tr("Language choice chese"));
    connect(cheseAct, SIGNAL(triggered()), this, SLOT(translateChese()));

    englishAct = new QAction("English", this);
    englishAct->setStatusTip(tr("Language choice english"));
    connect(englishAct, SIGNAL(triggered()), this, SLOT(translateEnglish()));

    languageMenu = menuBar->addMenu(tr("&Language"));
    languageMenu->addAction(cheseAct);
    languageMenu->addSeparator();
    languageMenu->addAction(englishAct);

    helpMenu = menuBar->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void SimulationDialog::createPublicInfo()
{
    groupBoxPublicInfo = new QGroupBox();

    QGridLayout *layout = new QGridLayout;

    labelDataTypeOfLocomotive           = new QLabel(tr("Type Of Locomotive"));
    labelDataTrainNumber                = new QLabel(tr("Train Number"));
//    labelDataTheSenderID                = new QLabel(tr("The Sender ID"));
    labelDataStationNo                  = new QLabel(tr("Station No"));
    labelDataSpeed                      = new QLabel(tr("Speed"));
    labelDataReconnectionInformation    = new QLabel(tr("Reconnection Information"));
    labelDataNumberOfVehicles           = new QLabel(tr("Number Of Vehicles"));
    labelDataLocomotiveWorkingCondition = new QLabel(tr("Locomotive Working Condition"));
    labelDataChauffeurOccupancy         = new QLabel(tr("Chauffeur Occupancy"));
    labelDataDeviceStatus               = new QLabel(tr("Device Status"));
    labelDataDriverNumber               = new QLabel(tr("Driver Number"));
    labelDataIntersectionNumber         = new QLabel(tr("Intersection Number"));
    labelDataKilometerPost              = new QLabel(tr("Kilomete rPost"));
    labelDataLengthCounting             = new QLabel(tr("Length Counting"));
    labelDataLocomotiveNumber           = new QLabel(tr("Locomotive Number"));

    lineEditDataStationNo           = new QLineEdit("5");
    lineEditDataSpeed               = new QLineEdit("0");
    lineEditDataNumberOfVehicles    = new QLineEdit("16");
    lineEditDataDriverNumber        = new QLineEdit("4220342");
    lineEditDataIntersectionNumber  = new QLineEdit("33");
    lineEditDataKilometerPost       = new QLineEdit("108");
    lineEditDataLengthCounting      = new QLineEdit("42");
    lineEditDataLocomotiveNumber    = new QLineEdit("33");
    lineEditDataTrainNumber1        = new QLineEdit("K");
    lineEditDataTrainNumber2        = new QLineEdit("800");

    comboBoxDataTypeOfLocomotive            = new QComboBox(); //机车类型
//    comboBoxDataTheSenderID                 = new QComboBox();
    comboBoxDataReconnectionInformation     = new QComboBox();
    comboBoxDataLocomotiveWorkingCondition  = new QComboBox();
    comboBoxDataChauffeurOccupancy          = new QComboBox();
    comboBoxDataDeviceStatus                = new QComboBox();
    comboBoxDataShunting                    = new QComboBox();

    QStringList comboBoxItem;
    comboBoxItem << tr("HXD2B") << tr("TA") << tr("DF9") << tr("DF7D") << tr("SS7D") << tr("SS3") << tr("NY7") << tr("GK1A") << tr("XSG") << tr("DF3");
    comboBoxDataTypeOfLocomotive->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << "1" << "2";
//    comboBoxDataTheSenderID->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << tr("unknown") << tr("reconnexion") << tr("non-reconnexion");
    comboBoxDataReconnectionInformation->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << tr("nonzero") << tr("zero") << tr("backward") << tr("forward") << tr("drag") << tr("braking");
    comboBoxDataLocomotiveWorkingCondition->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << tr("unknown") << tr("one occupation") << tr("two occupation") << tr("all occupation");
    comboBoxDataChauffeurOccupancy->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << tr("monitor") << tr("degrade");
    comboBoxDataDeviceStatus->addItems(comboBoxItem);
    comboBoxItem.clear();
    comboBoxItem << tr("non shunting") << tr("shunting");
    comboBoxDataShunting->addItems(comboBoxItem);

    radioDataBen    = new QRadioButton(tr("Leading"));
    radioDataBu     = new QRadioButton(tr("Assisting"));
    radioDataHuo    = new QRadioButton(tr("Freight"));
    radioDataKe     = new QRadioButton(tr("Passenger"));
    groupBoxBenBu   = new QGroupBox();
    groupBoxKeHuo   = new QGroupBox();
    radioDataBen->setChecked(true);
    radioDataHuo->setChecked(true);
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(radioDataBen);
    hbox->addWidget(radioDataBu);
    groupBoxBenBu->setLayout(hbox);
    hbox = new QHBoxLayout;
    hbox->addWidget(radioDataHuo);
    hbox->addWidget(radioDataKe);
    groupBoxKeHuo->setLayout(hbox);

    layout->addWidget(labelDataKilometerPost, 0, 0);
    layout->addWidget(lineEditDataKilometerPost, 0, 1);
    layout->addWidget(labelDataSpeed, 0, 2);
    layout->addWidget(lineEditDataSpeed, 0, 3);
    layout->addWidget(groupBoxBenBu, 0, 4, 2, 2);
    layout->addWidget(labelDataLengthCounting, 1, 0);
    layout->addWidget(lineEditDataLengthCounting, 1, 1);
    layout->addWidget(labelDataNumberOfVehicles, 1, 2);
    layout->addWidget(lineEditDataNumberOfVehicles, 1, 3);
    layout->addWidget(labelDataStationNo, 2, 0);
    layout->addWidget(lineEditDataStationNo, 2, 1);
    layout->addWidget(labelDataDriverNumber, 2, 2);
    layout->addWidget(lineEditDataDriverNumber, 2, 3);
    layout->addWidget(groupBoxKeHuo, 2, 4, 2, 2);
    layout->addWidget(labelDataChauffeurOccupancy, 3, 0);
    layout->addWidget(comboBoxDataChauffeurOccupancy, 3, 1);
    layout->addWidget(labelDataReconnectionInformation, 3, 2);
    layout->addWidget(comboBoxDataReconnectionInformation, 3, 3);
    layout->addWidget(labelDataDeviceStatus, 4, 0);
    layout->addWidget(comboBoxDataDeviceStatus, 4, 1);
    layout->addWidget(comboBoxDataShunting, 4, 2);
    layout->addWidget(labelDataTrainNumber, 5, 0);
    layout->addWidget(lineEditDataTrainNumber1, 5, 1);
    layout->addWidget(lineEditDataTrainNumber2, 5, 2);
//    layout->addWidget(labelDataTheSenderID, 5, 3);
//    layout->addWidget(comboBoxDataTheSenderID, 5, 4);
    layout->addWidget(labelDataIntersectionNumber, 6, 0);
    layout->addWidget(lineEditDataIntersectionNumber, 6, 1);
    layout->addWidget(labelDataLocomotiveWorkingCondition, 6, 2);
    layout->addWidget(comboBoxDataLocomotiveWorkingCondition, 6, 3);
    layout->addWidget(labelDataTypeOfLocomotive, 7, 0);
    layout->addWidget(comboBoxDataTypeOfLocomotive, 7, 1);
    layout->addWidget(labelDataLocomotiveNumber, 8, 0);
    layout->addWidget(lineEditDataLocomotiveNumber, 8, 1);

    groupBoxPublicInfo->setLayout(layout);
}

void SimulationDialog::createStatusBar()
{
    labelCompany = new QLabel(tr("thinkfreely"));
    labelStatus = new QLabel(tr("Ready!"));
}

void SimulationDialog::createSwitchTestGroupBox()
{
    switchTestGroupBox = new QGroupBox(tr("Switch Test"));
    QHBoxLayout *layout = new QHBoxLayout;

    buttonSwitchInTest = new QPushButton(tr("Switch In Test"));
    buttonSwitchOutTest = new QPushButton(tr("Switch Out Test"));
    buttonSwitchInTest->setFlat(true);
    buttonSwitchOutTest->setFlat(true);
    layout->addWidget(buttonSwitchOutTest);
    layout->addWidget(buttonSwitchInTest);

    switchTestGroupBox->setLayout(layout);
}

void SimulationDialog::createTimer()
{
    timerSendTime = new QTimer();
    timerSendTime->setInterval(1000);
    connect(timerSendTime, SIGNAL(timeout()), this, SLOT(slotSendTime()));
}

void SimulationDialog::setPublicInfo()
{
    g_taxData.m_kmMark = lineEditDataKilometerPost->text().toInt();
    g_taxData.m_realSpeed = lineEditDataSpeed->text().toInt();
    g_taxData.m_strTrainCode = lineEditDataTrainNumber1->text() + lineEditDataTrainNumber2->text();
    g_taxData.m_trainNum = lineEditDataLocomotiveNumber->text().toInt();
    switch (comboBoxDataLocomotiveWorkingCondition->currentIndex()) {
    case 0:
        g_taxData.m_handlePosition = 0;
        break;
    case 1:
        g_taxData.m_handlePosition = 1;
        break;
    case 2:
        g_taxData.m_cheWei = STM32Data::DBack;
        break;
    case 3:
        g_taxData.m_cheWei = STM32Data::DForword;
        break;
    case 4:
        g_taxData.m_bDrag = true;
        break;
    case 5:
        g_taxData.m_bBreak = true;
        break;
    default:
        qDebug() << "setPublicInfo unknown case :" << comboBoxDataLocomotiveWorkingCondition->currentText();
        break;
    }
    g_taxData.m_iRoomUsed = comboBoxDataChauffeurOccupancy->currentIndex();
    g_taxData.m_bDownLevel = comboBoxDataDeviceStatus->currentIndex();
    g_taxData.m_bShuntingFlag = comboBoxDataShunting->currentIndex();
    g_taxData.m_bReConnect = comboBoxDataReconnectionInformation->currentIndex();
    if (radioDataBen->isChecked()) {
        g_taxData.m_trainType = STM32Data::BENWU;
    } else {
        g_taxData.m_trainType = STM32Data::BUJI;
    }
    if (radioDataHuo->isChecked()) {
        g_taxData.m_bPassengerTrain = false;
    } else {
        g_taxData.m_bPassengerTrain = true;
    }
    g_taxData.m_iCountLong = lineEditDataLengthCounting->text().toInt();
    g_taxData.m_iCountNumer = lineEditDataNumberOfVehicles->text().toInt();
    g_taxData.m_stationNum = lineEditDataStationNo->text().toInt();
    g_taxData.m_driverNum = lineEditDataDriverNumber->text().toInt();
    g_taxData.m_segmentNum = lineEditDataIntersectionNumber->text().toInt();
    switch (comboBoxDataTypeOfLocomotive->currentIndex()) {
    case 0:
        g_taxData.m_trainModel = 235;
        break;
    case 1:
        g_taxData.m_trainModel = 139;
        break;
    case 2:
        g_taxData.m_trainModel = 112;
        break;
    case 3:
        g_taxData.m_trainModel = 136;
        break;
    case 4:
        g_taxData.m_trainModel = 217;
        break;
    case 5:
        g_taxData.m_trainModel = 206;
        break;
    case 6:
        g_taxData.m_trainModel = 126;
        break;
    case 7:
        g_taxData.m_trainModel = 137;
        break;
    case 8:
        g_taxData.m_trainModel = 149;
        break;
    case 9:
        g_taxData.m_trainModel = 103;
        break;
    default:
        qDebug() << "setPublicInfo unknown Type Of Locomotive case :" << comboBoxDataTypeOfLocomotive->currentText();
        break;
    }
}

void SimulationDialog::slotSendTime()
{
    setPublicInfo();
    prePublicPctl->pdtTimePlt();
}

void SimulationDialog::slotStart()
{
    timerSendTime->start();
}

void SimulationDialog::slotStop()
{
    if (buttonStart->isChecked()) {
        buttonStart->setChecked(false);
    }
    timerSendTime->stop();
}

void SimulationDialog::translateChese()
{
    InitUiByLanguage("chese");
}

void SimulationDialog::translateEnglish()
{
    InitUiByLanguage("english");
}

void SimulationDialog::translateUI()
{
    buttonStart->setText(tr("Start"));
    buttonStop->setText(tr("Stop"));
    qDebug() << "set start";
}


