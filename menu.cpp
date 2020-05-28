#include "menu.h"
#include "ui_menu.h"
#include "mainwindow.h"
#include "scoreboard.h"
#include <QPixmap>
#include <QPalette>
#include <QMovie>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "settings.h"
#include "Enums.h"

int GameState = GSExit;
extern int ScoreboardParent;
extern int MovementType;
QMap <QString, QString> loaded;

Menu::Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
    this->setFixedSize(755, 659);
    GameState = GSExit;

    QMovie *m = new QMovie(":src/img/label_main.gif");
    ui->lblName->setMovie(m);
    m->start();

    QPixmap pix(":src/img/background_menu.png");
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
    GameState = GSStartNewGame;
    this->close();
}

void Menu::on_btnScoreboard_clicked()
{
    ScoreboardParent = SPMenu;
    ScoreBoard w;
    w.exec();
}

void Menu::on_btnQuit_clicked()
{
    GameState = GSExit;
    this->close();
}


void Menu::on_btnLoadGame_clicked()
{
    bool ok;
    QString ans = QInputDialog::getText(this, "Key", "Enter your key: ", QLineEdit::Normal, "", &ok);
    if (ok && !ans.isEmpty()){
        QFile file("savedata.json");
        file.open(QIODevice::ReadOnly);
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
        file.open(QIODevice::WriteOnly);
        if (obj.value(ans) != QJsonValue::Undefined){
            GameState = 3;
            loaded["field"] = obj.value(ans).toObject().value("field").toString();
            loaded["next3balls"] = obj.value(ans).toObject().value("n3").toString();
            loaded["score"] = obj.value(ans).toObject().value("score").toString();
            MovementType = obj.value(ans).toObject().value("MT").toInt();
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
