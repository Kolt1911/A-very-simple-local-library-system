#include "searchbook.h"
#include "ui_searchbook.h"
#include<QDebug>
#include<QMessageBox>
#include<QSqlError>


Searchbook::Searchbook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Searchbook)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal); //弹出其他窗口阻塞当前主窗口


}

Searchbook::~Searchbook()
{
    delete ui;
}

void Searchbook::getdata(QSqlQuery l)//接受用户信息
{
    u=l;
}

void Searchbook::valueChanged(int current_index)
{
    if(current_index==0)//第第一个请选择不解开锁定
    {
        ui->lineEdit->clear();//清空
        ui->lineEdit->setEnabled(false);
    }
    if(current_index==1)//按书名查找
    {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit->clear();//清空
        ui->lineEdit->setValidator(new QRegExpValidator(QRegExp(".*")));//识别除换行符以外的

    }
    if(current_index==2)//按书号查找
    {
        ui->lineEdit->setEnabled(true);
        ui->lineEdit->clear();//清空
        ui->lineEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));//只能输入数字
    }
}

void Searchbook::search_initialize()//查询界面初始化
{
    setAttribute(Qt::WA_DeleteOnClose);//关闭释放
    setWindowTitle("图书查询");
    ui->lineEdit->setEnabled(false);
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(valueChanged(int)));
    connect(ui->s_b,SIGNAL(clicked()),this,SLOT(search()));
    //disconnect(ui->s_b,SIGNAL(clicked()),this,SLOT(delete_opration()));

}

void Searchbook::search()
{
    if(ui->lineEdit->isEnabled()&&(!ui->lineEdit->text().isEmpty()))//输入有值且可用
    {
        if(ui->comboBox->currentIndex()==1)//按书名查询
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书名='%1';")\
                    .arg(ui->lineEdit->text());
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
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n是否要借阅此书？")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                    if(i==0)//确认借书
                    {
                        QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书名='%3';")\
                                .arg("0").arg(u.value(1).toString()).arg(ui->lineEdit->text());
                        //修改书籍信息
                        q.exec(sqlcmd);
                        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                        if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                        {//更新用户借书的信息
                            QString borrow_books=u.value(4).toString();
                            if(borrow_books.isEmpty())//第一次借书情况
                            {
                                borrow_books=ui->lineEdit->text();
                            }
                            else//不是第一次借书
                            {
                                borrow_books=borrow_books+","+ui->lineEdit->text();
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
                                emit send_update_userinfo(q);//发送更新信号给主窗口，下一次查看用户信息即更新借书信息
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
                        QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书已经变成蝴蝶飞走了。")\
                                .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                                .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                        QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
                    }
                    else//借书人是持有者，可以还书
                    {
                        QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n你是该书持有者，是否还书？")\
                                .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                                .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                        int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                        if(i==0)//确认还书
                        {
                            QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书名='%3';")\
                                    .arg("1").arg("").arg(ui->lineEdit->text());
                            //修改书籍信息
                            q.exec(sqlcmd);
                            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                            if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                            {//更新用户借书的信息
                                QString borrow_books=u.value(4).toString();
                                QStringList b_b_list=borrow_books.split(',');//分开书名准备去掉归还的书
                                borrow_books.clear();//清空
                                b_b_list.removeOne(ui->lineEdit->text());
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
                                    emit send_update_userinfo(q);//发送更新信号给主窗口，下一次查看用户信息即更新借书信息
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
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书是馆藏精品，只限馆内阅读。")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
                }
            }
        }


        else if(ui->comboBox->currentIndex()==2)//按书号查询
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书号='%1';")\
                    .arg(ui->lineEdit->text());
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
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n是否要借阅此书？")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                    QString bookname=q.value(3).toString();//书名之后有用
                    if(i==0)//确认借书
                    {
                        QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书号='%3';")\
                                .arg("0").arg(u.value(1).toString()).arg(ui->lineEdit->text());
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
                                emit send_update_userinfo(q);

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
                        QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书已经变成蝴蝶飞走了。")\
                                .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                                .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                        QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
                    }
                    else//借书人是持有者，可以还书
                    {
                        QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n你是该书持有者，是否还书？")\
                                .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                                .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                        QString bookname=q.value(3).toString();//书名之后有用
                        int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                        if(i==0)//确认还书
                        {
                            QString sqlcmd=QString("UPDATE BookInfoTable SET 是否在馆=%1,借书人='%2' WHERE 书名='%3';")\
                                    .arg("1").arg("").arg(bookname);
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
                                    emit send_update_userinfo(q);//发送更新信号给主窗口，下一次查看用户信息即更新借书信息
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
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书是馆藏精品，只限馆内阅读。")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    QMessageBox::information(this,"提示！",bookinfo,tr("确定"));
                }
            }
        }
    }
    else
    {
        QMessageBox::critical(this,"警告！","没有输入查询参数或选择类型");
    }
}



void Searchbook::book_delete()//删除界面
{
    setAttribute(Qt::WA_DeleteOnClose);//关闭释放
    setWindowTitle("图书删除");
    ui->lineEdit->setEnabled(false);
    ui->s_b->setText("删除");
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(valueChanged(int)));
    //disconnect(ui->s_b,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->s_b,SIGNAL(clicked()),this,SLOT(delete_operation()));
}

