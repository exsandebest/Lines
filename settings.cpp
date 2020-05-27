#include "settings.h"
#include "ui_settings.h"
#include <QPixmap>
#include <QPalette>
#include "Enums.h"

int MovementType = MTStandard;

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setFixedSize(585,575);
    QPixmap t(":src/img/label_settings.png");
    t = t.scaled(ui->lblTitle->size());
    ui->lblTitle->setPixmap(t);

    QPixmap pix(":src/img/background_settings.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);
    if (MovementType == MTTeleport){
        ui->rbtnTeleport->setChecked(true);
    } else if (MovementType == MTStandard){
        ui->rbtnStandard->setChecked(true);
    } else if (MovementType == MTHandV){
        ui->rbtnHV->setChecked(true);
    }
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_btnSaveSettings_clicked()
{
    if (ui->rbtnTeleport->isChecked()) {
        MovementType = MTTeleport;
    } else if (ui->rbtnStandard->isChecked()){
        MovementType = MTStandard;
    } else if (ui->rbtnHV->isChecked()){
        MovementType = MTHandV;
    }
    this->close();
}
