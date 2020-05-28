#include "entername.h"
#include "ui_entername.h"
#include <QMessageBox>
#include "scoreboard.h"
#include <QCloseEvent>
#include <QPixmap>
#include <QPalette>
#include <QMovie>
#include <QRegularExpression>
#include <Enums.h>

extern int currentScore;
int ScoreboardParent = SPMenu;
QString newNickname;
bool needToSave = false;

EnterName::EnterName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnterName)
{
    ui->setupUi(this);
    this->setFixedSize(1380, 443);

    QPixmap pix(":src/img/background_entername.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);

    QMovie * m = new QMovie(":src/img/label_game_over.gif");
    ui->lblGameOver->setMovie(m);
    m->start();
    m->stop();

    ui->lblYourScore->setText("Your score is " + QString::number(currentScore));
}

EnterName::~EnterName()
{
    delete ui;
}

void EnterName::on_btnSaveName_clicked()
{
    QString name = ui->lineName->text();
    if (name.replace(QRegularExpression("\n"),"").trimmed() != ""){
        newNickname = name;
        hide();
        ScoreboardParent = SPGame;
        needToSave = true;
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
    needToSave = false;
    ScoreboardParent = SPGame;
    ScoreBoard win;
    win.setModal(true);
    win.exec();
}
void EnterName::closeEvent(QCloseEvent *e){
    e->ignore();
    on_btnDontSave_clicked();
}
