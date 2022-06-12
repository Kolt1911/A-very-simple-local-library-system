#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QToolButton>
#include<QMessageBox>
#include<QTabWidget>
#include"tablebookinfo.h"
#include"start_n_login_window.h"
#include"personal_info.h"
#include"searchbook.h"
#include<QInputDialog>
#include <QDebug>
#include<QSqlError>
#include"add_n_update.h"

int is_any_book_open=0;//检查显示图书页面是否开启
int is_any_user_open=0;//检查显示个人信息页面是否开启
int is_any_personinfo_open=0;//检查显示所有用户的页面是否开启
bool is_book_selected=0;//是否有书被选中
bool what_is_admin=0;//是否是管理员


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setVisible(false);//先不可见
    ui->tabWidget->clear();//清除所有页面
    ui->tabWidget->setTabsClosable(true);//Page有关闭按钮，可被关闭

    setAttribute(Qt::WA_DeleteOnClose);//删除释放空间

    setWindowIcon(QIcon(":/images/book"));//设置ico
    ui->exit_action->setIcon(QIcon(":/images/exit"));
    ui->actionlogout->setIcon(QIcon(":/images/logout"));
    ui->actionborrow->setIcon(QIcon(":/images/borrow"));
    ui->actionreturn->setIcon(QIcon(":/images/return"));
    ui->actionsearch->setIcon(QIcon(":/images/search"));
    ui->actionauthor->setIcon(QIcon(":/images/author"));
    ui->actionuser->setIcon(QIcon(":/images/user"));
    ui->actionall->setIcon(QIcon(":/images/all"));
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//设置字符在图标底下

    ui->actionborrow->setEnabled(false);//没有点开全部图书事件部分功能不能使用
    ui->actionreturn->setEnabled(false);
    ui->actionsearch->setEnabled(false);
    ui->actionauthor->setEnabled(false);


    connect(ui->exit_action,SIGNAL(triggered()),this,SLOT(win_close()));//信号连接
    connect(ui->actionall,SIGNAL(triggered()),this,SLOT(all_books()));
    connect(ui->actionlogout,SIGNAL(triggered()),this,SLOT(logout()));
    connect(ui->actionuser,SIGNAL(triggered()),this,SLOT(showuser()));
    connect(ui->actionsearch,SIGNAL(triggered()),this,SLOT(search_win()));
    connect(ui->actionauthor,SIGNAL(triggered()),this,SLOT(find_author()));
    connect(ui->actionborrow,SIGNAL(triggered()),this,SLOT(directly_borrow()));
    connect(ui->actionreturn,SIGNAL(triggered()),this,SLOT(directly_return()));
    connect(ui->actionbookdelete,SIGNAL(triggered()),this,SLOT(admin_delete()));
    connect(ui->actionbookadd,SIGNAL(triggered()),this,SLOT(admin_add()));
    connect(ui->actionbookupdate,SIGNAL(triggered()),this,SLOT(admin_update()));
    connect(ui->actionusersfind,SIGNAL(triggered()),this,SLOT(admin_user_find()));
    connect(ui->actionusersdelete,SIGNAL(triggered()),this,SLOT(admin_user_delete()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::userinfo(QSqlQuery i)//用户信息传递
{
    u=i;

    if(this->isVisible())//在主窗口显示后每次更新都要重新打开显示界面
    {
        int book,user,info;//查看是否有图书界面或者个人信息界面
        book=is_any_book_open;
        user=is_any_user_open;
        info=is_any_personinfo_open;

        ui->tabWidget->clear();//清除现有界面
        if(book>0&&user>0&&info>0)//打开过用户和图书页面，则都刷新
        {
            this->all_books();//重新打开显示界面
            this->showuser();//重新打开显示界面
            if(what_is_admin)
                this->showalluser();//重新打开用户列表
        }

        else if(book>0&&user<=0&&info>0)//打开过图书页面和用户列表，则刷新
        {
            this->all_books();//重新打开显示界面
            if(what_is_admin)
                this->showalluser();//重新打开用户列表
        }
        else if(book>0&&user>0&&info<=0)//打开过图书页面和个人信息，则刷新
        {
            this->all_books();//重新打开显示界面
            this->showuser();//重新打开个人信息
        }
        else if(book>0&&user<=0&&info<=0)
        {
            this->all_books();//重新打开显示界面
        }

    }


}

void MainWindow::refresh()//刷新界面
{
    if(this->isVisible())//在主窗口显示后每次更新都要重新打开显示界面
    {
        int book,user,info;//查看是否有图书界面或者个人信息界面
        book=is_any_book_open;
        user=is_any_user_open;
        info=is_any_personinfo_open;

        ui->tabWidget->clear();//清除现有界面
        if(book>0&&user>0&&info>0)//打开过用户和图书页面，则都刷新
        {
            this->all_books();//重新打开显示界面
            this->showuser();//重新打开显示界面
            if(what_is_admin)
                this->showalluser();//重新打开用户列表
        }

        else if(book>0&&user<=0&&info>0)//打开过图书页面和用户列表，则刷新
        {
            this->all_books();//重新打开显示界面
            if(what_is_admin)
                this->showalluser();//重新打开用户列表
        }
        else if(book>0&&user>0&&info<=0)//打开过图书页面和个人信息，则刷新
        {
            this->all_books();//重新打开显示界面
            this->showuser();//重新打开个人信息
        }
        else if(book>0&&user<=0&&info<=0)
        {
            this->all_books();//重新打开显示界面
        }

    }


}

void MainWindow::showuser()
{
    Personal_info *t=new Personal_info(this);
    t->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    t->getdata(u);//传递用户信息
    int cur=ui->tabWidget->addTab(t,QString::asprintf("个人信息"));
    ui->tabWidget->setCurrentIndex(cur);//TAB形式显示
    ui->tabWidget->setVisible(true);
    connect(t,SIGNAL(send_close()),this,SLOT(receive_close()));//连接两个界面的信号

    ++is_any_user_open;//页面数+1
}

void MainWindow::is_admin()//管理员的权限
{

    QMenu *bookmenu=new QMenu(this);//给toolbutton添加下拉菜单
    bookmenu->addAction(ui->actionbookupdate);//更新书
    ui->actionbookupdate->setIcon(QIcon(":/images/update"));
    ui->actionbookupdate->setEnabled(false);
    bookmenu->addAction(ui->actionbookadd);//添加书
    ui->actionbookadd->setIcon(QIcon(":/images/add"));
    ui->actionbookadd->setEnabled(false);
    bookmenu->addAction(ui->actionbookdelete);//删除书
    ui->actionbookdelete->setIcon(QIcon(":/images/delete"));
    ui->actionbookdelete->setEnabled(false);

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//设置字符在图标底下
    //ui->toolBar->setPopupMode(QToolButton::MenuButtonPopup)
    QToolButton *bookac=new QToolButton(this);
    bookac->setPopupMode(QToolButton::InstantPopup);//下拉菜单，该按钮无事件
    bookac->setIcon(QIcon(":/images/admin-book"));
    bookac->setText("图书管理");
    bookac->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    bookac->setMenu(bookmenu);//添加菜单
    ui->toolBar->addWidget(bookac);//添加该组件至工具栏



    QMenu *usersmenu=new QMenu(this);//给toolbutton添加下拉菜单
    usersmenu->addAction(ui->actionallusers);//显示所有用户
    ui->actionallusers->setIcon(QIcon(":/images/user"));
    ui->actionallusers->setEnabled(false);
    usersmenu->addAction(ui->actionusersfind);//查找用户
    ui->actionusersfind->setIcon(QIcon(":/images/search"));
    ui->actionusersfind->setEnabled(false);
    usersmenu->addAction(ui->actionusersdelete);//删除用户
    ui->actionusersdelete->setIcon(QIcon(":/images/delete"));
    ui->actionusersdelete->setEnabled(false);

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//设置字符在图标底下
    //ui->toolBar->setPopupMode(QToolButton::MenuButtonPopup)
    QToolButton *usersac=new QToolButton(this);
    usersac->setPopupMode(QToolButton::InstantPopup);//下拉菜单，该按钮无事件
    usersac->setIcon(QIcon(":/images/admin-users"));
    usersac->setText("用户管理");
    usersac->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    usersac->setMenu(usersmenu);//添加菜单
    ui->toolBar->addWidget(usersac);//添加该组件至工具栏
    connect(ui->actionallusers,SIGNAL(triggered()),this,SLOT(showalluser()));
    what_is_admin=1;//设置管理员权限

}

void MainWindow::showalluser()
{
    Tablebookinfo *t=new Tablebookinfo(this);
    t->setObjectName("allusersinfo");
    t->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    t->showallusers();
    int cur=ui->tabWidget->addTab(t,QString::asprintf("用户信息 %d",ui->tabWidget->count()));
    ui->tabWidget->setCurrentIndex(cur);//TAB形式显示
    ui->tabWidget->setVisible(true);
    ++is_any_personinfo_open;
}

void MainWindow::win_close()//关闭页面有提示
{
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this,"退出？","将要退出主界面，是否确认？",QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();
    }
}

void MainWindow::all_books()//显示所有书籍的表格
{
    Tablebookinfo *t=new Tablebookinfo(this);
    t->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
    int cur=ui->tabWidget->addTab(t,QString::asprintf("图书信息 %d",ui->tabWidget->count()));
    ui->tabWidget->setCurrentIndex(cur);//TAB形式显示
    ui->tabWidget->setVisible(true);
    connect(t,SIGNAL(selected_ID(QString)),this,SLOT(can_b_r_books(QString)));

    //ui->actionborrow->setEnabled(true);//点开全部图书事件部分功能能使用
    //ui->actionreturn->setEnabled(true);
    ui->actionsearch->setEnabled(true);
    ui->actionauthor->setEnabled(true);
    ui->actionbookupdate->setEnabled(true);
    ui->actionbookadd->setEnabled(true);
    ui->actionbookdelete->setEnabled(true);
    ui->actionusersfind->setEnabled(true);
    ui->actionusersdelete->setEnabled(true);
    ui->actionallusers->setEnabled(true);

    ++is_any_book_open;//打开一个窗口

}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{//关闭Tab
    if (index<0)
    {
        return;}
    QWidget* aForm=ui->tabWidget->widget(index);
    if(ui->tabWidget->widget(index)->objectName()=="Tablebookinfo")
    {
        --is_any_book_open;//关掉显示书的界面-1
    }
    else if(ui->tabWidget->widget(index)->objectName()=="Personal_info")
    {
        --is_any_user_open;//关掉显示用户界面-1
    }
    else if(ui->tabWidget->widget(index)->objectName()=="allusersinfo")
    {
        --is_any_personinfo_open;
    }
    aForm->close();
}
void MainWindow::on_tabWidget_currentChanged(int index)
{
    bool  en=ui->tabWidget->count()>0; //再无页面时，actions禁用
    ui->tabWidget->setVisible(en);
    if(this->isVisible())//主窗口必须打开
    {
        if(index>-1)//必须有窗口
        {
            if(ui->tabWidget->currentWidget()->objectName()=="Tablebookinfo")//必须现在的窗口是图书窗口才能直接借书还书
                {
                    ui->actionborrow->setEnabled(en&&is_any_book_open>0&&is_book_selected);
                    ui->actionreturn->setEnabled(en&&is_any_book_open>0&&is_book_selected);
                }
            else
                {//否则不能使用
                    ui->actionborrow->setEnabled(false);
                    ui->actionreturn->setEnabled(false);
                }
        }
        else
        {
            ui->actionborrow->setEnabled(false);
            ui->actionreturn->setEnabled(false);
        }
    }

    //没有点开全部图书事件部分功能不能使用

    ui->actionsearch->setEnabled(en&&is_any_book_open>0);
    ui->actionauthor->setEnabled(en&&is_any_book_open>0);
    ui->actionbookupdate->setEnabled(en&&is_any_book_open>0);
    ui->actionbookadd->setEnabled(en&&is_any_book_open>0);
    ui->actionbookdelete->setEnabled(en&&is_any_book_open>0);
    ui->actionusersfind->setEnabled(en&&is_any_book_open>0);
    ui->actionusersdelete->setEnabled(en&&is_any_book_open>0);
    ui->actionallusers->setEnabled(en&&is_any_book_open>0);
}

void MainWindow::logout()//注销事件
{
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this,"注销？","将要注销账号，是否确认？",QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();
        Start_n_login_window *s= new Start_n_login_window();
        s->show();//打开登陆窗口

    }
}

