#include "datacpacimen.h"
#include "ui_dataspecimen.h"

Dialog::Dialog(QWidget *parent):
    QDialog(parent),
    _diametr(-1.0),
    _height(-1.0),
    _name(""),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,this, &Dialog::pushData);
    ui->diametr->setRange(0, 1000);
    ui->height->setRange(0, 1000);
    ui->diametr->setSingleStep(0.1);
    ui->height->setSingleStep(0.1);


}

Dialog::~Dialog(){

}


void Dialog::pushData()
{

    if (access())
    {
        _name = ui->name->text();
        _height = ui->height->value();
        _diametr = ui->diametr->value();
    }
    else
    {
        QMessageBox::warning(this, "Ошибка ввода данных", "Проверьте правильность ввода данных, попытайтесь еще раз");
    }
}
void Dialog::setDefault()
{
    _name = "Образец 101";
    _diametr = 76.0;
    _height = 38.0;
}

bool Dialog::valid()
{
    if (_name != "" && _diametr > 1 && _height > 1)
        return true;

    return false;
}

bool Dialog::access()
{
    if (ui->name->text() == "" || ui->diametr->value() < 1 || ui->height->value() < 1)
        return false;

    return true;
}

void Dialog::setpararms(QString name, float height, float diametr)
{
    _name = name;
    _height = height;
    _diametr = diametr;
}


void Dialog::on_pushButton_clicked()
{
    ui->name->setText("Образец " + QString::number(rand()%10000));
    ui->diametr->setValue(38.0);
    ui->height->setValue(76);
    emit ui->buttonBox->accepted();
}

void Dialog::showEvent (QShowEvent * event)
{
    if(valid())
    {
        ui->name->setText(_name);
        ui->height->setValue(_height);
        ui->diametr->setValue(_diametr);
    }
}
//Dataspaciment::Dataspaciment(QObject *parent):
//    QAbstractItemModel(parent),
//    _diametr(-1.0),
//    _height(-1.0),
//    _name("")
//{}

//Dataspaciment::~Dataspaciment()
//{}