void Searchbook::delete_operation()//删除操作  在馆才能删除，不在馆不能删除
{
    if(ui->lineEdit->isEnabled()&&(!ui->lineEdit->text().isEmpty()))//输入有值且可用
    {
        if(ui->comboBox->currentIndex()==1)//按书名查询
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书名='%1';")\
                    .arg(ui->lineEdit->text());
            //搜索
            q.exec(sqlcmd);
            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
            if(q.value(0).toString().isEmpty())//如果为空没找到
            {
                QMessageBox::information(this,"提示!","没有找到此书");
            }
            else//找到
            {
                if(q.value(6).toString()=="1")//在馆
                {
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n是否要删除此书？")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                    if(i==0)//确认
                    {
                        QString sqlcmd=QString("DELETE * FROM BookInfoTable WHERE 书名='%1';")\
                                .arg(ui->lineEdit->text());
                        //修改书籍信息
                        q.exec(sqlcmd);
                        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                        if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                        {//更新用户借书的信息

                                QMessageBox::information(this,"提示！","删除成功");
                                emit send_update_userinfo(u);//发送更新信号给主窗口，下一次查看用户信息即更新书信息

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

                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书不在馆，无法删除此书!")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                    QMessageBox::information(this,"提示！",bookinfo,tr("确定"));

                }
            }
        }


        else if(ui->comboBox->currentIndex()==2)//按书号查询
        {
            QSqlQuery q;//开始查询数据库
            QString sqlcmd=QString("SELECT * FROM BookInfoTable where 书号='%1';")\
                    .arg(ui->lineEdit->text());
            //搜索
            q.exec(sqlcmd);
            q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
            if(q.value(0).toString().isEmpty())//如果为空没找到
            {
                QMessageBox::information(this,"提示!","没有找到此书");
            }
            else//找到
            {
                if(q.value(6).toString()=="1")//在馆
                {
                    QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n是否要删除此书？")\
                            .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                            .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("是").arg(q.value(7).toString());
                    int i=QMessageBox::information(this,"提示！",bookinfo,tr("确定"),tr("取消"),0);
                    QString bookname=q.value(3).toString();//书名之后有用
                    if(i==0)//确认
                    {
                        QString sqlcmd=QString("DELETE * FROM BookInfoTable WHERE 书号='%1';")\
                                .arg(ui->lineEdit->text());
                        //修改书籍信息
                        q.exec(sqlcmd);
                        q.next();//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
                        if(q.lastError().text().isEmpty())//无错误则说明书本信息操作成功
                        {//更新用户借书的信息

                                QMessageBox::information(this,"提示！","删除成功");
                                emit send_update_userinfo(u);
                        }

                     }}
                else if(q.value(6).toString()=="0")//不在馆
                {

                        QString bookinfo=QString("找到此书！\n书号：%1\n类型：%2\n书名：%3\n作者：%4\n出版社：%5\n是否在馆：%6\n借书人：%7\n该书不在馆，无法删除此书!")\
                                .arg(q.value(1).toString()).arg(q.value(2).toString()).arg(q.value(3).toString())\
                                .arg(q.value(4).toString()).arg(q.value(5).toString()).arg("否").arg(q.value(7).toString());
                        QMessageBox::information(this,"提示！",bookinfo,tr("确定"));


                }
            }
         }}
    else
    {
        QMessageBox::critical(this,"警告！","没有输入查询参数或选择类型");
    }
}