void MainWindow::receive_close()//从修改密码发送过来的信号指导该界面关闭
{
    this->close();
}

void MainWindow::search_win()//打开查询窗口
{
    Searchbook *s=new Searchbook(this);
    connect(s,SIGNAL(send_update_userinfo(QSqlQuery)),this,SLOT(userinfo(QSqlQuery)));//连接两个模块，更新用户借阅信息
    s->search_initialize();
    s->getdata(u);//传播用户信息
    s->show();
}

void MainWindow::find_author()//找作者
{
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
    bool ok=false;
    QString text=QInputDialog::getText(this,"作者查询","输入作者姓名：",echoMode,"",&ok);
    if (ok && !text.isEmpty())//确定且不空
    {
        Tablebookinfo *t=new Tablebookinfo(this);
        t->setAttribute(Qt::WA_DeleteOnClose);//关闭时自动删除
        connect(t,SIGNAL(selected_ID(QString)),this,SLOT(can_b_r_books(QString)));
        bool isfind=t->author_books(text);
        if(isfind)
        {
            int cur=ui->tabWidget->addTab(t,QString::asprintf("该作者图书信息"));
            ui->tabWidget->setCurrentIndex(cur);//TAB形式显示
            ui->tabWidget->setVisible(true);

            ui->actionborrow->setEnabled(true);//点开全部图书事件部分功能能使用
            ui->actionreturn->setEnabled(true);
            ui->actionsearch->setEnabled(true);
            ui->actionauthor->setEnabled(true);
            ui->actionbookupdate->setEnabled(true);
            ui->actionbookadd->setEnabled(true);
            ui->actionbookdelete->setEnabled(true);
            ui->actionusersfind->setEnabled(true);
            ui->actionusersdelete->setEnabled(true);
            ui->actionallusers->setEnabled(true);

            ++is_any_book_open;//打开一个窗口
        }

    }
    else if(ok && text.isEmpty())
    {
        QMessageBox::critical(this,"警告！","没有输入姓名");
    }
}

