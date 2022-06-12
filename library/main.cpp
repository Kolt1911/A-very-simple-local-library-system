#include "start_n_login_window.h"

#include <QApplication>
#include<database.h>
#include<QSplashScreen>
#include<windows.h>


Database d;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/images/start");
    QSplashScreen screen(pixmap);//初始化界面

    screen.show();
    screen.setFont(QFont("microsoft yahei", 12, QFont::Bold));
    screen.showMessage("连接数据库...", Qt::AlignLeft|Qt::AlignBottom, Qt::red);
    //界面信息提示


    //Database d;
    d.connect();

    if(d.isConnect())
    {
        Sleep(1000);
        screen.showMessage("连接数据库成功", Qt::AlignLeft|Qt::AlignBottom, Qt::red);
        Sleep(1000);
        //screen.setFont(QFont("microsoft yahei", 20, QFont::Bold));
        a.processEvents();//让界面显示同时也能处理其他事件
        Start_n_login_window w;
        w.show();
        screen.finish(&w);//打开登陆界面就关闭start界面
        return a.exec();
    }
    else//连接失败
    {
        //screen.setFont(QFont("microsoft yahei", 20, QFont::Bold));
        screen.showMessage(QCoreApplication::applicationDirPath(), Qt::AlignLeft|Qt::AlignBottom, Qt::red);
        Sleep(4000);
        screen.showMessage(QString("%1\n%2\n%3").arg("连接数据库失败:").arg(d.error()).arg("检查数据库，退出。"), Qt::AlignLeft|Qt::AlignBottom, Qt::red);
        Sleep(4000);

        a.processEvents();
    }




    return 0;
}
