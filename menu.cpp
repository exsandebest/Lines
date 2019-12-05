#include "menu.h"
#include "ui_menu.h"
#include "mainwindow.h"
#include "scoreboard.h"
#include <QApplication>
#include <QPixmap>
#include <QPalette>
#include <QMovie>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMap>
#include "settings.h"

int stGameGlob = 2;
extern int flagFrom;
 QString loadField;
 QString loadn3;
 QString loadScore;
Menu::Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
    stGameGlob = 2;
    QMovie *m = new QMovie(":src/img/title.gif");
    if (!m->isValid()){
        qDebug() << "!m->isValid() (title.gif)";
    }
    ui->lblName->setMovie(m);
    m->start();
    QPixmap pix(":src/img/mainBg.png");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);
}

Menu::~Menu()
{
    delete ui;
}


void Menu::on_btnStartGame_clicked()
{
    stGameGlob = 1;
    this->close();
}

void Menu::on_btnScoreboard_clicked()
{
    flagFrom = 0;
    ScoreBoard w;
    w.exec();
}

void Menu::on_btnQuit_clicked()
{
    stGameGlob = 2;
    this->close();
}


void Menu::on_btnLoadGame_clicked()
{
    bool ok;
    QString ans = QInputDialog::getText(this,"Key","Enter your key: ",QLineEdit::Normal,"",&ok);
    if (ok && !ans.isEmpty()){
        QFile file("savedata.json");
        file.open(QIODevice::ReadOnly);
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
        file.open(QIODevice::WriteOnly);
        if (obj.value(ans) != QJsonValue::Undefined){
            stGameGlob = 3;
            loadField = obj.value(ans).toObject().value("field").toString();
            loadn3 =obj.value(ans).toObject().value("n3").toString();
            loadScore = obj.value(ans).toObject().value("score").toString();
            obj.remove(ans);
            file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
            file.close();
            this->close();
        } else {
            QMessageBox::about(this, "Incorrect", "Wrong key\n ");
        }
    }
}
void Menu::closeEvent(QCloseEvent *e){
    e->accept();
}

void Menu::on_btnSettings_clicked()
{
    Settings w;
    w.exec();
}
