#include "SimulationDialog.h"
#include "comment.h"
#include "FireAlarmDialog.h"
//#include "QVideoMonitor.h"

extern void InitUiByLanguage(const QString strLanguage);
extern STM32Data g_taxData;
extern TVersionInfo g_tVersionInfo;
extern TImageInfo g_tImageInfo;//视频图片报文
TFireInfo g_FireInfo;
TFireproofMonitoringMessage g_FireproofMonitorInfo;
QString bigEditDisplay;
TEthernetCharacterVersionMessage g_EthernetCharacterVersionMessage;

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
    createFireResistanceLinkage();
    createStatusBar();
    createTimer();
    setWhatsThis(tr("Simulation Software"));

    bigEditor = new QTextEdit;
    bigEditor->setPlainText(tr("clicked start send public info, clicked simulation send fire info"));

    buttonBox = new QDialogButtonBox();

    buttonClear = buttonBox->addButton(tr("Clear"), QDialogButtonBox::ActionRole);
    buttonStart = buttonBox->addButton(tr("Start"), QDialogButtonBox::ActionRole);
    buttonStart->setCheckable(true);
    buttonStop = buttonBox->addButton(tr("Stop"), QDialogButtonBox::ActionRole);
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(slotStart()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(slotStop()));
    connect(buttonClear, SIGNAL(clicked()), this, SLOT(slotClear()));

    QVBoxLayout *statusLayout = new QVBoxLayout;
    statusLayout->addWidget(deviceStateGroupBox);
    statusLayout->addWidget(audioTestGroupBox);
    statusLayout->addWidget(switchTestGroupBox);
    statusLayout->addWidget(cameraSettingGroupBox);
    statusLayout->addWidget(imagePreviewGroupBox);

    QVBoxLayout *publicInfoLayout = new QVBoxLayout;

    publicInfoLayout->addWidget(groupBoxPublicInfo);
    publicInfoLayout->addWidget(groupBoxFireProof);
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

    prePublicPctl = QPrePublicPctl::getInstance();
    udpReceiveThread = new QUdpRecieveThread();
    QSettings _settings("./user.ini", QSettings::IniFormat);
    udpReceiveThread->m_strIPAddr = _settings.value("AV3/IP", "192.168.1.58").toString();
    udpReceiveThread->m_uIPPort = _settings.value("AV3/port", 7000).toInt();
    connect(udpReceiveThread, SIGNAL(signalSendData(QString)), bigEditor, SLOT(setText(QString)));
    connect(prePublicPctl, SIGNAL(signalSendUdpData(QByteArray&)), udpReceiveThread, SLOT(slotSendUdpData(QByteArray&)));
    connect(udpReceiveThread, SIGNAL(signalReceiveType(int)), this, SLOT(slotReceiveStatus(int)));
//    udpReceiveThread->start();
    isFireAlarmDataValid = false;
    if (_settings.contains("Simulation/FireAlarm")) {
        isFireAlarmDataValid = true;
        g_FireproofMonitorInfo.sync = 0xAAAA;
        g_FireproofMonitorInfo.length = 0x28;
        g_FireproofMonitorInfo.type = 0x22;
        g_FireproofMonitorInfo.busState = 0x01;
    }
    QByteArray tempArray;
    tempArray = QByteArray::fromHex(_settings.value("Simulation/FireLink").toByteArray());
    if (NumFirePropes * 4 == tempArray.size()) {
        memcpy(g_FireInfo.fireProbe, tempArray.data(), tempArray.size());
    }
    tempArray = QByteArray::fromHex(_settings.value("Simulation/FireAlarm").toByteArray());
    if (33 == tempArray.size()) {
        memcpy(&g_FireproofMonitorInfo.busState, tempArray.data(), tempArray.size());
    }
    if (!_settings.contains("Simulation/SendPublic")) {
        _settings.setValue("Simulation/SendPublic", false);
    }
    if (!_settings.contains("Simulation/SendTime")) {
        _settings.setValue("Simulation/SendTime", false);
    }
    if (!_settings.contains("Simulation/SendFire")) {
        _settings.setValue("Simulation/SendFire", true);
    }
    isSendPublicInfo = _settings.value("Simulation/SendPublic", false).toBool();
    isSendTime = _settings.value("Simulation/SendTime", false).toBool();
    isSendFire = _settings.value("Simulation/SendFire", true).toBool();
    translateUI();
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
    audioTestGroupBox->setEnabled(false);
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
    cameraSettingGroupBox->setEnabled(false);
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
    memset(&g_FireInfo, 0, sizeof(g_FireInfo));
    g_FireInfo.sync = 0xAAAA;
    g_FireInfo.length = 0x89;
    g_FireInfo.type = 0x55;
    groupBoxFireProof = new QGroupBox(tr("Fire Resistance Linkage"));
    buttonGroupFireProof = new QButtonGroup();
    QGridLayout *layout = new QGridLayout;
    for (int i = 0; i < NumFirePropes; i++) {
        buttonFireAlarmProbe[i] = new QPushButton(tr("prope %1").arg(i + 1));
        buttonFireAlarmProbe[i]->setCheckable(true);
        buttonGroupFireProof->addButton(buttonFireAlarmProbe[i], i);
        layout->addWidget(buttonFireAlarmProbe[i], i / 6, i % 6);
    }
    buttonFireAlarmProbe[0]->setChecked(true);
    buttonGroupFireProof->setExclusive(true);
    connect(buttonGroupFireProof, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonGroupFireProof(int)));
