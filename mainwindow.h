#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void set3balls();
    bool checkGameEnd();
    void updateField();
    void collapse(int arr[9][9]);
    void checkCollapse(int end);
    void gameRestart();
    bool checkAccess(int x1, int y1, int x2, int y2);
    void setBall(QPoint, int);
    void setSelectionBorder(QPoint);
    void setOriginalBorder(QPoint);

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void btnGameClicked();
    void on_btnSaveGame_clicked();
};

#endif // MAINWINDOW_H
