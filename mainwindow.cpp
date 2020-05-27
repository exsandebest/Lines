#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPushButton"
#include "QTime"
#include "QMessageBox"
#include "menu.h"
#include "scoreboard.h"
#include <math.h>
#include "entername.h"
#include <QCloseEvent>
#include <QProcess>
#include <QFile>
#include <QPixmap>
#include <QPalette>
#include <QMovie>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPropertyAnimation>
#include <QMap>
#include "Enums.h"

extern int GameState;
extern int MovementType;
extern QMap <QString, QString> loaded;


const int AnimationDuration = 100;
const QString colors[] = {"blue","green","red","purple","orange"};
const int cost = 20;
const int sizeOfIcon = 90;

QMovie * scoreMovie;
QPushButton * field[9][9];
QPushButton * next3[3] = {nullptr};
int selectionState = 0;
int currentScore;
int fieldNum[9][9];
int active[3] = {-1};
int path[9][9];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Menu menuWindow;
    menuWindow.setModal(true);
    menuWindow.exec();

    scoreMovie = new QMovie(":src/img/label_score.gif");
    ui->lblScoreTitle->setMovie(scoreMovie);
    scoreMovie->start();
    scoreMovie->stop();

    QPixmap p(":src/img/label_next_3_balls.png");
    p = p.scaled(ui->lblNext3Title->size());
    ui->lblNext3Title->setPixmap(p);

    QPixmap pix(":src/img/background_main.jpg");
    pix = pix.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette pal;
    pal.setBrush(QPalette::Background, pix);
    this->setPalette(pal);

    this->showMaximized();

    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    if (GameState == GSExit){
        exit(0);
    } else if (GameState == GSStartNewGame){
        if (next3[0] == nullptr){
            for (int i = 0; i < 3; ++i){
                QPushButton * btn = new QPushButton();
                btn->setProperty("colorId", qrand()%5);
                btn->setIconSize(QSize(60, 60));
                btn->setIcon(QIcon(":/src/img/ball_"+colors[btn->property("colorId").toInt()]+".png"));
                btn->setStyleSheet("border-radius: 35px;border: 6px solid white;");
                btn->setFixedSize(70,70);
                next3[i] = btn;
                ui->layNext3->addWidget(btn, 0, i);
            }
        } else {
            for (int i = 0; i < 3; ++i){
                next3[i]->setProperty("colorId", qrand()%5);
                next3[i]->setIcon(QIcon(":/src/img/ball_"+colors[next3[i]->property("colorId").toInt()]+".png"));
            }
        }
        for (int i = 0; i < 9; ++i){
            for (int j = 0; j < 9; ++j){
                QPushButton * btn = new QPushButton();
                btn->setFlat(true);
                btn->setMaximumSize(QSize(100,100));
                btn->setIconSize(QSize(sizeOfIcon, sizeOfIcon));
                btn->setProperty("coords", QPoint(i,j));
                btn->setProperty("colorId",-1);
                btn->setObjectName("btnGame");
                QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
                ui->layGame->addWidget(btn, i,j);
                field[i][j] = btn;
                setOriginalBorder(QPoint(i,j));
            }
        }
        ui->lblScore->setText("0");
        set3balls();
    } else if (GameState == GSLoadGame){
        currentScore = loaded["score"].toInt();
        ui->lblScore->setText(loaded["score"]);
        QString str = loaded["field"];
        int k = 0;
        for (int i = 0; i < 9; ++i){
            for (int j = 0; j < 9; ++j){
                int a = str.mid(k,1).toInt();
                QPushButton * btn = new QPushButton();
                btn->setMaximumSize(QSize(100,100));
                btn->setProperty("coords", QPoint(i,j));
                if (a == 9){
                    btn->setProperty("colorId",-1);
                } else {
                    btn->setProperty("colorId",a);
                    btn->setIcon(QIcon(":/src/img/ball_"+colors[a]+".png"));
                    btn->setIconSize(QSize(90,90));
                }
                btn->setObjectName("btnGame");
                btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
                field[i][j] = btn;
                ui->layGame->addWidget(btn, i,j);
                ++k;
            }
        }
        str = loaded["next3balls"];
        for (int i = 0; i < 3; ++i){
            int a = str.mid(i,1).toInt();
            QPushButton * btn = new QPushButton();
            btn->setIcon(QIcon(":/src/img/ball_"+colors[a]+".png"));
            btn->setIconSize(QSize(60,60));
            btn->setObjectName("btnGame");
            btn->setStyleSheet("border-radius: 35px; border: 6px solid white;");
            btn->setFixedSize(70,70);
            ui->layNext3->addWidget(btn,0,i);
        }
    }
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::setBall(QPoint p, int c){
    field[p.x()][p.y()]->setProperty("colorId", c);
    if (c == -1){
        field[p.x()][p.y()]->setIcon(QIcon(":/src/img/ball_null.png"));
    } else {
        field[p.x()][p.y()]->setIcon(QIcon(":/src/img/ball_"+colors[c]+".png"));
    }
}
void MainWindow::setSelectionBorder(QPoint p){
    field[p.x()][p.y()]->setStyleSheet("border-radius: 50px;border: 6px solid #00FA9A;");
}
void MainWindow::setOriginalBorder(QPoint p){
    field[p.x()][p.y()]->setStyleSheet("border-radius: 50px;border: 6px solid white;");
}


