#include "add_n_update.h"
#include "ui_add_n_update.h"
#include <QRegExpValidator>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QInputDialog>

Add_N_Update::Add_N_Update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Add_N_Update)
{
    ui->setupUi(this);

    setWindowModality(Qt::WindowModal); //弹出其他窗口阻塞当前主窗口

    ui->line_bnum->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));//书号只能输入数字
    ui->line_bname->setValidator(new QRegExpValidator(QRegExp(".*")));//识别除换行符以外的
    ui->line_bauthor->setValidator(new QRegExpValidator(QRegExp(".*")));
    ui->line_bpublish->setValidator(new QRegExpValidator(QRegExp(".*")));
    ui->line_bkind->setValidator(new QRegExpValidator(QRegExp("中文书|英文书|馆藏精品")));//种类输入限制

    connect(ui->check_bnum,SIGNAL(clicked(bool)),this,SLOT(bnum_enable(bool)));//信号连接
    connect(ui->check_bkind,SIGNAL(clicked(bool)),this,SLOT(bkind_enable(bool)));
    connect(ui->check_bname,SIGNAL(clicked(bool)),this,SLOT(bname_enable(bool)));
    connect(ui->check_bauthor,SIGNAL(clicked(bool)),this,SLOT(bauthor_enable(bool)));
    connect(ui->check_bpublish,SIGNAL(clicked(bool)),this,SLOT(bpublish_enable(bool)));


}

Add_N_Update::~Add_N_Update()
{
    delete ui;
}

void Add_N_Update::update_book()//更新初始化
{
    setAttribute(Qt::WA_DeleteOnClose);//关闭释放
    setWindowTitle("图书更新");
    ui->do_b->setText("更新");

    ui->line_bnum->setEnabled(false);//布局，更新需要选择性填写，通过checkbox控制条目是否更新
    ui->check_bnum->setEnabled(true);
    ui->check_bnum->setChecked(false);

    ui->line_bkind->setEnabled(false);
    ui->check_bkind->setEnabled(true);
    ui->check_bkind->setChecked(false);

    ui->line_bname->setEnabled(false);
    ui->check_bname->setEnabled(true);
    ui->check_bname->setChecked(false);

    ui->line_bauthor->setEnabled(false);
    ui->check_bauthor->setEnabled(true);
    ui->check_bauthor->setChecked(false);

    ui->line_bpublish->setEnabled(false);
    ui->check_bpublish->setEnabled(true);
    ui->check_bpublish->setChecked(false);

    connect(ui->do_b,SIGNAL(clicked()),this,SLOT(do_update()));

    QStringList items;
    items <<"按书名查找"<<"按书号查找";
    QString dlgTitle="查找删除书目";
    QString txtLabel="请选择查找方式";
    int     curIndex=0; //初始选择项
    bool    editable=false; //ComboBox是否可编辑
    bool    ok=false;
    QString  method = QInputDialog::getItem(this, dlgTitle,txtLabel,items,curIndex,editable,&ok);
    if (ok && method=="按书名查找")
    {
        QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入

        ok=false;
        QString text = QInputDialog::getText(this, "按书名查找","输入书名", echoMode,"", &ok);
        if (ok && !text.isEmpty())
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书名='%1';")\
                    .arg(text);
            //搜索
            q.exec(sqlcmd);
            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
            if(q.value(0).toString().isEmpty())//如果为空没找到
            {
                QMessageBox::information(this,"提示!","没有找到此书");
                this->close();
            }
            else//找到
            {
                if(q.value(6).toString()=="1")//在馆
                {
                    bookinfo=q.value(3).toString();//私有属性

                }
                else if(q.value(6).toString()=="0")//不在馆
                {

                                QMessageBox::critical(this,"错误！","该书不在馆，无法修改!");
                                this->close();


                }
        }}
        else
        {
            QMessageBox::critical(this,"错误","未填写书名");
            this->close();
        }
    }
    else if(ok && method=="按书号查找")
    {

        ok=false;
        int t = QInputDialog::getInt(this, "按书号查找","输入书号",10000,9999,99999999,1,&ok);//只输入数字
        if (ok)
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书号='%1';")\
                    .arg(QString::number(t));
            //搜索
            q.exec(sqlcmd);
            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
            if(q.value(0).toString().isEmpty())//如果为空没找到
            {
                QMessageBox::information(this,"提示!","没有找到此书");
                this->close();
            }
            else//找到
            {
                if(q.value(6).toString()=="1")//在馆
                {
                    bookinfo=q.value(3).toString();//私有属性

                }
                else if(q.value(6).toString()=="0")//不在馆
                {

                                QMessageBox::critical(this,"错误！","该书不在馆，无法修改!");
                                this->close();


                }
        }
        }
        else
        {
            this->close();
        }
    }
    else
    {
        this->close();
    }
}

void Add_N_Update::add_book()//添加初始化
{
    setAttribute(Qt::WA_DeleteOnClose);//关闭释放
    setWindowTitle("图书添加");
    ui->do_b->setText("添加");

    ui->line_bnum->setEnabled(true);//布局，添加需要全部填写
    ui->check_bnum->setEnabled(false);
    ui->check_bnum->setChecked(true);

    ui->line_bkind->setEnabled(true);
    ui->check_bkind->setEnabled(false);
    ui->check_bkind->setChecked(true);

    ui->line_bname->setEnabled(true);
    ui->check_bname->setEnabled(false);
    ui->check_bname->setChecked(true);

    ui->line_bauthor->setEnabled(true);
    ui->check_bauthor->setEnabled(false);
    ui->check_bauthor->setChecked(true);

    ui->line_bpublish->setEnabled(true);
    ui->check_bpublish->setEnabled(false);
    ui->check_bpublish->setChecked(true);

    connect(ui->do_b,SIGNAL(clicked()),this,SLOT(do_add()));
}

