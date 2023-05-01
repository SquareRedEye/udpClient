#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QUdpSocket>
class Client : public QDialog
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();


private:
    QGraphicsPixmapItem* item;
    QGraphicsScene *scene;
    QUdpSocket *socket;
    QImage *image;
    QLabel *label3;
    QLabel *label4;
    QLabel *label5;
private slots:
    void changeCrosshairColor(int i);
    void changeBackgroundColor(int i);
    void getAndSet();
};
