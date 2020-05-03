#include "settings.h"
#include "ui_settings.h"
#include <QPixmap>
#include <QPalette>
int movementG = 1;

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
    if (movementG == 0){
        ui->rbtnTeleport->setChecked(true);
    } else if (movementG == 1){
        ui->rbtnStandart->setChecked(true);
    } else if (movementG == 2){
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
        movementG = 0;
    } else if (ui->rbtnStandart->isChecked()){
        movementG = 1;
    } else if (ui->rbtnHV->isChecked()){
        movementG = 2;
    }
    this->close();
}
