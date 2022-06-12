#include "start_n_login_window.h"
#include "ui_start_n_login_window.h"
#include "database.h"
#include "start_n_login_window.h"
#include "signup.h"
#include "mainwindow.h"

extern Database d;

Start_n_login_window::Start_n_login_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Start_n_login_window)
{
    ui->setupUi(this);
    //setAttribute(Qt::WA_DeleteOnClose);//删除释放空间
    setWindowFlag(Qt::WindowMaximizeButtonHint,false);//不能最大化

    setWindowIcon(QIcon(":/images/login"));//设置ico
    ui->selectuser->setIcon(QIcon(":/images/user"));
    ui->selectadmin->setIcon(QIcon(":/images/admin"));

    connect(ui->selectuser,SIGNAL(clicked()),this,SLOT(on_selectuser_clicked()));//信号连接
    connect(ui->selectadmin,SIGNAL(clicked()),this,SLOT(on_selectuser_clicked()));
    connect(ui->login,SIGNAL(clicked()),this,SLOT(login()));
    connect(ui->signup,SIGNAL(clicked()),this,SLOT(U_signup()));
    connect(ui->quit,SIGNAL(clicked()),this,SLOT(win_close()));
    connect(ui->showpawd,SIGNAL(pressed()),this,SLOT(showpawd()));
    connect(ui->showpawd,SIGNAL(released()),this,SLOT(hidepawd()));
}

Start_n_login_window::~Start_n_login_window()
{
    delete ui;
}

void Start_n_login_window::login()//登陆函数
{
    QString userinfo=ui->userline->text();
    QString userpawd=ui->passwordline->text();

    bool whatislogin;//检查是什么类型用户在登陆

    //qDebug()<<ui->selectuser->isChecked();
    if(ui->selectuser->isChecked()||ui->selectadmin->isChecked())//检查是否选择登陆类型
    {
        whatislogin=this->on_selectuser_clicked();//必须选择才能进行下去

        QSqlQuery q;//开始查询数据库
        QString sqlcmd=QString("SELECT * FROM UserInfoTable where 用户类型=%1 and 用户名='%2';")\
                .arg(QString::number(whatislogin)).arg(userinfo);
        //根据用户类型&用户名搜索
        q.exec(sqlcmd);
        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。

        if(q.value(3).toString().isEmpty())//如果查询为空则不存在该用户
        {
            QMessageBox::warning(this,"提示","不存在该用户");
        }
        else if(q.value(3).toString()!=userpawd)//密码不对
        {
            QMessageBox::critical(this,"警告！","密码输入错误");
        }
        else if(q.value(3).toString()==userpawd)//成功输入
        {
            QMessageBox::information(this,"恭喜！","用户登陆成功");
            //打开主界面
            if(whatislogin==1)//管理员登陆
            {
                MainWindow *m =new MainWindow;//不做任何功能限制
                this->close();//关闭登陆窗口
                m->is_admin();//加入管理员功能
                m->userinfo(q);//传递用户信息
                m->show();
            }
            else if(whatislogin==0)//用户登陆
            {
                MainWindow *m =new MainWindow;
                this->close();//关闭登陆窗口
                m->userinfo(q);//传递用户信息
                m->show();
            }

        }
    }

    else
    {
        QMessageBox::critical(this,"警告！","没有选择用户登陆类型");
    }

}

bool Start_n_login_window::on_selectuser_clicked()//区别登录用户种类
{
    bool a=false;
    if(ui->selectuser->isChecked())//用户登录
    {
        return a;
    }
    else if(ui->selectadmin->isChecked())//管理员登陆
    {
        a=true;
        return a;
    }
}


void Start_n_login_window::U_signup()//打开注册页面
{
    Signup *s=new Signup(this);
    s->setWindowFlag(Qt::Window,true);//设置为无边框
    s->show();//显示
}

void Start_n_login_window::win_close()//关闭页面有提示
{
    QMessageBox::StandardButton result;//返回选择的按钮
    result=QMessageBox::question(this,"退出？","将要退出登陆界面，是否确认？",QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes)
    {
        this->close();
    }
}

void Start_n_login_window::showpawd()//按下显示密码，
{

    ui->passwordline->setEchoMode(QLineEdit::Normal);

}

void Start_n_login_window::hidepawd()//松开变为掩码
{
    ui->passwordline->setEchoMode(QLineEdit::Password);
}
