#include "SimulationDialog.h"

SimulationDialog::SimulationDialog(QWidget *parent) :
    QDialog(parent)
{
    createMenu();
    createHorizontalGroupBox();
    createGridGroupBox();
    createFormGroupBox();
    createAudioTestGroupBox();
    createCameraSettingGroupBox();
    createDeviceStateGroupBox();
    createImagePreviewGroupBox();
    createSwitchTestGroupBox();
    createStatusBar();

    bigEditor = new QTextEdit;
    bigEditor->setPlainText(tr("This widget takes up all the remaining space "
                               "in the top-level layout."));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *statusLayout = new QVBoxLayout;
    statusLayout->addWidget(deviceStateGroupBox);
    statusLayout->addWidget(audioTestGroupBox);
    statusLayout->addWidget(switchTestGroupBox);
    statusLayout->addWidget(cameraSettingGroupBox);
    statusLayout->addWidget(imagePreviewGroupBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;

//    mainLayout->setMenuBar(menuBar);

    mainLayout->addWidget(horizontalGroupBox);
    mainLayout->addWidget(gridGroupBox);
    mainLayout->addWidget(formGroupBox);
    mainLayout->addWidget(bigEditor);
    mainLayout->addWidget(buttonBox);

    QGridLayout *Layout = new QGridLayout;

    Layout->setMenuBar(menuBar);
//    Layout->setSpacing(50);
//    statusLayout->setSpacing(25);
//    mainLayout->setSpacing(25);
    Layout->addLayout(statusLayout, 0, 0);
    Layout->addLayout(mainLayout, 0, 1);


    Layout->addWidget(labelCompany, 1, 0);
    Layout->addWidget(labelStatus, 1,1);

    setLayout(Layout);

    setWindowTitle(tr("Basic Layouts"));
}

void SimulationDialog::createAudioTestGroupBox()
{
    audioTestGroupBox = new QGroupBox(tr("Audio Test"));

    QGridLayout *layout = new QGridLayout;

    for (int i = 0; i < NumAudioCannels; ++i) {
        buttonRecordSound[i] = new QPushButton(tr("Channel %1 Record").arg(i + 1));
        layout->addWidget(buttonRecordSound[i], 0, i);
    }
    for (int i = 0; i < NumAudioCannels; ++i) {
        buttonPlaySound[i] = new QPushButton(tr("Channel %1 Play").arg(i + 1));
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


void SimulationDialog::createFormGroupBox()
{
    formGroupBox = new QGroupBox(tr("Form layout"));
    QFormLayout *layout = new QFormLayout;
    layout->addRow(new QLabel(tr("Line 1:")), new QLineEdit);
    layout->addRow(new QLabel(tr("Line 2, long text:")), new QComboBox);
    layout->addRow(new QLabel(tr("Line 3:")), new QSpinBox);
    formGroupBox->setLayout(layout);
}


void SimulationDialog::createGridGroupBox()
{
    gridGroupBox = new QGroupBox(tr("Grid layout"));

    QGridLayout *layout = new QGridLayout;

    for (int i = 0; i < NumGridRows; ++i) {
        labels[i] = new QLabel(tr("Line %1:").arg(i + 1));
        lineEdits[i] = new QLineEdit;
        layout->addWidget(labels[i], i + 1, 0);
        layout->addWidget(lineEdits[i], i + 1, 1);
    }

    smallEditor = new QTextEdit;
    smallEditor->setPlainText(tr("This widget takes up about two thirds of the "
                                 "grid layout."));
    layout->addWidget(smallEditor, 0, 2, 4, 1);

    layout->setColumnStretch(1, 10);
    layout->setColumnStretch(2, 20);
    gridGroupBox->setLayout(layout);
}

void SimulationDialog::createHorizontalGroupBox()
{
    horizontalGroupBox = new QGroupBox(tr("Horizontal layout"));
    QHBoxLayout *layout = new QHBoxLayout;

    for (int i = 0; i < NumButtons; ++i) {
        buttons[i] = new QPushButton(tr("Button %1").arg(i + 1));
        layout->addWidget(buttons[i]);
    }
    horizontalGroupBox->setLayout(layout);
}

void SimulationDialog::createImagePreviewGroupBox()
{
    imagePreviewGroupBox = new QGroupBox(tr("Image Preview"));
}

void SimulationDialog::createMenu()
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
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
    layout->addWidget(buttonSwitchOutTest);
    layout->addWidget(buttonSwitchInTest);

    switchTestGroupBox->setLayout(layout);
}

