#ifndef START_N_LOGIN_WINDOW_H
#define START_N_LOGIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Start_n_login_window; }
QT_END_NAMESPACE

class Start_n_login_window : public QMainWindow
{
    Q_OBJECT

public:
    Start_n_login_window(QWidget *parent = nullptr);
    ~Start_n_login_window();




private slots:
    bool on_selectuser_clicked();
    void login();
    void U_signup();
    void showpawd();
    void win_close();
    void hidepawd();
private:
    Ui::Start_n_login_window *ui;
};
#endif // START_N_LOGIN_WINDOW_H
