#include "personal_info.h"
#include "ui_personal_info.h"
#include<start_n_login_window.h>
#include<mainwindow.h>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>

Personal_info::Personal_info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Personal_info)
{
    ui->setupUi(this);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(change_pawd()));
}

Personal_info::~Personal_info()
{
    delete ui;
}

void Personal_info::getdata(QSqlQuery l)//显示用户个人信息
{
    ui->ID_s->setText(l.value(0).toString());
    ui->name_s->setText(l.value(1).toString());
    if(l.value(2).toString()=="0")
        ui->kind_s->setText("普通用户");
    else if(l.value(2).toString()=="1")
        ui->kind_s->setText("管理员");
    ui->book_s->setText(l.value(4).toString());
}

void Personal_info::change_pawd()//修改密码
{
    QString newpawd=ui->lineEdit->text();
    if(!ui->lineEdit->isEnabled())//输入框没有启用则点击按钮启用
    {
        ui->lineEdit->setEnabled(true);
    }

    else if(ui->lineEdit->isEnabled()&&newpawd.isEmpty())//输入框启用但没有输入，点击按钮关闭输入框
    {
        ui->lineEdit->setEnabled(false);
    }

    else if(ui->lineEdit->isEnabled()&&!newpawd.isEmpty())//输入框启用且有输入，点击按钮修改密码
    {
        QMessageBox::StandardButton result;//返回选择的按钮
        result=QMessageBox::question(this,"修改？","将要修改密码，是否确认？",QMessageBox::Yes|QMessageBox::No);
        if(result==QMessageBox::Yes)
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("UPDATE UserInfoTable SET 用户密码 ='%1' where 用户名='%2';")\
                    .arg(newpawd).arg(ui->name_s->text());
            //根据用户名更新密码
            q.exec(sqlcmd);
            if(q.lastError().text().isEmpty())//无错误则说明插入成功
            {
                QMessageBox::information(this,"提示！","已成功修改密码");
                QMessageBox::information(this,"提示！","需要重新登陆");
                this->close();//关闭修改界面
                emit send_close();//发送关闭主界面的信号
                Start_n_login_window *s=new Start_n_login_window();//重新登陆
                s->show();

            }
            else
            {
                QMessageBox::critical(this,"错误！","数据库出现问题，无法修改");
                this->close();
            }
        }
    }
}
