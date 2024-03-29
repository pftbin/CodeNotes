#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QApplication>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

#define  DF_MAX_COL     (1024)
#define  DF_MAX_ROW     (0xFFFFFFFF)
#define  CELL_WIDTH     (80)
#define  CELL_HEIGHT    (25)

class QMyTableView : public QTableView
{
    Q_OBJECT

public:
    QMyTableView(QWidget *parent = nullptr);

    int getColCount(){return model->columnCount();};
    int getrowCount(){return model->rowCount();}

    int addCol(const QString itemText, int colWidth = CELL_WIDTH);
    QString getColText(int colNum);

    int addRow();
    int delRow(int rowNum);
    int delAllRow();
    int delAllRowCol();

    void setItemText(int rowNum, int colNum, const QString itemText);
    QString getItemText(int rowNum, int colNum);

    void setItemEdit(int rowNum, int colNum, const QString itemText = "");
    void setItemButton(int rowNum, int colNum, const QString itemText = "");
    void setItemCombox(int rowNum, int colNum, const QStringList itemTextList);

signals:
    void buttonClicked(int rowNum, int colNum);

public slots:
    void handlePushButtonClicked();
    void handleLineEditTextChanged(const QString& text);
    void handleLineEditEditingFinished();
    void handleComboBoxCurrentIndexChanged(int index);

protected:
    int     m_colCount;
    int     m_rowCount;

    int     m_rowHight;
    int     m_colWidthArray[DF_MAX_COL];

private:
    QStandardItemModel *model;
};




#endif // MYTABLEVIEW_H
