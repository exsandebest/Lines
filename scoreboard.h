#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QDialog>

namespace Ui {
class ScoreBoard;
}

class ScoreBoard : public QDialog {
    Q_OBJECT

public:
    explicit ScoreBoard(QWidget * parent = nullptr);
    ~ScoreBoard();
    QJsonArray sort(QJsonArray arr);

private slots:
    void on_btnOk_clicked();

protected:
    void closeEvent(QCloseEvent * e);

private:
    Ui::ScoreBoard * ui;
};

#endif // SCOREBOARD_H