//    layout->setSpacing(20);
    buttonGroupFireCamera = new QButtonGroup();
    for (int i = 0; i < NumCameras; i++) {
        checkBoxCameras[i] = new QCheckBox(tr("Camera %1").arg(i + 1));
        buttonGroupFireCamera->addButton(checkBoxCameras[i], i);
        layout->addWidget(checkBoxCameras[i], i / 6 + 6, i % 6);
    }
    buttonGroupFireCamera->setExclusive(false);
    buttonFireLink = new QPushButton(tr("Fire Link"));
    buttonFireSimulation = new QPushButton(tr("Simulation"));
    buttonFireResetParam = new QPushButton(tr("Reset Param"));
    layout->addWidget(buttonFireLink, 8, 3);
    layout->addWidget(buttonFireSimulation, 8, 4);
    layout->addWidget(buttonFireResetParam, 8, 5);
    connect(buttonGroupFireCamera, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonGroupFireCamera(int)));
    connect(buttonFireLink, SIGNAL(clicked()), this, SLOT(slotButtonFireLink()));
    connect(buttonFireSimulation, SIGNAL(clicked()), this, SLOT(slotButtonFireSimulation()));
    connect(buttonFireResetParam, SIGNAL(clicked()), this, SLOT(slotButtonFireResetParam()));
    groupBoxFireProof->setLayout(layout);
}

