#ifndef ENTERNAME_H
#define ENTERNAME_H

#include <QDialog>

namespace Ui {
class EnterName;
}

class EnterName : public QDialog
{
    Q_OBJECT

public:
    explicit EnterName(QWidget *parent = nullptr);
    ~EnterName();

private slots:
    void on_btnSaveName_clicked();

    void on_btnDontSave_clicked();
protected:
    void closeEvent(QCloseEvent * e);
private:
    Ui::EnterName *ui;
};

#endif // ENTERNAME_H
