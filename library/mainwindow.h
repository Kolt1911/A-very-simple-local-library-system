#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QSqlQuery>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void is_admin();

public slots:
    void userinfo(QSqlQuery i);
    void can_b_r_books(QString bnum);
    void refresh();
private slots:
    void win_close();

    void all_books();
    void on_tabWidget_tabCloseRequested(int index);
    void on_tabWidget_currentChanged(int index);
    void logout();
    void receive_close();
    void showuser();
    void search_win();
    void find_author();
    void directly_borrow();
    void directly_return();

    void admin_delete();
    void admin_add();
    void admin_update();
    void showalluser();
    void admin_user_delete();
    void admin_user_find();
private:
    Ui::MainWindow *ui;
    QSqlQuery u;//用户信息
    QString book_num;//选中的图书书号
};

#endif // MAINWINDOW_H