void MainWindow::can_b_r_books(QString bnum)
{
    is_book_selected=1;
    ui->actionborrow->setEnabled(true);//点开全部图书事件部分功能能使用
    ui->actionreturn->setEnabled(true);
    book_num=bnum;
}

void MainWindow::directly_borrow()//从表中借书
{
    QSqlQuery q;//开始查询数据库
    QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书号='%1';")\
            .arg(book_num);
    //搜索
    q.exec(sqlcmd);
    q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
    if(q.value(0).toString().isEmpty())//如果为空没找到
    {
        QMessageBox::information(this,"提示!","没有找到此书");
    }
    else//找到
    {
        if(q.value(6).toString()=="1"&&q.value(2).toString()!="馆藏精品")//在馆且不是精品
        {
            QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n是否要借阅此书？")\
                    .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                    .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
            int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
            QString bookname=q.value(3).toString();//书名之后有用
            if(i==0)//确认借书
            {
                QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书号='%3';")\
                        .arg("0").arg(u.value(1).toString()).arg(book_num);
                //修改书籍信息
                q.exec(sqlcmd);
                q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                {//更新用户借书的信息
                    QString borrow_books=u.value(4).toString();
                    if(borrow_books.isEmpty())//第一次借书情况
                    {
                        borrow_books=bookname;
                    }
                    else//不是第一次借书
                    {
                        borrow_books=borrow_books+","+bookname;
                    }
                    QString sqlcmd=QString("UPDATE UserInfoTable SET 借阅信息='%1' WHERE 用户名='%2';")\
                            .arg(borrow_books).arg(u.value(1).toString());
                    //修改用户信息
                    q.exec(sqlcmd);
                    q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                    if(q.lastError().text().isEmpty())//无错误则说明用户信息操作成功
                    {
                        QString sqlcmd=QString("SELECT * FROM UserInfoTable WHERE 用户名='%1';")\
                                .arg(u.value(1).toString());
                        //找到用户信息为了更新
                        q.exec(sqlcmd);
                        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                        QMessageBox::information(this,"提示！","借书成功");
                        u=q;//用户信息更新
                        //ui->tabWidget->clear();//清除现有界面
                        //this->showuser();//重新打开显示界面
                        //this->all_books();//重新打开显示界面
                        this->refresh();

                    }
                    else
                        QMessageBox::critical(this,"错误！","数据库出现问题，无法借书");
                }
                else
                {
                    QMessageBox::critical(this,"错误！","数据库出现问题，无法借书");
                    this->close();
                }
            }

        }
        else if(q.value(6).toString()=="0")//不在馆
        {
            if(u.value(1).toString()!=q.value(7).toString())//借书人不是持有者
            {
                QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书不在馆无法借阅。")\
                        .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                        .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
            }

        }
        else if(q.value(2).toString()=="馆藏精品")//馆藏精品不借
        {
            QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书是馆藏精品，只限馆内阅读。")\
                    .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                    .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
            QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
        }
    }
}

