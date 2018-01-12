#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <QtGui>
#include "QPrePublicPctl.h"
#include "QUdpRecieveThread.h"
#include "comment.h"

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE

class SimulationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SimulationDialog(QWidget *parent = 0);

private:
    void changeEvent(QEvent *event);
    void createAudioTestGroupBox();
    void createCameraSettingGroupBox();
    void createDeviceStateGroupBox();
    void createFireResistanceLinkage();
    void createImagePreviewGroupBox();
    void createMenu();
    void createPublicInfo();
    void createStatusBar();
    void createSwitchTestGroupBox();
    void createTimer();
    void setPublicInfo();
    void translateUI();

    QMenuBar *menuBar;
    QMenu *languageMenu;
    QMenu *helpMenu;
    QAction *cheseAct;
    QAction *englishAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QGroupBox *groupBoxPublicInfo;
    QGroupBox *deviceStateGroupBox;
    QGroupBox *audioTestGroupBox;
    QGroupBox *switchTestGroupBox;
    QGroupBox *cameraSettingGroupBox;
    QGroupBox *imagePreviewGroupBox;
    QTextEdit *bigEditor;
    QLabel *labels[NumGridRows];
    QLineEdit *lineEdits[NumGridRows];
    QPushButton *buttons[NumButtons];
    QDialogButtonBox *buttonBox;
    QLabel *labelAVBoardsState[NumAVBoards];
    QLabel *labelSoftwareVersion;
    QLabel *labelHardwareVersion;
    QLabel *labelCamerasState[NumCameras];
    QLabel *labelAVBoardsStateValue[NumAVBoards];
    QLabel *labelSoftwareVersionValue;
    QLabel *labelHardwareVersionValue;
    QLabel *labelCamerasStateValue[NumCameras];
    QPushButton *buttonRecordSound[NumAudioCannels];
    QPushButton *buttonPlaySound[NumAudioCannels];
    QPushButton *buttonSwitchInTest;
    QPushButton *buttonSwitchOutTest;
    QLabel *labelCameraSetting[NumCameras];
    QLabel *labelCompany;
    QLabel *labelStatus;

    QLabel *labelDataKilometerPost; //公里标
    QLineEdit *lineEditDataKilometerPost;
    QLabel *labelDataSpeed; //速度
    QLineEdit *lineEditDataSpeed;
    QLabel *labelDataLengthCounting;  //计长
    QLineEdit *lineEditDataLengthCounting ;
    QLabel *labelDataNumberOfVehicles;  //辆数
    QLineEdit *lineEditDataNumberOfVehicles;
    QLabel *labelDataStationNo; //车站号
    QLineEdit *lineEditDataStationNo;
    QLabel *labelDataDriverNumber;  //司机号
    QLineEdit *lineEditDataDriverNumber;
    QLabel *labelDataTrainNumber;   //车次
    QLineEdit *lineEditDataTrainNumber1;
    QLineEdit *lineEditDataTrainNumber2;
    QLabel *labelDataIntersectionNumber;    //交路号
    QLineEdit *lineEditDataIntersectionNumber;
    QLabel *labelDataLocomotiveNumber;  //机车号
    QLineEdit *lineEditDataLocomotiveNumber;
    QLabel *labelDataChauffeurOccupancy;    //司机室占用
    QComboBox *comboBoxDataChauffeurOccupancy;
    QLabel *labelDataReconnectionInformation;    //重联信息
    QComboBox *comboBoxDataReconnectionInformation;
    QLabel *labelDataDeviceStatus;    //装置状态
    QComboBox *comboBoxDataDeviceStatus;
    QLabel *labelDataLocomotiveWorkingCondition;    //机车工况
    QComboBox *comboBoxDataLocomotiveWorkingCondition;
//    QLabel *labelDataTheSenderID;    //发送方ID
//    QComboBox *comboBoxDataTheSenderID;
    QLabel *labelDataTypeOfLocomotive;    //机车类型
    QComboBox *comboBoxDataTypeOfLocomotive;
    QComboBox *comboBoxDataShunting;    //调车、非调车
    QRadioButton *radioDataBen;
    QRadioButton *radioDataBu;
    QRadioButton *radioDataKe;
    QRadioButton *radioDataHuo;
    QGroupBox *groupBoxBenBu;
    QGroupBox *groupBoxKeHuo;
    QRadioButton *radioFourPicture;
    QRadioButton *radioEightPicture;
    QPushButton *buttonStart;
    QPushButton *buttonStop;

    QGroupBox *groupBoxFireProof;
    QButtonGroup *buttonGroupFireProof;
    QButtonGroup *buttonGroupFireCamera;
    QPushButton *buttonFireAlarmProbe[NumFirePropes];
    QCheckBox *checkBoxCameras[NumCameras];
    QPushButton *buttonFireSimulation;
    QPushButton *buttonFireLink;
    QPushButton *buttonFireResetParam;

    QTimer *timerSendTime;

    QPrePublicPctl *prePublicPctl;
    QUdpRecieveThread *udpReceiveThread;
    int sendTimes;
    bool isFireAlarmDataValid;

    QMenu *fileMenu;
    QAction *exitAction;

signals:
    
public slots:
    void about();
    void slotButtonFireLink();
    void slotButtonFireResetParam();
    void slotButtonFireSimulation();
    void slotButtonGroupFireCamera(int id);
    void slotButtonGroupFireProof(int id);
    void slotReceiveStatus(int type);
    void slotSendTime();
    void slotStart();
    void slotStop();
    void translateChese();
    void translateEnglish();
};

#endif // SIMULATIONDIALOG_H
