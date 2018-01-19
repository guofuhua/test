#include "QVideoMonitor.h"
//#include <windows.h>
#include "WVideoPreview.h"
//#include "VideoMon.h"
//#include "LibMediaReader.h"
//#pragma comment(lib,"VideoMon.lib")
//#pragma comment(lib,"MediaReader.lib")

QVideoMonitor::QVideoMonitor(QWidget *parent) :
    QDialog(parent)
{
//    m_video = NULL;
    createImagePreviewGroupBox();
    labelImageView = new QLabel(tr("Image View"));
    pixmapLabelArea = new QScrollArea;
    pixmapLabelArea->setWidget(labelImageView);
    pixmapLabelArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    labelImageView->resize(300,300);
//    labelImageView->setMinimumSize(300, 300);
    pixmapLabelArea->setMinimumSize(350, 350);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pixmapLabelArea);
    mainLayout->addWidget(imagePreviewGroupBox);
    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 0);

    setLayout(mainLayout);
    adjustSize();
}

void QVideoMonitor::createImagePreviewGroupBox()
{
    imagePreviewGroupBox = new QGroupBox(tr("Image Preview"));
    radioFourPicture    = new QRadioButton(tr("Four Picture"));
    radioEightPicture   = new QRadioButton(tr("Eight Picture"));
    QHBoxLayout *hbox   = new QHBoxLayout;
    hbox->addWidget(radioFourPicture);
    hbox->addWidget(radioEightPicture);
    imagePreviewGroupBox->setLayout(hbox);
    imagePreviewGroupBox->setEnabled(false);
}

void QVideoMonitor::createVideoChannel()
{
//    if (NULL == m_video) {
//        m_video = new CVideoMon();
//    }
    // TODO: Add extra initialization here
    ///////////////////////////////////////////////////////////////////////
    QString str_ip;
    str_ip = "192.168.60.25";
    m_server_ip = str_ip;
    m_server_port = 7000;

    m_DisplayHandle = NULL;
    QRect rect;
    rect = labelImageView->rect();
    RECT c_rect = {rect.left(), rect.top(), rect.right(), rect.bottom()};
//    m_DisplayHandle = m_video->VideoCreate(CWnd::FromHandle((HWND)(labelImageView->winId())), c_rect, str_ip.toStdString().c_str());   // 更改IP，端口号
    m_DisplayHandle = WVideoCreate((HWND)(labelImageView->winId()), c_rect, (TCHAR*)str_ip.toStdString().c_str(), str_ip.length());   // 更改IP，端口号

    // 默认单通道
//    m_video->VideoSetMode(m_DisplayHandle, m_chn_1);
    WVideoSetMode(m_DisplayHandle, m_chn_1);

    if(!m_DisplayHandle)
    {
        QMessageBox::information(NULL,"NO","不能创建对象!");
        return ;
    }
    ///////////////////////////////////////////////////////////////////////
//    m_pictureCtrl.GetClientRect(&ClientRect);
//    Old_Rect = ClientRect;
    b_run = FALSE;
    return ;  // return TRUE  unless you set the focus to a control
}