void MainWindow::directly_return()
{
    QSqlQuery q;//开始查询数据库
    QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书号='%1';")\
            .arg(book_num);
    //搜索
    q.exec(sqlcmd);
    q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
    if(q.value(0).toString().isEmpty())//如果为空没找到
    {
        QMessageBox::information(this,"提示!","没有找到此书");
    }
    else//找到
    {
        if(q.value(6).toString()=="1"&&q.value(2).toString()!="馆藏精品")//在馆且不是精品
        {
            QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书已在馆。")\
                    .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                    .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
            QMessageBox::information(this,"提示！",bookinfo,tr("确定"));

        }
        else if(q.value(6).toString()=="0")//不在馆
        {
            if(u.value(1).toString()!=q.value(7).toString())//借书人不是持有者
            {
                QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n不是该书的借阅者，不能归还。")\
                        .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                        .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
            }
            else//借书人是持有者，可以还书
            {
                QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n你是该书持有者，是否还书？")\
                        .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                        .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                QString bookname=q.value(3).toString();//书名之后有用
                int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                if(i==0)//确认还书
                {
                    QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书号='%3';")\
                            .arg("1").arg("").arg(book_num);
                    //修改书籍信息
                    q.exec(sqlcmd);
                    q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                    if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                    {//更新用户借书的信息
                        QString borrow_books=u.value(4).toString();
                        QStringList b_b_list=borrow_books.split(',');//分开书名准备去掉归还的书
                        borrow_books.clear();//清空
                        b_b_list.removeOne(bookname);
                        if(b_b_list.isEmpty())//没有书的情况
                        {
                            borrow_books="";//空值
                        }
                        else//不只一本书
                        {
                            borrow_books=b_b_list[0];
                            for(int j=1;j<b_b_list.size();j++)
                            {
                                borrow_books=borrow_books+","+b_b_list[j];
                            }
                        }
                        QString sqlcmd=QString("UPDATE UserInfoTable SET 借阅信息='%1' WHERE 用户名='%2';")\
                                .arg(borrow_books).arg(u.value(1).toString());
                        //修改用户信息
                        q.exec(sqlcmd);
                        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                        if(q.lastError().text().isEmpty())//无错误则说明用户信息操作成功
                        {

                            QString sqlcmd=QString("SELECT * FROM UserInfoTable WHERE 用户名='%1';")\
                                    .arg(u.value(1).toString());
                            //找到用户信息为了更新
                            q.exec(sqlcmd);
                            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                            QMessageBox::information(this,"提示！","还书成功");
                            u=q;//发送更新信号给主窗口，下一次查看用户信息即更新借书信息
                            ui->tabWidget->clear();//清除现有界面
                            //this->showuser();//重新打开显示界面
                            //this->all_books();//重新打开显示界面
                            this->refresh();
                        }
                        else
                            QMessageBox::critical(this,"错误！","数据库出现问题，无法还书");
                    }
                    else
                    {
                        QMessageBox::critical(this,"错误！","数据库出现问题，无法还书");
                        this->close();
                    }
                }
            }
        }
        else if(q.value(2).toString()=="馆藏精品")//馆藏精品不借
        {
            QString bookinfo=QString("书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书是馆藏精品，已在馆内。")\
                    .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                    .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
            QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
        }
    }
}


