#ifndef MENU_H
#define MENU_H

#include <QDialog>

namespace Ui {
class Menu;
}

class Menu : public QDialog
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

private slots:

    void on_btnStartGame_clicked();

    void on_btnScoreboard_clicked();

    void on_btnQuit_clicked();

    void on_btnLoadGame_clicked();
    void on_btnSettings_clicked();

protected:
    void closeEvent(QCloseEvent * e);
private:
    Ui::Menu *ui;
};

#endif // MENU_H
