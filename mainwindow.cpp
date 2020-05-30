#include "mainwindow.h"
#include "ui_mainwindow.h"
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
const int fieldSize = 9;

QMovie * scoreMovie;
QPushButton * field[fieldSize][fieldSize] = {{nullptr}};
QPushButton * next3[3] = {nullptr};
int selectionState = SSNone;
QPoint selected;
int currentScore = 0;
int path[fieldSize][fieldSize] = {{-1}};

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
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

    if (GameState == GSExit) {
        exit(0);
    } else if (GameState == GSStartNewGame) {
        for (int i = 0; i < 3; ++i) {
            QPushButton * btn = new QPushButton();
            btn->setProperty("colorId", qrand()%5);
            btn->setIconSize(QSize(60, 60));
            btn->setIcon(QIcon(":/src/img/ball_"+colors[btn->property("colorId").toInt()]+".png"));
            btn->setStyleSheet("border-radius: 35px;border: 6px solid white;");
            btn->setFixedSize(70,70);
            if (next3[i] != nullptr) delete next3[i];
            next3[i] = btn;
            ui->layNext3->addWidget(btn, 0, i);
        }
        for (int i = 0; i < fieldSize; ++i) {
            for (int j = 0; j < fieldSize; ++j) {
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
    } else if (GameState == GSLoadGame) {
        currentScore = loaded["score"].toInt();
        ui->lblScore->setText(loaded["score"]);
        QString str = loaded["field"];
        int k = 0;
        for (int i = 0; i < fieldSize; ++i) {
            for (int j = 0; j < fieldSize; ++j) {
                int a = str.mid(k,1).toInt();
                QPushButton * btn = new QPushButton();
                btn->setMaximumSize(QSize(100,100));
                btn->setProperty("coords", QPoint(i,j));
                if (a == 9) {
                    btn->setProperty("colorId", -1);
                } else {
                    btn->setProperty("colorId", a);
                    btn->setIcon(QIcon(":/src/img/ball_"+colors[a]+".png"));
                    btn->setIconSize(QSize(sizeOfIcon, sizeOfIcon));
                }
                btn->setStyleSheet("border-radius: 50px;border: 6px solid white;");
                QObject :: connect(btn,SIGNAL(clicked()),this,SLOT(btnGameClicked()));
                if (field[i][j] != nullptr) delete field[i][j];
                field[i][j] = btn;
                ui->layGame->addWidget(btn, i,j);
                ++k;
            }
        }
        str = loaded["next3balls"];
        for (int i = 0; i < 3; ++i) {
            int a = str.mid(i,1).toInt();
            QPushButton * btn = new QPushButton();
            btn->setIcon(QIcon(":/src/img/ball_"+colors[a]+".png"));
            btn->setIconSize(QSize(60,60));
            btn->setObjectName("btnGame");
            btn->setStyleSheet("border-radius: 35px; border: 6px solid white;");
            btn->setFixedSize(70,70);
            if (next3[i] != nullptr) delete next3[i];
            next3[i] = btn;
            ui->layNext3->addWidget(btn,0,i);
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::setBall(QPoint p, int c) {
    QPushButton * btn = field[p.x()][p.y()];
    btn->setProperty("colorId", c);
    QPropertyAnimation * animation = new QPropertyAnimation(btn, "iconSize");
    animation->setDuration(AnimationDuration);
    if (c == -1) {
        animation->setStartValue(QSize(sizeOfIcon, sizeOfIcon));
        animation->setEndValue(QSize(5, 5));
        animation->start();
        while (animation->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();
        btn->setIcon(QIcon(":/src/img/ball_null.png"));
        btn->setIconSize(QSize(sizeOfIcon, sizeOfIcon));
    } else {
        animation->setStartValue(QSize(sizeOfIcon, sizeOfIcon));
        animation->setEndValue(QSize(40, 40));
        animation->start();
        while (animation->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();
        btn->setIcon(QIcon(":/src/img/ball_"+colors[c]+".png"));
        animation->setStartValue(QSize(40, 40));
        animation->setEndValue(QSize(sizeOfIcon, sizeOfIcon));
        animation->start();
        while (animation->state() != QAbstractAnimation::Stopped) QCoreApplication::processEvents();
    }
    delete animation;
}

void MainWindow::setSelectionBorder(QPoint p) {
    field[p.x()][p.y()]->setStyleSheet("border-radius: 50px;border: 6px solid #00FA9A;");
}
void MainWindow::setOriginalBorder(QPoint p) {
    field[p.x()][p.y()]->setStyleSheet("border-radius: 50px;border: 6px solid white;");
}

void MainWindow::btnGameClicked() {
    if (selectionState == SSBlocked) {
        return;
    } else if (selectionState == SSNone) {
        QObject * btn = sender();
        if (btn->property("colorId").toInt() == -1) return;
        selectionState = SSSelected;
        setSelectionBorder(btn->property("coords").toPoint());
        selected = btn->property("coords").toPoint();
    } else if (selectionState == SSSelected) {
        QObject * btn = sender();
        QPoint coords = btn->property("coords").toPoint();
        if (btn->property("colorId").toInt() != -1) {
            setOriginalBorder(selected);
            selected = coords;
            setSelectionBorder(selected);
            return;
        }

        if (MovementType == MTTeleport) {
            selectionState = SSBlocked;
            int selectedColor = field[selected.x()][selected.y()]->property("colorId").toInt();
            setOriginalBorder(selected);
            setBall(selected, -1);
            setBall(coords, selectedColor);
            checkCollapse();
            set3balls();
            checkCollapse();
            checkGameEnd();
            selectionState = SSNone;
        } else if ((MovementType == MTStandard || MovementType == MTHandV) && checkAccess(selected.x(), selected.y(), coords.x(), coords.y())) {
            selectionState = SSBlocked;
            setOriginalBorder(selected);
            int x2 = coords.x(), y2 = coords.y(), x1 = selected.x(), y1 = selected.y();
            int selectedColor = field[selected.x()][selected.y()]->property("colorId").toInt();
            while (true) {
                if (x1+1 < fieldSize && path[x1+1][y1] == path[x1][y1]+1) {
                    x2 = x1+1;
                    y2 = y1;
                } else if (x1-1 >= 0 && path[x1-1][y1] == path[x1][y1]+1) {
                    x2 = x1-1;
                    y2 = y1;
                } else if (y1+1 < fieldSize && path[x1][y1+1] == path[x1][y1]+1) {
                    x2 = x1;
                    y2 = y1+1;
                } else if (y1-1 >= 0 && path [x1][y1-1] == path[x1][y1]+1) {
                    x2 = x1;
                    y2 = y1-1;
                }

                setBall(QPoint(x1, y1), -1);
                setBall(QPoint(x2, y2), selectedColor);

                if (x2 == coords.x() && y2 == coords.y()) {
                    break;
                }
                y1 = y2;
                x1 = x2;
            }
            checkCollapse();
            set3balls();
            checkCollapse();
            checkGameEnd();
            selectionState = SSNone;
        }
    }
}



void MainWindow::set3balls() {
    int k = 0;
    while (k < 3) {
        int x = qrand()%fieldSize;
        int y = qrand()%fieldSize;
        int c = next3[k]->property("colorId").toInt();
        QPushButton * btn = field[x][y];
        int colorId  = btn->property("colorId").toInt();
        if (colorId == -1) {
            setBall(QPoint(x,y), c);
            next3[k]->setProperty("colorId", qrand()%5);
            next3[k]->setIcon(QIcon(":/src/img/ball_"+colors[next3[k]->property("colorId").toInt()]+".png"));
            ++k;
            if (checkGameEnd()) {
                currentScore = ui->lblScore->text().toInt();
                hide();
                EnterName w;
                w.exec();
            }
        }
    }
}

QVector<QVector<int>> MainWindow::fieldToNum() {
    QVector<QVector<int>> res;
    for (int i = 0; i < fieldSize; ++i) {
        QVector<int> v;
        for (int j = 0; j < fieldSize; ++j) {
            v.push_back(ui->layGame->itemAtPosition(i,j)->widget()->property("colorId").toInt());
        }
        res.push_back(v);
    }
    return res;
}


void MainWindow::checkCollapse() {
    int arr[fieldSize][fieldSize];
    int flG = 0;
    QVector<QVector<int>> fieldNum = fieldToNum();
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            arr[i][j] = 0;
        }
    }

    for(int i = 0; i < fieldSize; ++i) {
        int c = fieldNum[i][0];
        int l = 1;
        int fl = 0;
        for(int j = 1; j < fieldSize; ++j) {
            if (fieldNum[i][j] == -1) {
                l = 1;
                fl = 0;
                c = -2;
                continue;
            }
            if (fieldNum[i][j] != c) {
                l = 1;
                c = fieldNum[i][j];
                fl = 0;
            } else {
                ++l;
                if (l == 5 && !fl) {
                    flG = 1;
                    fl = 1;
                    for (int k = j-4; k <= j; ++k) {
                        arr[i][k] = 1;
                    }
                }
                if (fl) {
                    arr[i][j] = 1;
                }
            }
        }
    }


    for(int i = 0; i < fieldSize; ++i) {
        int c = fieldNum[0][i];
        int l = 1;
        int fl = 0;
        for(int j = 1; j < fieldSize; ++j) {
            if (fieldNum[j][i] == -1) {
                l = 1;
                fl = 0;
                c = -2;
                continue;
            }
            if (fieldNum[j][i] != c) {
                l = 1;
                c = fieldNum[j][i];
                fl = 0;
            } else {
                ++l;
                if (l == 5 && !fl) {
                    flG = 1;
                    fl = 1;
                    for (int k = j-4; k <= j; ++k) {
                        arr[k][i] = 1;
                    }
                }
                if (fl) {
                    arr[j][i] = 1;
                }
            }
        }
    }

    bool changes = true;
    while (changes) {
        changes = false;
        for (int i =0; i < fieldSize; ++i) {
            for (int j = 0; j < fieldSize; ++j) {
                if (arr[i][j] == 1) {
                    if (i+1 < fieldSize && arr[i+1][j] == 0 && fieldNum[i+1][j] == fieldNum[i][j]) {
                        arr[i+1][j] = 1;
                        changes = true;
                    }
                    if (i-1 >= 0 && arr[i-1][j] == 0 && fieldNum[i-1][j] == fieldNum[i][j]) {
                        arr[i-1][j] = 1;
                        changes = true;
                    }
                    if (j+1 < fieldSize && arr[i][j+1] == 0 && fieldNum[i][j+1] == fieldNum[i][j]) {
                        arr[i][j+1] = 1;
                        changes = true;
                    }
                    if (j-1 >= 0 && arr[i][j-1] == 0 && fieldNum[i][j-1] == fieldNum[i][j]) {
                        arr[i][j-1] = 1;
                        changes = true;
                    }
                }
            }
        }
    }
    if (flG) {
        collapse(arr);
    }
}




void MainWindow::collapse(int arr[fieldSize][fieldSize]) {
    int k = 0;
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            if (arr[i][j] == 1) {
                setBall(QPoint(i, j), -1);
                ++k;
            }
        }
    }
    currentScore = ui->lblScore->text().toInt();
    currentScore += k*cost;
    if (k > 5) {
        currentScore += ceil(((k-5)*1.0)/2)*cost;
    }
    scoreMovie->start();
    ui->lblScore->setText(QString::number(currentScore));
}



void MainWindow::gameRestart() {
    ui->lblScore->setText("0");
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            field[i][j]->setProperty("colorId", -1);
            field[i][j]->setIcon(QIcon(":/src/img/ball_null.png"));
        }
    }
    set3balls();
}


