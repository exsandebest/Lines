#include "scoreboard.h"
#include "ui_scoreboard.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMap>
#include <QJsonDocument>
#include <QProcess>
#include <QCloseEvent>
#include <QPixmap>
#include <QPalette>
#include "Enums.h"

extern QString newNickname;
extern int currentScore;
extern int ScoreboardParent;
extern bool needToSave;

ScoreBoard::ScoreBoard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoreBoard)
{
    ui->setupUi(this);
    this->setFixedSize(588, 790);
    QPixmap pix(":src/img/background_scoreboard.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);

    QFile file("scoreboard.json");
    file.open(QIODevice::ReadWrite);
    QJsonArray ja = QJsonDocument::fromJson(file.readAll()).array();
    if (ScoreboardParent == SPGame){
        if (needToSave){
            QJsonObject objNew;
            objNew["nickname"] = newNickname;
            QString s1;
            s1.setNum(currentScore);
            objNew["score"] = s1;
            ja.append(objNew);
        } else {
            QString s1;
            s1.setNum(currentScore);
            ui->lblYourScore->setText("You: "+s1);
        }
    }
    QJsonArray sArr = sort(ja);

    for (int i = 0; i < sArr.count(); ++i){
        ui->lblList->setText(ui->lblList->text() + sArr.at(i).toObject().value("nickname").toString() + ": " + sArr.at(i).toObject().value("score").toString() + "\n");
    }

    file.reset();
    file.write(QJsonDocument(sArr).toJson(QJsonDocument::Indented));
    file.close();
}

ScoreBoard::~ScoreBoard()
{
    delete ui;
}

QJsonArray ScoreBoard::sort(QJsonArray arr){
    int len = arr.count();
    int scores[len];
    QMap <int, QString> map;
    for (int i = 0; i < len; ++i){
        QJsonObject obj = arr.at(i).toObject();
        map[obj.value("score").toString().toInt()] = obj.value("nickname").toString();
        scores[i] = obj.value("score").toString().toInt();
    }
    std::sort(scores, scores+len);
    QJsonArray ans;
    QJsonObject obj;
    QString s;
    for (int i = len-1; i >=0; --i){
        s.setNum(scores[i]);
        obj["score"] = s;
        obj["nickname"] = map[scores[i]];
        ans.append(obj);
    }
    return ans;
}

void ScoreBoard::on_btnOk_clicked()
{
    this->close();
}

void ScoreBoard::closeEvent(QCloseEvent * e){
    if (ScoreboardParent == SPGame){
        ScoreboardParent = SPMenu;
        e->ignore();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    } else {
        e->accept();
    }
}
