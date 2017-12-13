HEADERS       = ftpwindow.h
SOURCES       = ftpwindow.cpp \
                main.cpp
RESOURCES    += ftp.qrc
QT           += network

RC_FILE      = qftp.rc

TRANSLATIONS += cn.ts

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/qftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/network/qftp
INSTALLS += target sources
