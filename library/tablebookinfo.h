#ifndef TABLEINFO_H
#define TABLEINFO_H

#include <QWidget>
#include<QStandardItemModel>
#include<QItemSelectionModel>
#include<QLabel>

namespace Ui {
class Tablebookinfo;
}

class Tablebookinfo : public QWidget
{
    Q_OBJECT private:
    QLabel *LabCellPos;    //当前单元格行列号
    QLabel *LabCellText; //当前单元格内容
    QStandardItemModel * theModel; //数据模型
    QItemSelectionModel *theSelection; //选择模型

public:
    explicit Tablebookinfo(QWidget *parent = nullptr);
    ~Tablebookinfo();

    bool author_books(QString t);
    void showallusers();
private slots:
        void on_currentChanged(const QModelIndex &current, const QModelIndex &previous);
        //用于在 TableView 上选择单元格发生变化时，更新状态栏的信息显示，这个槽函数将会与项选择模型 theSelection 的 currentChanged() 信号关联。
signals:
        void selected_ID(QString id);

private:
    Ui::Tablebookinfo *ui;
};

#endif // TABLEINFO_H
