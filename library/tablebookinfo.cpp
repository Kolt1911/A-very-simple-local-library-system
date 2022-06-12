#include "tablebookinfo.h"
#include "ui_tablebookinfo.h"
#include<QTableView>
#include<headers.h>

Tablebookinfo::Tablebookinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tablebookinfo)
{
    ui->setupUi(this);
    theModel=new QStandardItemModel(2,8,this);//数据模型
    theSelection=new QItemSelectionModel(theModel);//选择模型
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));
    ui->tableView->setModel (theModel) ; //设置数据模型
    ui->tableView-> setSelectionModel(theSelection) ; //设置选择模型
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//只单行选择
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//只选择一行

    QStringList tableheaders;
    tableheaders<<"ID"<<"书号"<<"类型"<<"书名"<<"作者"<<"出版社"<<"是否在馆"<<"借书人";
    theModel->setHorizontalHeaderLabels(tableheaders); //设置表头文字

    QSqlQuery q;//开始查询数据库
    QString sqlcmd="SELECT count(*) FROM BookInfoTable";//查询总行数
    q.exec(sqlcmd);
    q.next();
    theModel->setRowCount(q.value(0).toInt()); //实际数据行数

    sqlcmd=QString("SELECT  * FROM BookInfoTable ORDER BY ID;");//查询所有书
    q.exec(sqlcmd);

    int i=1,j=0;
    QStandardItem   *aItem;

    while (q.next())//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
    {
        for(j=0;j<8;j++)
        {
            if(j%8==6)//是否在馆要改格式
            {
                if(q.value(j).toString()=="0")
                {//不在馆
                    aItem=new QStandardItem("否");//创建item
                    theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                }
                else if(q.value(j).toString()=="1")
                {//在馆
                    aItem=new QStandardItem("是");//创建item
                    theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                }
            }
            else
            {
                aItem=new QStandardItem(q.value(j).toString());//创建item
                theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
            }
        }
        ++i;//行数+1
    }
}

Tablebookinfo::~Tablebookinfo()
{
    delete ui;
}

void Tablebookinfo::on_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{ //选择单元格变化时的响应
    if (current.isValid())
    {
        QStandardItem* aItem=theModel->itemFromIndex(current);
        ui->label->setText (QString("当前单元格：%1行，%2列。单元格内容：%3").arg(current.row()).arg(current.column()).arg(aItem->text()));

        QModelIndex bID=theModel->index(current.row(),1);

        emit selected_ID(theModel->data(bID).toString());

    }

}

bool Tablebookinfo::author_books(QString t)//显示一个作者的书目
{
    //ui->setupUi(this);
    theModel=new QStandardItemModel(2,8,this);//数据模型
    theSelection=new QItemSelectionModel(theModel);//选择模型
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));
    ui->tableView->setModel (theModel) ; //设置数据模型
    ui->tableView-> setSelectionModel(theSelection) ; //设置选择模型
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//只单行选择
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//只选择一行

    QStringList tableheaders;
    tableheaders<<"ID"<<"书号"<<"类型"<<"书名"<<"作者"<<"出版社"<<"是否在馆"<<"借书人";
    theModel->setHorizontalHeaderLabels(tableheaders); //设置表头文字

    QSqlQuery q;//开始查询数据库
    QString sqlcmd="SELECT count(*) FROM BookInfoTable";//查询总行数
    q.exec(sqlcmd);
    q.next();
    theModel->setRowCount(q.value(0).toInt()); //实际数据行数

    sqlcmd=QString("SELECT  * FROM BookInfoTable WHERE 作者='%1' ORDER BY ID;").arg(t);//查询所有书
    q.exec(sqlcmd);

    int i=1,j=0;
    QStandardItem   *aItem;
    if(!q.next())
    {
         QMessageBox::information(this, "提示！","书库中没有该作者相关的书",QMessageBox::Ok,QMessageBox::NoButton);
         return false;
    }
    q.last();
    while (q.next())//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
    {
            for(j=0;j<8;j++)
            {
                if(j%8==6)//是否在馆要改格式
                {
                    if(q.value(j).toString()=="0")
                    {//不在馆
                        aItem=new QStandardItem("否");//创建item
                        theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                    }
                    else if(q.value(j).toString()=="1")
                    {//在馆
                        aItem=new QStandardItem("是");//创建item
                        theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                    }
                }
                else
                {
                    aItem=new QStandardItem(q.value(j).toString());//创建item
                    theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                }
            }
            ++i;//行数+1


    }
    return true;
}

void Tablebookinfo::showallusers()
{
    theModel=new QStandardItemModel(2,5,this);//数据模型
    theSelection=new QItemSelectionModel(theModel);//选择模型
    connect(theSelection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));
    ui->tableView->setModel (theModel) ; //设置数据模型
    ui->tableView-> setSelectionModel(theSelection) ; //设置选择模型
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);//只单行选择
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//只选择一行

    QStringList tableheaders;
    tableheaders<<"ID"<<"用户名"<<"用户类型"<<"用户密码"<<"借阅信息";
    theModel->setHorizontalHeaderLabels(tableheaders); //设置表头文字

    QSqlQuery q;//开始查询数据库
    QString sqlcmd="SELECT count(*) FROM UserInfoTable";//查询总行数
    q.exec(sqlcmd);
    q.next();
    theModel->setRowCount(q.value(0).toInt()); //实际数据行数

    sqlcmd=QString("SELECT * FROM UserInfoTable ORDER BY ID;");//查询所有用户
    q.exec(sqlcmd);

    int i=1,j=0;
    QStandardItem   *aItem;

    while (q.next())//获得数据集后，必须执行next()或first()到第一条记录，这时候record才是有效的。
    {
        for(j=0;j<5;j++)
        {
            if(j%5==2)//是否在馆要改格式
            {
                if(q.value(j).toString()=="0")
                {//不在馆
                    aItem=new QStandardItem("普通用户");//创建item
                    theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                }
                else if(q.value(j).toString()=="1")
                {//在馆
                    aItem=new QStandardItem("管理员");//创建item
                    theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
                }
            }
            else
            {
                aItem=new QStandardItem(q.value(j).toString());//创建item
                theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
            }
        }
        ++i;//行数+1
    }
}
