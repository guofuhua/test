#include <QtGui>

unsigned int p1 = 0, p2 = 0, score = 0;

#define EX(x) ((((x)&0xf000ull)>>12)|(((x)&0xf00ull)<<8)|(((x)&0xf0ull)<<28)|(((x)&0xfull)<<48))

quint64 pats[7*4] = {

  EX(0x0660),EX(0x0660),EX(0x0660),EX(0x0660), EX(0x4460),EX(0x0740),EX(0x6220),EX(0x02e0),

  EX(0x2260),EX(0x0470),EX(0x6440),EX(0x0e20), EX(0x0630),EX(0x2640),EX(0x0630),EX(0x2640),

  EX(0x0360),EX(0x4620),EX(0x0360),EX(0x4620), EX(0x0720),EX(0x2620),EX(0x0270),EX(0x2320),

  EX(0x4444),EX(0x0f00),EX(0x4444),EX(0x0f00),

};

quint16 map[28] = {0xffff,0xffff,0xffff,0xffff};

struct MainWindow : public QMainWindow {

 MainWindow(QWidget *parent = 0) : QMainWindow(parent),timer(new QTimer(this)){

     setGeometry(400,200,250,250);

     connect(timer, SIGNAL(timeout()), this, SLOT(raise()));

 }

 virtual bool
event ( QEvent * e ){

     if(e->type() == QEvent::ZOrderChange)keyPressEvent(0);

     return QMainWindow::event(e);

 }

 int blockDrop(bool real = true){

     if(real)*(quint64*)&map[curpos.y()] |= (pats[p1%28]<<curpos.x());

     for(int i=0;i<4 && real; i++)

         if(map[i+curpos.y()] == 0xffff){

             memmove(&map[5],&map[4], (i+curpos.y() - 4)*2);

             map[4] = ~0x1ff8;

             score++;

         }

     timer->setInterval(score<450 ? 500-(score/50)*50 : 50);

     p1 = p2;

     curpos = QPoint(6,0);

     while(!isValid(curpos.x(),curpos.y()) && curpos.y()<6)curpos.ry()++;

     if(curpos.y()>=6) timer->stop();

     return p2 = qrand();

 }

 void paintEvent(QPaintEvent *){

     QPainter painter(this);

     painter.drawImage(5,5,QImage((uchar*)map,16,28,2,QImage::Format(2)).scaled(160,280),28,38,104,204);

     QImage img = QImage((uchar*)&pats[p1%28],4,4,2,QImage::Format(2)).convertToFormat(QImage::Format(3));

     painter.drawImage(curpos.x()*10-23,curpos.y()*10-33,(img.setColor(0,0),img.scaled(40,40)));

     painter.drawImage(130,30,QImage((uchar*)&pats[p2%28],4,4,2,QImage::Format(2)).scaled(40,40));

     painter.drawText(130,20,QString("score:%1").arg(score));

     if(!timer->isActive())painter.drawText(130,85,QString("Game over"));

     painter.drawText(130,100,80,140,0,"P-Restart\nW-Rotate\nA-Left\nD-Right\nS-Down\nSpace-Drop");

 }

 virtual void keyPressEvent ( QKeyEvent * e ){

     if(!timer->isActive()){

         if(e->key() == Qt::Key_P || e->key() == Qt::Key_Enter){

             *(quint64*)&map[24] = 0xfffefffefffefffeull;

             for(int i=4;i<24;i++) map[i] = ~0x1ff8;

             qsrand(QTime::currentTime().msec());

             score = 0;

             p2 = qrand();

             blockDrop(false);

             timer->start();

         }

     }else if(!e || e->key() == Qt::Key_Down || e->key() == Qt::Key_S){

         isValid(curpos.x(),curpos.y()+1) ? curpos.ry()++ : blockDrop();

     }else if(e->key() == Qt::Key_Space){

         while(isValid(curpos.x(),curpos.y()+1)) curpos.ry()++;

         blockDrop();

     }else if(e->key() == Qt::Key_Left || e->key() == Qt::Key_A){

         if(isValid(curpos.x()-1,curpos.y())) curpos.rx()--;

     }else if(e->key() == Qt::Key_Right || e->key() == Qt::Key_D){

         if(isValid(curpos.x()+1,curpos.y())) curpos.rx()++;

     }else if(e->key() == Qt::Key_Up || e->key() == Qt::Key_W){

         p1 = ((p1&(~3)) | ((p1+1)&3));

         if(!isValid(curpos.x(),curpos.y())) p1 = ((p1&(~3)) | ((p1-1)&3));

     }

     update();

 }

 bool isValid(int x, int y){ return !( (pats[p1%28]<<x) & *(quint64*)&map[y]); }

 QTimer* timer;

 QPoint  curpos;

};

int main(int argc, char *argv[]){

   QApplication app(argc, argv);

   MainWindow mainWindow;

   mainWindow.show();

   return app.exec();

}