void MainWindow::admin_delete()//删除窗口建立
{
    Searchbook *s=new Searchbook(this);
    connect(s,SIGNAL(send_update_userinfo(QSqlQuery)),this,SLOT(userinfo(QSqlQuery)));//连接两个模块，更新用户借阅信息
    s->book_delete();//进入删除模块
    s->getdata(u);//传播用户信息
    s->show();
}

void MainWindow::admin_add()
{
    Add_N_Update *a= new Add_N_Update(this);
    connect(a,SIGNAL(send_refresh()),this,SLOT(refresh()));
    a->show();
    a->add_book();//进入添加模块


}

void MainWindow::admin_update()
{
    Add_N_Update *a= new Add_N_Update(this);
    connect(a,SIGNAL(send_refresh()),this,SLOT(refresh()));
    a->update_book();//进入更新模块
    a->show();
}

void MainWindow::admin_user_delete()
{
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
    bool ok=false;
    QString text=QInputDialog::getText(this,"用户删除","输入用户名：",echoMode,"",&ok);
    if (ok && !text.isEmpty())//确定且不空
    {
        QSqlQuery find_u;
        QString sqlcmd=QString("SELECT * FROM UserInfoTable WHERE 用户名='%1';").arg(text);//查询所有用户
        find_u.exec(sqlcmd);
        find_u.next();

        if(find_u.value(0).toString().isEmpty())//如果为空没找到
        {

            QMessageBox::information(this,"提示!","没有找到该用户,无法删除");
        }
        else if(text==u.value(1).toString())
        {
            QMessageBox::critical(this,"错误!","无法删除自己");
        }
        else//找到
        {
            if(find_u.value(4).toString().isEmpty())//没有借书则可以删除
            {

                QMessageBox::StandardButton result;//返回选择的按钮
                result=QMessageBox::question(this,"是否删除？","找到该用户，是否删除？",QMessageBox::Yes|QMessageBox::No);
                if (result==QMessageBox::Yes)//确认删除
                {
                    QString sqlcmd=QString("DELETE * FROM UserInfoTable WHERE 用户名='%1';").arg(text);//查询所有用户
                    find_u.exec(sqlcmd);
                    find_u.next();
                    if(find_u.lastError().text().isEmpty())//无错误则说明删除操作成功
                    {//更新用户借书的信息

                            QMessageBox::information(this,"提示！","删除成功");
                            //ui->tabWidget->clear();//清除现有界面
                            this->refresh();//查看用户信息更新

                    }
                    else
                    {
                        QMessageBox::critical(this,"错误！","数据库出现问题，无法删除");
                        this->close();
                    }
                }

            }
            else if(!find_u.value(4).toString().isEmpty())//借了书不能删除
            {

                QMessageBox::information(this,"提示！","该用户还有借阅图书，不能删除。\n必须先归还图书",tr("确定"));

            }
        }
    }
    else if(ok && text.isEmpty())
    {
        QMessageBox::critical(this,"警告！","没有输入用户名");
    }
}

