#include "client.h"
#include <QDebug>
#include <QStandardPaths>
const QString crosshairColor[] = {
    "Красный",
    "Чёрный"
}; //Цвета прицела
const QString backgroundColor[] = {
    "Чёрный",
    "Белый",
    "Серый"
}; //Цвета фона
const QString crosshairPath[] = {
    "D:/Qt projects/udpClient/Client/crosshair red",
    "D:/Qt projects/udpClient/Client/crosshair black"
}; //Места расположения прицельных сеток
const QBrush backgroundBrush[] = {
    Qt::darkGray,
    Qt::white,
    Qt::gray
}; //Коды цветов фона
const float scale = 0.25;//коэффициент увеличения прицела
const unsigned short windowHeight{600}; // Высота окна
const unsigned short windowWidth{800}; // Ширина окна
const unsigned short sceneHeight{300}; // Высота рабочей области
const unsigned short scenewWidth{400}; // Ширина рабочей области

Client::Client(QWidget *parent)
    : QDialog(parent)
{
    //Главное окно
    setWindowTitle("Client");
    QDialog *window = new QDialog();
    window->resize(windowWidth,windowHeight);
    window->show();
    //Окно для отображения прицела
    QGraphicsView *view = new QGraphicsView;
    scene = new QGraphicsScene(0,0,scenewWidth,sceneHeight);
    scene->setBackgroundBrush(backgroundBrush[0]);
    view->setScene(scene);
    scene->addRect(scene->sceneRect());

    //Статический прицел
    unsigned short l = scene->height()/10;
    scene->addLine(
                scene->width()/2-l,scene->height()/2,
                scene->width()/2+l,scene->height()/2,
                QColor("green")
                );
    scene->addLine(
                scene->width()/2,scene->height()/2-l,
                scene->width()/2,scene->height()/2+l,
                QColor("green")
                );

    //Рабочий прицел
    image = new QImage(crosshairPath[0]);
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    item->setScale(scale);
    //По умолчанию рабочий прицел расположен в центре
    item->setPos(
                scenewWidth/2-(image->width()*scale)/2,
                sceneHeight/2-(image->height()*scale)/2
                );
    scene->addItem(item);

    //Селектор фона
    QLabel *label1 = new QLabel("Цвет фона");
    label1->setAlignment(Qt::AlignHCenter);
    QComboBox *combo2 = new QComboBox;
    for(unsigned long long i = 0; i < sizeof(backgroundColor)/sizeof(QString); i++)
    {
        combo2->addItem(backgroundColor[i]);
    }
    connect(combo2, SIGNAL(currentIndexChanged(int)), this, SLOT(changeBackgroundColor(int)));
    QVBoxLayout *column1 = new QVBoxLayout;
    column1->addWidget(label1);
    column1->addWidget(combo2);

    //Селектор цвета рабочего прицела
    QLabel *label2 = new QLabel("Цвет прицела");
    label2->setAlignment(Qt::AlignHCenter);
    QComboBox *combo1 = new QComboBox;
    for(unsigned long long i = 0; i < sizeof(crosshairColor)/sizeof(QString); i++)
    {
        combo1->addItem(crosshairColor[i]);
    }
    connect(combo1, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCrosshairColor(int)));
    QVBoxLayout *column2 = new QVBoxLayout;
    column2->addWidget(label2);
    column2->addWidget(combo1);

    //Компоновщик параметров положения прицела
    QVBoxLayout *column3 = new QVBoxLayout;
    label3 = new QLabel("Угол камеры: 2 град.");
    label4 = new QLabel("Отступ по горизонтали: 0.00");
    label5 = new QLabel("Отступ по вертикали: 0.00");
    label3->setAlignment(Qt::AlignHCenter);
    label4->setAlignment(Qt::AlignHCenter);
    label5->setAlignment(Qt::AlignHCenter);
    column3->addWidget(label3);
    column3->addWidget(label4);
    column3->addWidget(label5);

    //Компоновщик панели настроек
    QHBoxLayout *row = new QHBoxLayout;
    row -> addLayout(column1);
    row -> addLayout(column2);
    row -> addLayout(column3);

    //Основной компоновщик
    QVBoxLayout*layout = new QVBoxLayout;
    window->setLayout(layout);
    layout->addWidget(view);
    layout->addLayout(row);

    socket = new QUdpSocket(this);
    socket->bind(5555);
    connect(socket,SIGNAL(readyRead()),this,SLOT(getAndSet()));
}

void Client::changeCrosshairColor(int i)
{
    //Меняет цвет прицела
    item->setPixmap(QPixmap::fromImage(QImage(crosshairPath[i])));
}

void Client::changeBackgroundColor(int i)
{
    //Меняет цвет фона
    scene->setBackgroundBrush(backgroundBrush[i]);
}

void Client::getAndSet()
{
    //Получает данные от сервера и меняет положение прицела на сцене
    QByteArray datagram;
    datagram.resize(socket->pendingDatagramSize());
    socket->readDatagram(datagram.data(),datagram.size());
    QString msg = datagram.data();

    label3->setText("Угол камеры: " + msg.split(" ")[0] + " град.");
    label4->setText("Отступ по горизонтали: " + msg.split(" ")[1]);
    label5->setText("Отступ по вертикали: " + msg.split(" ")[2]);
    /*
     * точке с горизонтальным смещением (-1) и вертикальным (-1)
     * соответствует левый верхний угол сцены, а точке с (1;1)
     * правый нижний
     */
    float
            hoffset = msg.split(" ")[1].toFloat(),//Смещение по горизонтали
            voffset = msg.split(" ")[2].toFloat();//Смещение по вертикали
    item->setPos(
                scenewWidth/2-(image->width()*scale)/2 + scenewWidth/2*hoffset,
                sceneHeight/2-(image->height()*scale)/2 + sceneHeight/2*voffset
                );
}

Client::~Client()
{

}
