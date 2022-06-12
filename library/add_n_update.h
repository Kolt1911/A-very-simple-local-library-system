#ifndef ADD_N_UPDATE_H
#define ADD_N_UPDATE_H

#include <QDialog>

namespace Ui {
class Add_N_Update;
}

class Add_N_Update : public QDialog
{
    Q_OBJECT

public:
    explicit Add_N_Update(QWidget *parent = nullptr);
    ~Add_N_Update();

    void update_book();
    void add_book();
private slots:

    void bnum_enable(bool check);
    void bkind_enable(bool check);
    void bname_enable(bool check);
    void bauthor_enable(bool check);
    void bpublish_enable(bool check);
    void do_add();
    void do_update();
private:
    Ui::Add_N_Update *ui;
    QString bookinfo;

signals:
    void send_refresh();
};

#endif // ADD_N_UPDATE_H