void Add_N_Update::do_add()//添加按钮事件
{
    if(ui->line_bnum->text().isEmpty()||ui->line_bkind->text().isEmpty()||ui->line_bname->text().isEmpty()|| \
            ui->line_bauthor->text().isEmpty()||ui->line_bpublish->text().isEmpty())//只要由一个输入为空则报错
    {
        QMessageBox::critical(this,"错误！","有未填写字段，请检查。");
    }
    else
    {
        QSqlQuery in;
        QString sqlcmd=QString("INSERT INTO BookInfoTable (书号,类型,书名,作者,出版社) VALUES ('%1','%2','%3','%4','%5');")\
                .arg(ui->line_bnum->text()).arg(ui->line_bkind->text()).arg(ui->line_bname->text())\
                .arg(ui->line_bauthor->text()).arg(ui->line_bpublish->text());

        //搜索
        in.exec(sqlcmd);
        in.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
        if(in.lastError().text().isEmpty())//无错则插入成功
        {
            QMessageBox::information(this,"成功！","图书添加成功");
            ui->line_bnum->clear();//清空输入
            ui->line_bkind->clear();
            ui->line_bname->clear();
            ui->line_bauthor->clear();
            ui->line_bpublish->clear();

            emit send_refresh();//刷新界面
        }
        else
        {
            QMessageBox::critical(this,"错误！","图书添加失败，检查数据库\n或者书号重复。");
        }
    }
}


void Add_N_Update::do_update()
{
    QString update_items="";//记录更新项目
    if(!ui->line_bnum->text().isEmpty()&&ui->line_bnum->isEnabled())//非空且可用
    {
        if(update_items.isEmpty())//为空则第一个不加逗号
        {
            if(ui->line_bnum->text().toInt()>=9999&&ui->line_bnum->text().toInt()<=99999999)
                update_items="书号='"+ui->line_bnum->text()+"'";
            else
                QMessageBox::critical(this,"警告！","输入书号范围在9999~99999999");
        }
        else
        {
            if(ui->line_bnum->text().toInt()>=9999&&ui->line_bnum->text().toInt()<=99999999)
                update_items=update_items+","+"书号='"+ui->line_bnum->text()+"'";
            else
                QMessageBox::critical(this,"警告！","输入书号范围在9999~99999999");

        }

    }
    if(!ui->line_bkind->text().isEmpty()&&ui->line_bkind->isEnabled())//非空且可用
    {
        if(update_items.isEmpty())//为空则第一个不加逗号
        {
            update_items="类型='"+ui->line_bkind->text()+"'";
        }
        else
        {
            update_items=update_items+","+"类型='"+ui->line_bkind->text()+"'";
        }

    }
    if(!ui->line_bname->text().isEmpty()&&ui->line_bname->isEnabled())//非空且可用
    {
        if(update_items.isEmpty())//为空则第一个不加逗号
        {
            update_items="书名='"+ui->line_bname->text()+"'";
        }
        else
        {
            update_items=update_items+","+"书名='"+ui->line_bname->text()+"'";
        }

    }
    if(!ui->line_bauthor->text().isEmpty()&&ui->line_bauthor->isEnabled())//非空且可用
    {
        if(update_items.isEmpty())//为空则第一个不加逗号
        {
            update_items="作者='"+ui->line_bauthor->text()+"'";
        }
        else
        {
            update_items=update_items+","+"作者='"+ui->line_bauthor->text()+"'";
        }

    }
    if(!ui->line_bpublish->text().isEmpty()&&ui->line_bpublish->isEnabled())//非空且可用
    {
        if(update_items.isEmpty())//为空则第一个不加逗号
        {
            update_items="出版社='"+ui->line_bpublish->text()+"'";
        }
        else
        {
            update_items=update_items+","+"出版社='"+ui->line_bpublish->text()+"'";
        }

    }



    if(!update_items.isEmpty())//有修改项目
    {
        QSqlQuery q;
        QString sqlcmd=QString("UPDATE BookInfoTable SET %1 WHERE 书名='%2';")\
                .arg(update_items).arg(bookinfo);
        //修改书籍信息
        q.exec(sqlcmd);
        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
        if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
        {//更新信息
            QMessageBox::information(this,"提示！","修改成功");
            this->close();
            emit send_refresh();
        }
        else
        {
            QMessageBox::critical(this,"错误！","数据库出现问题，无法修改\n或者检查是否更新书号重复。");
            this->close();
        }
    }
    else
    {
        QMessageBox::critical(this,"错误","无更新项目!");
    }
}

void Add_N_Update::bnum_enable(bool check)//checkbox信号控制
{
    ui->line_bnum->setEnabled(check);
}

void Add_N_Update::bkind_enable(bool check)//checkbox信号控制
{
    ui->line_bkind->setEnabled(check);
}

void Add_N_Update::bname_enable(bool check)//checkbox信号控制
{
    ui->line_bname->setEnabled(check);
}

void Add_N_Update::bauthor_enable(bool check)//checkbox信号控制
{
    ui->line_bauthor->setEnabled(check);
}

void Add_N_Update::bpublish_enable(bool check)//checkbox信号控制
{
    ui->line_bpublish->setEnabled(check);
}