void MainWindow::btnGameClicked(){
    if (active[0] != -1){
        QObject * btn = sender();
        int colorId = btn->property("colorId").toInt();
        QPoint coords = btn->property("coords").toPoint();
        int x2 = coords.x(), y2 = coords.y();
        int x1 = active[0], y1 = active[1];
        if(colorId == -1 && checkAccess(x1,y1,x2,y2)){
            if (MovementType != MTTeleport){
                while (1){
                    if (x1+1<9 && path[x1+1][y1] == path[x1][y1]+1){
                        x2 = x1+1;
                        y2 = y1;
                    } else if (x1-1>=0 && path[x1-1][y1] == path[x1][y1]+1){
                        x2 = x1-1;
                        y2 = y1;
                    } else if (y1+1<9 && path[x1][y1+1] == path[x1][y1]+1){
                        x2 = x1;
                        y2 = y1+1;
                    } else if (y1 - 1 >=0 && path [x1][y1-1] == path[x1][y1]+1){
                        x2 = x1;
                        y2 = y1-1;
                    }
                    QPropertyAnimation * animation = new QPropertyAnimation(ui->layGame->itemAtPosition(x1,y1)->widget(),"iconSize");
                    animation->setDuration(AnimationDuration);
                    animation->setStartValue(QSize(90,90));
                    animation->setEndValue(QSize(0,0));
                    animation->start();
                    while (animation->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();

                    delete ui->layGame->itemAtPosition(x1,y1)->widget();
                    QPushButton * btn2 = new QPushButton();
                    btn2->setProperty("coords", QPoint(x1, y1));
                    btn2->setProperty("colorId",-1);
                    btn2->setFixedSize(100,100);
                    btn2->setObjectName("btnGame");
                    btn2->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                    QObject :: connect(btn2,SIGNAL(clicked()),this,SLOT(btnGameClicked()));

                    ui->layGame->addWidget(btn2,x1,y1);

                    delete ui->layGame->itemAtPosition(x2,y2)->widget();
                    QPushButton * btn = new QPushButton();
                    btn->setIcon(QIcon(":/src/img/ball_"+colors[active[2]]+".png"));
                    btn->setIconSize(QSize(90,90));
                    btn->setProperty("coords", QPoint(x2,y2));
                    btn->setProperty("colorId",active[2]);
                    btn->setFixedSize(100,100);
                    btn->setObjectName("btnGame");
                    btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                    QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));

                    QPropertyAnimation * animation2 = new QPropertyAnimation(btn,"iconSize");
                    animation2->setDuration(AnimationDuration);
                    animation2->setStartValue(QSize(0,0));
                    animation2->setEndValue(QSize(90,90));
                    ui->layGame->addWidget(btn,x2,y2);
                    animation2->start();
                    while (animation2->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();

                    if (x2 == coords.x() && y2 == coords.y()){
                        break;
                    }
                    y1 = y2;
                    x1 = x2;
                }
            } else {
                QPropertyAnimation * animation = new QPropertyAnimation(ui->layGame->itemAtPosition(x1,y1)->widget(),"iconSize");
                animation->setDuration(AnimationDuration);
                animation->setStartValue(QSize(90,90));
                animation->setEndValue(QSize(0,0));
                animation->start();
                while (animation->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();

                delete ui->layGame->itemAtPosition(x1,y1)->widget();
                QPushButton * btn2 = new QPushButton();
                btn2->setProperty("coords", QPoint(x1, y1));
                btn2->setProperty("colorId",-1);
                btn2->setFixedSize(100,100);
                btn2->setObjectName("btnGame");
                btn2->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                QObject :: connect(btn2,SIGNAL(clicked()),this,SLOT(btnGameClicked()));

                ui->layGame->addWidget(btn2,x1,y1);

                delete ui->layGame->itemAtPosition(x2,y2)->widget();
                QPushButton * btn = new QPushButton();
                btn->setIcon(QIcon(":/src/img/ball_"+colors[active[2]]+".png"));
                btn->setIconSize(QSize(90,90));
                btn->setProperty("coords", QPoint(x2, y2));
                btn->setProperty("colorId",active[2]);
                btn->setFixedSize(100,100);
                btn->setObjectName("btnGame");
                btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));

                QPropertyAnimation * animation2 = new QPropertyAnimation(btn,"iconSize");
                animation2->setDuration(AnimationDuration);
                animation2->setStartValue(QSize(0,0));
                animation2->setEndValue(QSize(90,90));
                ui->layGame->addWidget(btn,x2,y2);

                animation2->start();
                while (animation2->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();
            }
            for (int i = 0; i < 9; ++i){
                for (int j = 0; j < 9; ++j){
                    path[i][j] = 0;
                }
            }
            for (int i =0; i < 3; ++i){
                active[i] = -1;
            }
            checkCollapse(0);


        } else {
            delete ui->layGame->itemAtPosition(active[0],active[1])->widget();
            QPushButton * btn3 = new QPushButton();
            btn3->setProperty("coords", QPoint(active[0], active[1]));
            btn3->setProperty("colorId", active[2]);
            btn3->setFixedSize(100,100);
            btn3->setObjectName("btnGame");
            btn3->setIcon(QIcon(":/src/img/ball_"+colors[active[2]]+".png"));
            btn3->setIconSize(QSize(90,90));
            btn3->setStyleSheet("border-radius: 50px;border: 6px solid white;");
            QObject :: connect(btn3,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
            ui->layGame->addWidget(btn3,active[0],active[1]);
            for (int i =0; i < 3; ++i){
                active[i] = -1;
            }
        }
    } else {
        QObject * btn = sender();
        QPoint coords = btn->property("coords").toPoint();
        if (btn->property("colorId").toInt() != -1){
            active[0] = coords.x();
            active[1] = coords.y();
            active[2] = btn->property("colorId").toInt();
            delete ui->layGame->itemAtPosition(active[0],active[1])->widget();
            QPushButton * btn2 = new QPushButton();
            btn2->setProperty("coords", coords);
            btn2->setProperty("colorId",active[2]);
            btn2->setFixedSize(100,100);
            btn2->setObjectName("btnGame");
            btn2->setIcon(QIcon(":/src/img/ball_"+colors[active[2]]+".png"));
            btn2->setIconSize(QSize(90,90));
            btn2->setStyleSheet("border-radius: 50px;border: 6px solid 	#00FA9A;");
            QObject :: connect(btn2,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
            ui->layGame->addWidget(btn2,active[0],active[1]);
        }
    }
}



void MainWindow::set3balls(){
    int k = 0;
    while (k<3){
        int x = qrand()%9;
        int y = qrand()%9;
        int c = next3[k]->property("colorId").toInt();
        QPushButton  * btn = field[x][y];
        int colorId  = btn->property("colorId").toInt();
        if (colorId == -1){
            btn->setProperty("colorId", c);
            btn->setIcon(QIcon(":/src/img/ball_"+colors[c]+".png"));
            next3[k]->setProperty("colorId", qrand()%5);
            next3[k]->setIcon(QIcon(":/src/img/ball_"+colors[next3[k]->property("colorId").toInt()]+".png"));
            ++k;
            if (checkGameEnd()){
                currentScore = ui->lblScore->text().toInt();
                hide();
                EnterName w;
                w.exec();
            }
        }
    }
    updateField();
}


void MainWindow::checkCollapse(int end){
    updateField();
    int arr[9][9];
    int flG = 0;

    for (int i = 0; i < 9; ++i){
        for (int j = 0; j < 9; ++j){
            arr[i][j] = 0;
        }
    }

    for(int i = 0; i < 9; ++i){
        int c = fieldNum[i][0];
        int l = 1;
        int fl = 0;
        for(int j = 1; j < 9; ++j){
            if (fieldNum[i][j] == -1){
                l = 1;
                fl = 0;
                c = -2;
                continue;
            }
            if (fieldNum[i][j] != c){
                l = 1;
                c = fieldNum[i][j];
                fl = 0;
            } else {
                ++l;
                if (l == 5 && !fl){
                    flG = 1;
                    fl = 1;
                    for (int k = j-4; k<=j; ++k){
                        arr[i][k] = 1;
                    }
                }
                if (fl){
                    arr[i][j] = 1;
                }
            }
        }
    }


    for(int i = 0; i < 9; ++i){
        int c = fieldNum[0][i];
        int l = 1;
        int fl = 0;
        for(int j = 1; j < 9; ++j){
            if (fieldNum[j][i] == -1){
                l = 1;
                fl = 0;
                c = -2;
                continue;
            }
            if (fieldNum[j][i] != c){
                l = 1;
                c = fieldNum[j][i];
                fl = 0;
            } else {
                ++l;
                if (l == 5 && !fl){
                    flG = 1;
                    fl = 1;
                    for (int k = j-4; k<=j; ++k){
                        arr[k][i] = 1;
                    }
                }
                if (fl){
                    arr[j][i] = 1;
                }
            }
        }
    }

    bool changes = 1;
    while (changes){
        changes = 0;
        for (int i =0; i < 9; ++i){
            for (int j = 0; j <9; ++j){
                if (arr[i][j] == 1){
                    if (i+1<9 && arr[i+1][j] == 0 && fieldNum[i+1][j] == fieldNum[i][j]){
                        arr[i+1][j] = 1;
                        changes = 1;
                    }
                    if (i-1>=0 && arr[i-1][j] == 0 && fieldNum[i-1][j] == fieldNum[i][j]){
                        arr[i-1][j] = 1;
                        changes = 1;
                    }
                    if (j+1<9 && arr[i][j+1] == 0 && fieldNum[i][j+1] == fieldNum[i][j]){
                        arr[i][j+1] = 1;
                        changes =1;
                    }
                    if (j-1>=0 && arr[i][j-1] == 0 && fieldNum[i][j-1] == fieldNum[i][j]){
                        arr[i][j-1] = 1;
                        changes =1;
                    }
                }
            }
        }
    }

    if (flG){
        collapse(arr);
    } else {
        if (!end){
            set3balls();
            checkCollapse(1);
        }
    }
}




void MainWindow::collapse(int arr[9][9]){
    int ans = 0;

    int k =0;
    for (int i = 0; i<9;++i){
        for (int j = 0; j <9; ++j){
            if (arr[i][j] == 1){
                delete ui->layGame->itemAtPosition(i,j)->widget();
                QPushButton * btn = new QPushButton();
                btn->setFixedSize(100,100);
                btn->setProperty("coords", QPoint(i, j));
                btn->setProperty("colorId",-1);
                btn->setObjectName("btnGame");
                btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
                ui->layGame->addWidget(btn, i,j);
                ++k;
            }
        }
    }
    ans = k*cost;
    if (k>5){
        ans+=ceil(((k-5)*1.0)/2)*cost;
    }
    updateField();
    ans+=ui->lblScore->text().toInt();
    currentScore = ans;
    scoreMovie->start();
    ui->lblScore->setText(QString::number(ans));
}




void MainWindow::updateField(){
    for (int i = 0; i < 9; ++i){
        for (int j = 0; j < 9; ++j){
            fieldNum[i][j] = ui->layGame->itemAtPosition(i,j)->widget()->property("colorId").toInt();
        }
    }
}



void MainWindow::gameRestart(){
    ui->lblScore->setText("0");

    for (int i = 0; i < 3; ++i){
        active[i] = -1;
    }
    for (int i = 0; i < 9; ++i){
        for (int j = 0; j < 9; ++j){
            fieldNum[i][j] =-1;
            delete ui->layGame->itemAtPosition(i,j)->widget();
            QPushButton * btn = new QPushButton();
            btn->setFixedSize(100,100);
            btn->setProperty("coords", QPoint(i, j));
            btn->setProperty("colorId",-1);
            btn->setObjectName("btnGame");
            btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
            QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
            ui->layGame->addWidget(btn, i,j);
        }
    }
    set3balls();
}


bool MainWindow::checkGameEnd(){
    for (int i =0; i <9; ++i){
        for (int j = 0; j<9; ++j){
            if (ui->layGame->itemAtPosition(i,j)->widget()->property("colorId").toInt() == -1){
                return 0;
            }
        }
    }
    return 1;
}


void MainWindow::closeEvent(QCloseEvent *e){
    if (GameState != GSExit){
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
    e->accept();
}


bool MainWindow::checkAccess(int x1, int y1, int x2, int y2){
    if (MovementType == MTTeleport){
        return true;
    } else if (MovementType == MTStandard) {
        updateField();
        int arr[9][9];
        for (int i = 0; i < 9; ++i){
            for (int j = 0; j < 9; ++j){
                if (fieldNum[i][j] != -1){
                    arr[i][j] = -1;
                } else {
                    arr[i][j] = -2;
                }
            }
        }
        arr[x1][y1] = 0;
        int changes = 1;
        while (changes){
            changes = 0;
            for (int i = 0; i < 9; ++i){
                for (int j = 0; j < 9; ++j){
                    if (arr[i][j]>=0){
                        if (i+1<9 && arr[i+1][j] == -2){
                            arr[i+1][j] = arr[i][j]+1;
                            changes = 1;
                        }
                        if (i-1>=0 && arr[i-1][j] == -2){
                            arr[i-1][j] = arr[i][j] + 1;
                            changes =1;
                        }
                        if (j+1 < 9 && arr[i][j+1] == -2){
                            arr[i][j+1] = arr[i][j] + 1;
                            changes = 1;
                        }
                        if (j-1>=0 && arr[i][j-1] == -2){
                            arr[i][j-1] = arr[i][j]+1;
                            changes = 1;
                        }
                    }
                }
            }
        }

        if (arr[x2][y2] > 0){
            path[x2][y2] = arr[x2][y2];
            while (x2 != x1 || y2 != y1){
                if (x2+1<9 && arr[x2+1][y2] == arr[x2][y2]-1){
                    x2+=1;
                    path[x2][y2] = arr[x2][y2];
                } else if (x2-1>=0 && arr[x2-1][y2] == arr[x2][y2]-1){
                    x2-=1;
                    path[x2][y2] = arr[x2][y2];
                } else if (y2+1 < 9 && arr[x2][y2+1] == arr[x2][y2]-1){
                    y2+=1;
                    path[x2][y2] = arr[x2][y2];
                } else if (y2-1>=0 && arr[x2][y2-1] == arr[x2][y2]-1){
                    y2-=1;
                    path[x2][y2] = arr[x2][y2];
                }
            }
            return true;
        } else {
            return false;
        }

    } else if (MovementType == MTHandV){
        updateField();
        int arr[9][9];
        for (int i = 0; i < 9; ++i){
            for (int j = 0; j < 9; ++j){
                arr[i][j] = 0;
            }
        }
        for (int i = x1+1; i <9; ++i){
            if (fieldNum[i][y1] == -1){
                arr[i][y1] = 1;
            } else {
                break;
            }
        }
        for (int i = x1-1; i>=0; --i){
            if (fieldNum[i][y1] == -1){
                arr[i][y1] = 1;
            } else {
                break;
            }
        }
        for (int j = y1+1; j<9; ++j){
            if (fieldNum[x1][j] == -1){
                arr[x1][j] = 1;
            } else {
                break;
            }
        }
        for (int j = y1-1; j >=0; --j){
            if (fieldNum[x1][j] == -1){
                arr[x1][j] = 1;
            } else {
                break;
            }
        }

        if (arr[x2][y2] == 1){
            int k = 1;
            if (y2>y1){
                for (int i = y1+1; i <= y2; ++i){
                    path[x1][i] = k++;
                }
            } else if (y2 < y1){
                for (int i = y1-1; i>= y2; --i){
                    path[x1][i] = k++;
                }

            } else if (x2 > x1){
                for (int i = x1+1; i<= x2; ++i){
                    path[i][y1] = k++;
                }
            } else if (x2 < x1){
                for (int i = x1-1; i>=x2; -- i){
                    path[i][y1] = k++;
                }
            }
            return true;
        } else {
            return false;
        }
    }
    return true;
}







void MainWindow::on_btnSaveGame_clicked() {
    QString mas[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
    QString key = mas[qrand()%25] + mas[(qrand()%25)] + mas[(qrand()%25)];
    QFile file("savedata.json");
    file.open(QFile::ReadWrite);
    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    while (obj.value(key) != QJsonValue::Undefined){
        key = mas[qrand()%25] + mas[(qrand()%25)] + mas[(qrand()%25)];
    }

    QString f = "";
    updateField();
    for (int i = 0; i < 9; ++i){
        for (int j = 0; j < 9; ++j){
            QString s1;
            s1.setNum(fieldNum[i][j]);
            if (fieldNum[i][j] == -1){
                s1 = "9";
            }
            f+=s1;
        }
    }
    QString n3s = "";
    for (int i = 0; i < 3; ++i){
        n3s += next3[i]->property("colorId").toString();
    }

    QJsonObject g;
    g["score"] = ui->lblScore->text();
    g["field"] = f;
    g["n3"] = n3s;
    obj[key] = g;
    file.reset();
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    QMessageBox::about(this,"Your game key", "Your game key is\n"+key+"\nSave it to recover the game.");
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}
