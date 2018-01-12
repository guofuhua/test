#include "mainwindow.h"
#include <QtGui>
#if (0)
#include "bmp.c"
#else
#include "bmp.h"
int show_photo(const char *bmpdata, const int bmpsize)
{
    if(NULL == bmpdata || bmpsize < 54)
        return -1;

    BitMapFileHeader FileHead;
    BitMapInfoHeader InfoHead;
    RgbQuad QuadInfo[256];
    const char *pbmpdata = bmpdata;

    memset(&QuadInfo, 0, sizeof(QuadInfo));
    //读文件信息
    memcpy(&FileHead, pbmpdata, sizeof(BitMapFileHeader));
    pbmpdata += sizeof(BitMapFileHeader);
    if (memcmp(FileHead.bfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file\n");
        return -1;
    }

    //读位图信息
    memcpy(&InfoHead, pbmpdata, sizeof(BitMapInfoHeader));
    pbmpdata += sizeof(BitMapInfoHeader);

    //读
    if (FileHead.bfOffBits > 54) {
        memcpy(&QuadInfo, pbmpdata, FileHead.bfOffBits - 54);
    }

    //跳转至数据区
    pbmpdata = bmpdata + FileHead.bfOffBits;

    // 输出BMP文件的位图文件头的所有信息
    printf("位图文件头主要是对位图文件的一些描述:BMPFileHeader\n\n");
    printf("文件标识符 = %c%c\n", FileHead.bfType[0], FileHead.bfType[1]);
    printf("BMP 文件大小 = %lu 字节\n", FileHead.bfSize);
    printf("保留值1 = %d \n", FileHead.bfReserved1);
    printf("保留值2 = %d \n", FileHead.bfReserved2);
    printf("文件头的最后到图像数据位开始的偏移量 = %lu 字节\n", FileHead.bfOffBits);

    // 输出BMP文件的位图信息头的所有信息
    printf("\n\n位图信息头主要是对位图图像方面信息的描述:BMPInfo\n\n");
    printf("信息头的大小 = %lu 字节\n", InfoHead.biSize);
    printf("位图的高度 = %ld \n", InfoHead.biHeight);
    printf("位图的宽度 = %ld \n", InfoHead.biWidth);
    printf("图像的位面数(位面数是调色板的数量,默认为1个调色板) = %d \n", InfoHead.biPlanes);
    printf("每个像素的位数 = %d 位\n", InfoHead.biBitCount);
    printf("压缩类型 = %lu \n", InfoHead.biCompression);
    printf("图像的大小 = %lu 字节\n", InfoHead.biSizeImage);
    printf("水平分辨率 = %ld \n", InfoHead.biXPelsPerMeter);
    printf("垂直分辨率 = %ld \n", InfoHead.biYPelsPerMeter);
    printf("使用的色彩数 = %lu \n", InfoHead.biClrUsed);
    printf("重要的色彩数 = %lu \n", InfoHead.biClrImportant);

    printf("\n\n\n压缩说明：有0（不压缩），1（RLE 8，8位RLE压缩），2（RLE 4，4位RLE压缩，3（Bitfields，位域存放）");
    fflush(stdout);
    return 0;
}

int parse_bmp(const char *bmpdata, const int bmpsize, char **data, unsigned long *size, long *width, long *height)
{
    if(NULL == bmpdata || bmpsize < 54)
        return -1;

    BitMapFileHeader FileHead;
    BitMapInfoHeader InfoHead;
    RgbQuad QuadInfo[256];
    long bmp_height = 0;
    const char *pbmpdata = bmpdata;

    memset(&QuadInfo, 0, sizeof(QuadInfo));
    //读文件信息
    memcpy(&FileHead, pbmpdata, sizeof(BitMapFileHeader));
    pbmpdata += sizeof(BitMapFileHeader);
    if (memcmp(FileHead.bfType, "BM", 2) != 0)
    {
        printf("it's not a BMP file\n");
        return -1;
    }

    //读位图信息
    memcpy(&InfoHead, pbmpdata, sizeof(BitMapInfoHeader));
    pbmpdata += sizeof(BitMapInfoHeader);

    //读
    if (FileHead.bfOffBits > 54) {
        memcpy(&QuadInfo, pbmpdata, FileHead.bfOffBits - 54);
    }

    //跳转至数据区
    pbmpdata = bmpdata + FileHead.bfOffBits;

    *width = InfoHead.biWidth;
    *height = InfoHead.biHeight;
    if (InfoHead.biHeight < 0 ) {
        bmp_height = -InfoHead.biHeight;
    } else {
        bmp_height = InfoHead.biHeight;
    }
    int data_row_size = 0;
//    if (InfoHead.biWidth < 4) {
//        data_row_size = 12;
//    } else if (InfoHead.biWidth % 2) {
//        data_row_size = InfoHead.biWidth * 3 + 1;
//    } else {
//        data_row_size = InfoHead.biWidth * 3;
//    }
    data_row_size = InfoHead.biWidth * 3;
    if (data_row_size % 4) {
        data_row_size += 4 - data_row_size % 4;
    }
    *size = data_row_size * bmp_height;

    *data = (char *)malloc(*size);
    if (NULL == *data) {
        printf("malloc memory failed!\n");
        return -1;
    }
    memset(*data, 0, *size);

    unsigned char *pparse_data = (unsigned char *)*data;
    const char *pcurrent_row = NULL;
    unsigned char *pcurrent_row_data = NULL;
    int row_size = 0;
//    if (InfoHead.biWidth < 4) {
//        row_size = 4 * InfoHead.biBitCount / 8;
//    } else {
//        row_size = InfoHead.biWidth * InfoHead.biBitCount / 8;
//    }

//    if (row_size < 4) {
//        row_size = 4;
//    } else if (row_size % 2) {
//        row_size++;
//    }

    row_size = (InfoHead.biWidth * InfoHead.biBitCount + 7)/ 8;
    if (row_size % 4) {
        row_size += 4 - row_size % 4;
    }
//    int quad_index;

    for (int i = 0; i < bmp_height; i++) {
        if (NULL == pcurrent_row) {
            pcurrent_row = pbmpdata;
        } else {
            pcurrent_row += row_size;
        }
        pbmpdata = pcurrent_row;

        if (NULL == pcurrent_row_data) {
            pcurrent_row_data = pparse_data;
        } else {
            pcurrent_row_data += data_row_size;
        }
        pparse_data = pcurrent_row_data;
        for (int j = 0; j < InfoHead.biWidth; j++) {
            switch (InfoHead.biBitCount)
            {
            case 1:
//                quad_index = (pbmpdata[j / 8] >> (7 - j % 8)) & 0x1;
//                *pparse_data++ = QuadInfo[quad_index].Blue;
//                *pparse_data++ = QuadInfo[quad_index].Green;
//                *pparse_data++ = QuadInfo[quad_index].Red;
                memcpy(pparse_data, &QuadInfo[(pbmpdata[j / 8] >> (7 - j % 8)) & 0x1], 3);
                pparse_data += 3;
                break;
            case 2:
                memcpy(pparse_data, &QuadInfo[(pbmpdata[j / 4] >> (6 - j % 4 * 2)) & 0x3], 3);
                pparse_data += 3;
                break;
            case 4:
                memcpy(pparse_data, &QuadInfo[(pbmpdata[j / 2] >> (4 - j % 2 * 4)) & 0xf], 3);
                pparse_data += 3;
                break;
            case 8:
                memcpy(pparse_data, &QuadInfo[(unsigned char)pbmpdata[j]], 3);
                pparse_data += 3;
                break;
            case 24:
                *pparse_data++ = *pbmpdata++;
                *pparse_data++ = *pbmpdata++;
                *pparse_data++ = *pbmpdata++;
                break;
            default:
                printf("biBitCount is %d ,not 1,2,4,8,24\n", InfoHead.biBitCount);
                free(*data);
                *data = NULL;
                return -1;
                break;
            }
        }
    }

    return 0;
}


int componse_bmp(const unsigned long bmpsize, long width, long height, BitMapFileHeader *FileHead, BitMapInfoHeader *InfoHead)
{
    memset(FileHead, 0, sizeof(BitMapFileHeader));
    memset(InfoHead, 0, sizeof(BitMapInfoHeader));

    FileHead->bfType[0] = 'B';
    FileHead->bfType[1] = 'M';
    FileHead->bfSize = 54 + bmpsize;
    FileHead->bfReserved1 = 0;
    FileHead->bfReserved2 = 0;
    FileHead->bfOffBits = 54;

    InfoHead->biSize = 40;
    InfoHead->biWidth = width;
    InfoHead->biHeight = height;
    InfoHead->biPlanes = 1;
    InfoHead->biBitCount = 24;
    InfoHead->biCompression = 0;
    InfoHead->biSizeImage = bmpsize;
    InfoHead->biXPelsPerMeter = 0;
    InfoHead->biYPelsPerMeter = 0;
    InfoHead->biClrUsed = 0;
    InfoHead->biClrImportant = 0;

//    memcpy(bmpdata, FileHead, sizeof(BitMapFileHeader));
//    memcpy(bmpdata + 14, InfoHead, sizeof(BitMapInfoHeader));

    return 0;
}

/*********************/
void bmpConvert(const char *bmpdata, const int bmpsize,const char* sOut){
    FILE* fout=fopen(sOut,"wb");
    int ret = 0;
    char *data = NULL;
    long width = 0;
    long height = 0;
    unsigned long data_size = 0;
    BitMapFileHeader FileHead;
    BitMapInfoHeader InfoHead;
    size_t write_count = 0;

    if (NULL == fout) {
        printf("fopen failed\n");
        return;
    }

    ret = parse_bmp(bmpdata, bmpsize, &data, &data_size, &width, &height);
    if ((0 == ret) && (NULL != data)) {
        ret = componse_bmp(data_size, width, height, &FileHead, &InfoHead);
        if (0 == ret) {
            write_count = fwrite(&FileHead, 1, sizeof(BitMapFileHeader), fout);
            if (sizeof(BitMapFileHeader) != write_count) {
                printf("fill bmp file header write_count=%d\n", write_count);
            }
            write_count = fwrite(&InfoHead, 1, sizeof(BitMapInfoHeader), fout);
            if (sizeof(BitMapInfoHeader) != write_count) {
                printf("fill bmp info header write_count=%d\n", write_count);
            }
            write_count = fwrite(data, 1, data_size, fout);
            if (data_size != write_count) {
                printf("fill bmp write_count=%d\n", write_count);
            }

//            data_size += 54;
//            write_count = fwrite(data, 1, data_size, fout);
//            if (data_size != write_count) {
//                printf("fill bmp write_count=%d\n", write_count);
//            }
        }
    }

    fclose(fout);
    fflush(stdout);
    if (NULL != data) {
        free(data);
    }
}

char *create_canvas(const int canvas_width, const int canvas_height, unsigned long *size)
{
    if((canvas_width <= 0) || (canvas_height <= 0))
        return NULL;

    int data_row_size = 0;

    data_row_size = canvas_width * 3;
    if (data_row_size % 4) {
        data_row_size += 4 - data_row_size % 4;
    }
    *size = data_row_size * canvas_height;

    return (char *)malloc(*size);
}

bool save_bmp(const char *canvas, const unsigned long size, const long width, const long height, const char* sOut){
    FILE* fout=fopen(sOut,"wb");
    int ret = 0;
//    char *canvas = create_canvas(width, height, &row_size);
//    unsigned long data_size = row_size * height;
    BitMapFileHeader FileHead;
    BitMapInfoHeader InfoHead;
    size_t write_count = 0;

    if (NULL == fout) {
        printf("fopen failed\n");
        return false;
    }

    if (NULL != canvas) {
        ret = componse_bmp(size, width, height, &FileHead, &InfoHead);
        if (0 == ret) {
            write_count = fwrite(&FileHead, 1, sizeof(BitMapFileHeader), fout);
            if (sizeof(BitMapFileHeader) != write_count) {
                printf("fill bmp file header write_count=%d\n", write_count);
            }
            write_count = fwrite(&InfoHead, 1, sizeof(BitMapInfoHeader), fout);
            if (sizeof(BitMapInfoHeader) != write_count) {
                printf("fill bmp info header write_count=%d\n", write_count);
            }
            write_count = fwrite(canvas, 1, size, fout);
            if (size != write_count) {
                printf("fill bmp write_count=%d\n", write_count);
            }
        }
    }

    fclose(fout);
    fflush(stdout);

    return true;
}

bool general_default_rect()
{
    int width = 64;
    int height = 64;
    unsigned long size = 0;
    char *canvas = create_canvas(width, height, &size);
    bool ret = false;
//    int row_size = size / height;
    memset(canvas, 0, size);

    if (NULL != canvas) {
        if (save_bmp(canvas, size, width, height, "default_rect.bmp")) {
            ret = true;
        }
        free(canvas);
    }
    return ret;
}

bool general_rect(int x, int y, int w, int h)
{
    int width = 64;
    int height = 64;
    unsigned long size = 0;
    char *canvas = create_canvas(width, height, &size);
    bool ret = false;
    int row_size = size / height;
    RgbQuad quadInfo = {40, 250, 60, 0};
    memset(canvas, 0xFF, size);

    for (int i = 0; i <= w; i++) {
        memcpy(&canvas[y * row_size + 3 * (x + i)], &quadInfo, 3);
        memcpy(&canvas[(y + h) * row_size + 3 * (x + i)], &quadInfo, 3);
    }

    for (int i = 1; i < h; i++) {
        memcpy(&canvas[(y + i) * row_size + 3 * x], &quadInfo, 3);
        memcpy(&canvas[(y + i) * row_size + 3 * (x + w)], &quadInfo, 3);
    }

    if (NULL != canvas) {
        if (save_bmp(canvas, size, width, -height, "default_rect.bmp")) {
            ret = true;
        }
        free(canvas);
    }
    return ret;
}
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
//    Qt::KeepAspectRatio,
//    Qt::SmoothTransformation
    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    setCentralWidget(scrollArea);

    resize(500, 400);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newRect()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(newAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        QImage image(fileName);

        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray bmpData = file.readAll();
            char *data = bmpData.data();
            qDebug() << "bmpdata size" << bmpData.size();
            qDebug() << "return " << show_photo(data, bmpData.size());
            qDebug() << "bmpConvert ";
            bmpConvert(data, bmpData.size(), file.fileName().append("g.bmp").toStdString().c_str());
            file.close();
        }
        imageLabel->setPixmap(QPixmap::fromImage(image).scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation));

        scaleFactor = 1.0;

        fitToWindowAct->setEnabled(true);
        updateActions();

        if (!fitToWindowAct->isChecked())
            imageLabel->adjustSize();
    }
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void MainWindow::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void MainWindow::newRect()
{
    qDebug() << "newRect" << general_rect(8, 5, 18, 25);
}

void MainWindow::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 10.0);
    zoomOutAct->setEnabled(scaleFactor > 0.1);
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