bool MainWindow::checkGameEnd() {
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            if (field[i][j]->property("colorId").toInt() == -1) {
                return false;
            }
        }
    }
    return true;
}


void MainWindow::closeEvent(QCloseEvent *e) {
    if (GameState != GSExit) {
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
    e->accept();
}


bool MainWindow::checkAccess(int x1, int y1, int x2, int y2) {
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            path[i][j] = -1;
        }
    }

    if (MovementType == MTHandV && !(x1 == x2 || y1 == y2)) return false;

    QVector<QVector<int>> fieldNum = fieldToNum();
    int arr[fieldSize][fieldSize];
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            if (fieldNum[i][j] != -1) {
                arr[i][j] = -1;
            } else {
                arr[i][j] = -2;
            }
        }
    }
    arr[x1][y1] = 0;
    bool changes = true;
    while (changes) {
        changes = false;
        for (int i = 0; i < fieldSize; ++i) {
            for (int j = 0; j < fieldSize; ++j) {
                if (arr[i][j] >= 0) {
                    if (i+1 < fieldSize && arr[i+1][j] == -2) {
                        arr[i+1][j] = arr[i][j] + 1;
                        changes = true;
                    }
                    if (i-1 >= 0 && arr[i-1][j] == -2) {
                        arr[i-1][j] = arr[i][j] + 1;
                        changes = true;
                    }
                    if (j+1 < fieldSize && arr[i][j+1] == -2) {
                        arr[i][j+1] = arr[i][j] + 1;
                        changes = true;
                    }
                    if (j-1 >= 0 && arr[i][j-1] == -2) {
                        arr[i][j-1] = arr[i][j] + 1;
                        changes = true;
                    }
                }
            }
        }
    }

    if (arr[x2][y2] < 0) return false;

    path[x2][y2] = arr[x2][y2];
    while (x2 != x1 || y2 != y1) {
        if (x2+1 < fieldSize && arr[x2+1][y2] == arr[x2][y2]-1) {
            ++x2;
        } else if (x2-1 >= 0 && arr[x2-1][y2] == arr[x2][y2]-1) {
            --x2;
        } else if (y2+1 < fieldSize && arr[x2][y2+1] == arr[x2][y2]-1) {
            ++y2;
        } else if (y2-1 >= 0 && arr[x2][y2-1] == arr[x2][y2]-1) {
            --y2;
        }
        path[x2][y2] = arr[x2][y2];
    }
    return true;
}



void MainWindow::on_btnSaveGame_clicked() {
    QString dict[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
    QString key = dict[qrand()%25] + dict[(qrand()%25)] + dict[(qrand()%25)];
    QFile file("savedata.json");
    file.open(QFile::ReadWrite);
    QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
    while (obj.value(key) != QJsonValue::Undefined) key = dict[qrand()%25] + dict[(qrand()%25)] + dict[(qrand()%25)];

    QString f = "";
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            QString s1 = field[i][j]->property("colorId").toString();
            if (s1 == "-1"){
                s1 = "9";
            }
            f += s1;
        }
    }

    QString n3s = "";
    for (int i = 0; i < 3; ++i) {
        n3s += next3[i]->property("colorId").toString();
    }

    QJsonObject g;
    g["score"] = ui->lblScore->text();
    g["field"] = f;
    g["n3"] = n3s;
    g["MT"] = MovementType;
    obj[key] = g;
    file.reset();
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    file.close();
    QMessageBox::about(this,"Your game key", "Your game key is\n" + key + "\nSave it to recover the game.");
    qApp->quit();
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
}
