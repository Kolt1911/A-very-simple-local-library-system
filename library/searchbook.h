#ifndef SEARCHBOOK_H
#define SEARCHBOOK_H

#include <QDialog>
#include<QSqlQuery>
#include<mainwindow.h>

namespace Ui {
class Searchbook;
}

class Searchbook : public QDialog
{
    Q_OBJECT

public:
    explicit Searchbook(QWidget *parent = nullptr);
    ~Searchbook();

    void getdata(QSqlQuery l);



    void book_delete();
    void search_initialize();
private slots:
    void valueChanged(int current_index);
    void search();
    void delete_operation();
private:
    Ui::Searchbook *ui;
    QSqlQuery u;//用户信息

signals:
    void send_update_userinfo(QSqlQuery l);
};

#endif // SEARCHBOOK_H
