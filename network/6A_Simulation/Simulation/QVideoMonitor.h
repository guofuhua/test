#ifndef QVIDEOMONITOR_H
#define QVIDEOMONITOR_H

#include <QDialog>
#include <QtGui>
#include <windows.h>
//#include "CVideoMon.h"
//#include <afxwin.h>

class QVideoMonitor : public QDialog
{
    Q_OBJECT
public:
    explicit QVideoMonitor(QWidget *parent = 0);
    
    void createImagePreviewGroupBox();
    void createVideoChannel();
    QGroupBox *imagePreviewGroupBox;
    QRadioButton *radioFourPicture;
    QRadioButton *radioEightPicture;
    QScrollArea *pixmapLabelArea;
    QLabel *labelImageView;

    HANDLE	m_DisplayHandle;
//    CRect	ClientRect, Old_Rect;
    BOOL	b_run;
    QString	m_server_ip;
    int		m_server_port;
    int		m_chn_1;
    int		m_chn_2;
    int		m_chn_3;
    int		m_chn_4;
//    CVideoMon *m_video;
signals:
    
public slots:
    
};

#endif // QVIDEOMONITOR_H
