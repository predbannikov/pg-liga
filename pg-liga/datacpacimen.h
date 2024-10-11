#ifndef DATASAMPLE_H
#define DATASAMPLE_H

#include "QString"
#include <QtCore>
#include <QWidget>
#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Dialog;
}

class Dialog: public QDialog
{
    Q_OBJECT
public:
        explicit Dialog(QWidget *parent = nullptr);
        ~Dialog();
    bool access();
    bool valid();
    void setpararms(QString name, float height, float diametr);
    void setDefault();


    double getheight(){return _height;}
    double getdiametr(){return _diametr;}
    QString getname(){return _name;}
    void showEvent (QShowEvent * event) override;

private slots:
    void pushData();

    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QString _name;
    double _height;
    double _diametr;
};


//class Dataspaciment: public QAbstractItemModel
//{
//    Q_OBJECT
//public:
//    explicit Dataspaciment(QObject *parent = nullptr);
//    ~Dataspaciment();

//    bool access();
//    void setpararms(QString name, float height, float diametr);

//    double getheight(){return _height;}
//    double getdiametr(){return _diametr;}
//    QString getname(){return _name;}

//private:

//    QString _name;
//    float _height;
//    float _diametr;
//};

#endif // DATASAMPLE_H
