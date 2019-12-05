#include "scoreboard.h"
#include "ui_scoreboard.h"
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QMap>
#include <QJsonDocument>
#include <QApplication>
#include <QProcess>
#include <QCloseEvent>
#include <QPixmap>
#include <QPalette>
extern QString nameG;
extern int currentScore;
extern int flagFrom;
extern int nameGExists;

ScoreBoard::ScoreBoard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScoreBoard)
{
    ui->setupUi(this);
    QPixmap pix(":src/img/bgSc3.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background,pix);
    this->setPalette(pal);

    QFile file("scoreboard.json");
    file.open(QIODevice::ReadWrite);

    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    QJsonValue jv = obj.value("mainArray");
    if(jv.isArray()){
        QJsonArray ja = jv.toArray();
        if (flagFrom){
            if (nameGExists){
        QJsonObject objNew;
        objNew["name"] = nameG;
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
            ui->lblList->setText(ui->lblList->text() + sArr.at(i).toObject().value("name").toString()+": "+sArr.at(i).toObject().value("score").toString()+"\n");
            // ui->lblList_2->setText(ui->lblList->text() + sArr.at(i).toObject().value("name").toString()+": "+sArr.at(i).toObject().value("score").toString()+"\n");
        }

        QJsonObject objEnd;
        objEnd["mainArray"] = sArr;
        file.reset();
        file.write(QJsonDocument(objEnd).toJson(QJsonDocument::Indented));
        file.close();
    }


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
        map[obj.value("score").toString().toInt()] = obj.value("name").toString();
        scores[i] = obj.value("score").toString().toInt();
    }
    std::sort(scores, scores+len);
    QJsonArray ans;
    QJsonObject obj;
    QString s;
    for (int i = len-1; i >=0; --i){
        s.setNum(scores[i]);
        obj["score"] = s;
        obj["name"] = map[scores[i]];
        ans.append(obj);
    }
    return ans;
}

void ScoreBoard::on_btnOk_clicked()
{
   this->close();
}
void ScoreBoard::closeEvent(QCloseEvent * e){
    if (flagFrom){
        flagFrom = 0;
        e->ignore();
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    } else {
    e->accept();
    }
}