void SimulationDialog::createImagePreviewGroupBox()
{
    imagePreviewGroupBox = new QGroupBox(tr("Image Preview"));
    radioFourPicture    = new QRadioButton(tr("Four Picture"));
    radioEightPicture   = new QRadioButton(tr("Eight Picture"));
    buttonImageView     = new QPushButton(tr("Image Previews"));
    QHBoxLayout *hbox   = new QHBoxLayout;
    hbox->addWidget(radioFourPicture);
    hbox->addWidget(radioEightPicture);
    hbox->addWidget(buttonImageView);
    connect(buttonImageView, SIGNAL(clicked()), this, SLOT(slotImagePreview()));
    imagePreviewGroupBox->setLayout(hbox);
//    imagePreviewGroupBox->setEnabled(false);
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
    switchTestGroupBox->setEnabled(false);
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

void SimulationDialog::slotButtonFireLink()
{
    prePublicPctl->pdtFireInfo();
}

void SimulationDialog::slotButtonFireResetParam()
{
    for (int i = 0; i < NumCameras; i++) {
        checkBoxCameras[i]->setCheckState(Qt::Unchecked);
    }

    memset(g_FireInfo.fireProbe, 0, NumFirePropes * 4);

    QSettings _settings("./user.ini", QSettings::IniFormat);
    _settings.remove("Simulation/FireLink");
//    QByteArray tempArray(g_FireInfo.fireProbe, NumFirePropes * 4);
//    _settings.setValue("Simulation/FireLink", tempArray.toHex());
}

void SimulationDialog::slotButtonFireSimulation()
{
    FireAlarmDialog dialog;
//    qDebug() << dialog.exec();
    if (QDialog::Accepted == dialog.exec()) {
        qDebug() << "fire alarm changed";
        isFireAlarmDataValid = true;
        QSettings _settings("./user.ini", QSettings::IniFormat);
        QByteArray tempArray(g_FireproofMonitorInfo.probeStatus, 32);
        tempArray.insert(0, g_FireproofMonitorInfo.busState);
        _settings.setValue("Simulation/FireAlarm", tempArray.toHex());
    }
//    prePublicPctl->pdtFireAlarmInfo();
//    QString display = tr("send data:");
//    display.append(prePublicPctl->pdtFireInfo());
//    bigEditor->setText(display);
}

void SimulationDialog::slotButtonGroupFireCamera(int id)
{
    qDebug() << "slotButtonGroupFireCamera" << id << buttonGroupFireCamera->button(id)->isChecked();
    int currentProbe = buttonGroupFireProof->checkedId();
    if (currentProbe >=0 ) {
        if (checkBoxCameras[id]->checkState() == Qt::Checked) {
            int cameraId = g_FireInfo.fireProbe[currentProbe].camera[3];
            if (cameraId) {
                checkBoxCameras[cameraId - 1]->setCheckState(Qt::Unchecked);
            }
            g_FireInfo.fireProbe[currentProbe].camera[3] = g_FireInfo.fireProbe[currentProbe].camera[2];
            g_FireInfo.fireProbe[currentProbe].camera[2] = g_FireInfo.fireProbe[currentProbe].camera[1];
            g_FireInfo.fireProbe[currentProbe].camera[1] = g_FireInfo.fireProbe[currentProbe].camera[0];
            g_FireInfo.fireProbe[currentProbe].camera[0] = id + 1;
        } else {
            for (int i = 0; i < 4; i++) {
                if (id == g_FireInfo.fireProbe[currentProbe].camera[i] - 1) {
                    g_FireInfo.fireProbe[currentProbe].camera[i] = 0;
                    break;
                }
            }
            for (int i = 0; i < 3; i++) {
                int cameraId = g_FireInfo.fireProbe[currentProbe].camera[i];
                if (!cameraId) {
                    g_FireInfo.fireProbe[currentProbe].camera[i] = g_FireInfo.fireProbe[currentProbe].camera[i + 1];
                    g_FireInfo.fireProbe[currentProbe].camera[i + 1] = 0;
                }
            }
        }
    }

    QSettings _settings("./user.ini", QSettings::IniFormat);
    QByteArray tempArray((char *)g_FireInfo.fireProbe, NumFirePropes * 4);
    _settings.setValue("Simulation/FireLink", tempArray.toHex());
}

void SimulationDialog::slotButtonGroupFireProof(int id)
{
    if (id < 0)
        return;

    for (int i = 0; i < NumCameras; i++) {
        checkBoxCameras[i]->setCheckState(Qt::Unchecked);
    }
    for (int i = 0; i < 4; i++) {
        int cameraId = g_FireInfo.fireProbe[id].camera[i];
        if (cameraId) {
            checkBoxCameras[cameraId - 1]->setCheckState(Qt::Checked);
        }
    }
}

void SimulationDialog::slotClear()
{
    bigEditDisplay.clear();
    bigEditor->clear();
}

void SimulationDialog::slotImagePreview()
{
//    QVideoMonitor dialog;
//    qDebug() << dialog.exec();
//    if (QDialog::Accepted == dialog.exec()) {
//        qDebug() << "fire alarm changed";
//    }
}

void SimulationDialog::slotReceiveStatus(int type)
{
    qDebug() << "slotReceiveStatus" << type;
    switch (type)
    {
    case 1:
        labelSoftwareVersionValue->setText(QString("%1").arg((int)g_tVersionInfo.m_uSoftwaveVersion));
        labelHardwareVersionValue->setText(QString("%1").arg((int)g_tVersionInfo.m_uHardwaveVersion));
        break;
    case 5:
        if (g_tImageInfo.m_bCard1Fault) {
            labelAVBoardsStateValue[0]->setText(tr("fault"));
        } else {
            labelAVBoardsStateValue[0]->setText(tr("normal"));
        }
        if (g_tImageInfo.m_bCard2Fault) {
            labelAVBoardsStateValue[1]->setText(tr("fault"));
        } else {
            labelAVBoardsStateValue[1]->setText(tr("normal"));
        }
        for (int i = 0; i < NumCameras; i++) {
            if (g_tImageInfo.m_uVideoCheck & (0x1 << i)) {
                labelCamerasStateValue[i]->setText(tr("fault"));
            } else {
                labelCamerasStateValue[i]->setText(tr("normal"));
            }
        }
        break;
    }
}

void SimulationDialog::slotSendTime()
{
    if (isSendPublicInfo) {
        setPublicInfo();
    }
    bool isAlarm = false;

    if (0 == sendTimes++ % 10) {
        if (0 == sendTimes % 60) {
            if (isSendTime) {
                prePublicPctl->pdtTimePlt();
            }
        }
//        prePublicPctl->pdtEthernetCharacterVersionMessage();
        if (isFireAlarmDataValid && isSendFire) {
            prePublicPctl->pdtFireAlarmInfo();
        }
    } else if (isFireAlarmDataValid && isSendFire) {
        for (int i = 0; i < NumFirePropes; i++) {
            if (0x20 == (g_FireproofMonitorInfo.probeStatus[i] & 0x38)) {
                isAlarm = true;
                break;
            }
        }
        if (isAlarm) {
            prePublicPctl->pdtFireAlarmInfo();
        }
    }
    if (isSendPublicInfo) {
        prePublicPctl->pdtPublicPtl();
        prePublicPctl->pdtTrainNumberPlt();
    }
}

void SimulationDialog::slotStart()
{
//    qDebug() << "buttonStart" << buttonStart->isChecked();
    if (buttonStart->isChecked()) {
        sendTimes = 0;
        timerSendTime->start();
        slotSendTime();
    } else {
        buttonStart->setChecked(true);
    }
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
    slotButtonGroupFireProof(buttonGroupFireProof->checkedId());
    qDebug() << "set start";
}




