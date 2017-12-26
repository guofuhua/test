#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <QtGui>

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
    void createAudioTestGroupBox();
    void createCameraSettingGroupBox();
    void createDeviceStateGroupBox();
    void createFormGroupBox();
    void createGridGroupBox();
    void createHorizontalGroupBox();
    void createImagePreviewGroupBox();
    void createMenu();
    void createStatusBar();
    void createSwitchTestGroupBox();

    enum { NumGridRows = 3, NumAVBoards = 3, NumButtons = 4, NumAudioCannels = 4, NumCameras = 14 };

    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QGroupBox *gridGroupBox;
    QGroupBox *formGroupBox;
    QGroupBox *deviceStateGroupBox;
    QGroupBox *audioTestGroupBox;
    QGroupBox *switchTestGroupBox;
    QGroupBox *cameraSettingGroupBox;
    QGroupBox *imagePreviewGroupBox;
    QTextEdit *smallEditor;
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

    QLabel *labelDataKilometerPost; //�����
    QLineEdit *lineEditDataKilometerPost;
    QLabel *labelDataSpeed; //�ٶ�
    QLineEdit *lineEditDataSpeed;
    QLabel *labelDataLengthCounting;  //�Ƴ�
    QLineEdit *lineEditDataLengthCounting ;
    QLabel *labelDataNumberOfVehicles;  //����
    QLineEdit *lineEditDataNumberOfVehicles;
    QLabel *labelDataStationNo; //��վ��
    QLineEdit *lineEditDataStationNo;
    QLabel *labelDataDriverNumber;  //˾����
    QLineEdit *lineEditDataDriverNumber;
    QLabel *labelDataTrainNumber;   //����
    QLineEdit *lineEditDataTrainNumber1;
    QLineEdit *lineEditDataTrainNumber2;
    QLabel *labelDataIntersectionNumber;    //��·��
    QLineEdit *lineEditDataIntersectionNumber;
    QLabel *labelDataLocomotiveNumber;  //������
    QLineEdit *lineEditDataLocomotiveNumber;
    QLabel *labelDataChauffeurOccupancy;    //˾����ռ��
    QComboBox *comboBoxDataChauffeurOccupancy;
    QLabel *labelDataReconnectionInformation;    //������Ϣ
    QComboBox *comboBoxDataReconnectionInformation;
    QLabel *labelDataDeviceStatus;    //װ��״̬
    QComboBox *comboBoxDataDeviceStatus;
    QLabel *labelDataLocomotiveWorkingCondition;    //��������
    QComboBox *comboBoxDataLocomotiveWorkingCondition;
    QLabel *labelDataTheSenderID;    //���ͷ�ID
    QComboBox *comboBoxDataTheSenderID;
    QLabel *labelDataTypeOfLocomotive;    //��������
    QComboBox *comboBoxDataTypeOfLocomotive;
    QComboBox *comboBoxDataShunting;    //�������ǵ���


    QMenu *fileMenu;
    QAction *exitAction;

signals:
    
public slots:
    
};

#endif // SIMULATIONDIALOG_H
