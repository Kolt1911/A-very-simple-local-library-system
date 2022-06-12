#ifndef PERSONAL_INFO_H
#define PERSONAL_INFO_H

#include <QWidget>
#include<QSqlQuery>

namespace Ui {
class Personal_info;
}

class Personal_info : public QWidget
{
    Q_OBJECT

public:
    explicit Personal_info(QWidget *parent = nullptr);
    ~Personal_info();

public slots:
    void getdata(QSqlQuery l);
private slots:
    void change_pawd();
private:
    Ui::Personal_info *ui;

signals:
    void send_close();
};

#endif // PERSONAL_INFO_H