void MainWindow::admin_user_find()
{
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
    bool ok=false;
    QString text=QInputDialog::getText(this,"用户查询","输入用户名：",echoMode,"",&ok);
    if (ok && !text.isEmpty())//确定且不空
    {
        QSqlQuery find_u;
        QString sqlcmd=QString("SELECT * FROM UserInfoTable WHERE 用户名='%1';").arg(text);//查询所有用户
        find_u.exec(sqlcmd);
        find_u.next();

        if(find_u.value(0).toString().isEmpty())//如果为空没找到
        {
            QMessageBox::information(this,"提示!","没有找到该用户");
        }
        else//找到
        {
            if(find_u.value(2).toString()=="1")//管理员
            {
                QString bookinfo=QString("ID：%1\n用户名：%2\n用户类型：%3\n用户密码：%4\n借阅图书：%5\n")\
                        .arg(find_u.value(0).toString()).arg(find_u.value(1).toString()).arg("管理员")\
                        .arg(find_u.value(3).toString()).arg(find_u.value(4).toString());
                QMessageBox::information(this,"提示！",bookinfo,tr("确定"));

            }
            else if(find_u.value(2).toString()=="0")//普通用户
            {
                QString bookinfo=QString("ID：%1\n用户名：%2\n用户类型：%3\n用户密码：%4\n借阅图书：%5\n")\
                        .arg(find_u.value(0).toString()).arg(find_u.value(1).toString()).arg("普通用户")\
                        .arg(find_u.value(3).toString()).arg(find_u.value(4).toString());
                QMessageBox::information(this,"提示！",bookinfo,tr("确定"));

            }
        }
    }
    else if(ok && text.isEmpty())
    {
        QMessageBox::critical(this,"警告！","没有输入用户名");
    }
}
