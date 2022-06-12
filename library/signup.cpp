#include "signup.h"
#include "ui_signup.h"
#include "headers.h"

Signup::Signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Signup)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);//删除释放空间
    setWindowTitle("注册");
    setWindowFlag(Qt::WindowMinMaxButtonsHint,false);//无最大最小化按钮
    setWindowFlag(Qt::WindowContextHelpButtonHint,true);//帮助中可以看到管理员邀请码，测试使用
    setWindowModality(Qt::WindowModal); //弹出其他窗口阻塞当前主窗口

    ui->s_user_b->setIcon(QIcon(":/images/user"));//ico
    ui->s_admin_b->setIcon(QIcon(":/images/admin"));

    connect(ui->s_user_b,SIGNAL(clicked()),this,SLOT(on_selectuser_clicked()));//信号连接
    connect(ui->s_admin_b,SIGNAL(clicked()),this,SLOT(on_selectuser_clicked()));
    connect(ui->signup_b,SIGNAL(clicked()),this,SLOT(signup_action()));
}

Signup::~Signup()
{
    delete ui;
}

bool Signup::on_selectuser_clicked()//区别注册用户种类
{
    bool a=false;
    if(ui->s_user_b->isChecked())//用户注册
    {
        ui->code_line->setEnabled(false);//取消管理员邀请码
        return a;
    }
    else if(ui->s_admin_b->isChecked())//管理员注册
    {
        a=true;
        ui->code_line->setEnabled(true);//增加管理员邀请码
        return a;
    }
}

void Signup::signup_action()//注册函数
{
    QString userinfo=ui->s_userline->text();
    QString userpawd=ui->s_pawdline->text();
    //qDebug()<<userinfo<<" "<<userpawd;
    bool whatislogin;//检查是什么类型用户在注册

    //qDebug()<<ui->selectuser->isChecked();
    if(ui->s_user_b->isChecked()||ui->s_admin_b->isChecked())//检查是否选择注册类型
    {
        whatislogin=this->on_selectuser_clicked();//必须选择才能进行下去

        QSqlQuery q;//开始查询数据库
        QString sqlcmd=QString("SELECT 用户名 FROM UserInfoTable WHERE 用户类型=%1 and 用户名='%2';")\
                .arg(QString::number(whatislogin)).arg(userinfo);
        //根据用户类型&用户名搜索
        q.exec(sqlcmd);
        //QSqlRecord re=q.record();
        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
        //qDebug()<<q.value(0).toString()<<"\n"<<sqlcmd<<"\n";

        if(q.value(0).toString().isEmpty())//如果查询为空则不存在该用户，可以注册
        {
            if(!whatislogin)//用户注册
            {
                QMessageBox::information(this,"恭喜！","用户注册成功");
                QString sqlcmd=QString("INSERT INTO UserInfoTable ( 用户名, 用户类型, 用户密码 ) VALUES ('%1', %2, '%3');")\
                        .arg(userinfo).arg(QString::number(whatislogin)).arg(userpawd);//插入数据
                q.exec(sqlcmd);
                this->close();
            }
            else if(whatislogin)//管理员注册
            {
                if(ui->code_line->text()=="admin@123")
                {
                    QMessageBox::information(this,"恭喜！","管理员注册成功");
                    QString sqlcmd=QString("INSERT INTO UserInfoTable ( 用户名, 用户类型, 用户密码 ) VALUES ('%1', %2, '%3');")\
                            .arg(userinfo).arg(QString::number(whatislogin)).arg(userpawd);//插入数据
                    q.exec(sqlcmd);
                    this->close();
                }
                else
                {
                    QMessageBox::critical(this,"警告！","邀请码错误！");
                }
            }
        }
        else if(q.value(0).toString()==userinfo)//用户名不能重复
        {
            QMessageBox::critical(this,"警告！","用户名重复！");
        }
    }

    else
    {
        QMessageBox::critical(this,"警告！","没有选择用户注册类型");
    }

}
