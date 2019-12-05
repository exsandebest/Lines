#include "entername.h"
#include "ui_entername.h"
#include <QtWidgets>
#include <QMessageBox>
#include "scoreboard.h"
#include <QCloseEvent>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QMovie>
extern int currentScore;
int flagFrom = 0;
QString nameG;
int nameGExists = 0;
EnterName::EnterName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterName)
{
    ui->setupUi(this);
    QPixmap pix(":src/img/bgName.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);
    QMovie * m = new QMovie(":src/img/GO.gif");
    ui->lblGameOver->setMovie(m);
    m->start();
    m->stop();


    QString s1;
    s1.setNum(currentScore);
    ui->lblYourScore->setText("Your score is "+s1);
}

EnterName::~EnterName()
{
    delete ui;
}

void EnterName::on_btnSaveName_clicked()
{
    QString name = ui->lineName->text();
    if (name.replace(QRegularExpression("\n"),"")!= ""){
        nameG = name;
        hide();
        flagFrom = 1;
        nameGExists = 1;
        ScoreBoard win;
        win.setModal(true);
        win.exec();
    } else {
        QMessageBox::about(this, "Incorrect","Enter your name!");
    }
}

void EnterName::on_btnDontSave_clicked()
{
    hide();
    nameGExists = 0;
    flagFrom = 1;
    ScoreBoard win;
    win.setModal(true);
    win.exec();
}
void EnterName::closeEvent(QCloseEvent *e){
    e->ignore();
    on_btnDontSave_clicked();
}
