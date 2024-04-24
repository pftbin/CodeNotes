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
#include <QCheckBox>

#define  DF_MAX_COL     (1024)
#define  DF_MAX_ROW     (0xFFFFFFFF)
#define  CELL_WIDTH     (100)
#define  CELL_HEIGHT    (25)

class QMyTableView : public QTableView
{
    Q_OBJECT

public:
    QMyTableView(QWidget *parent = nullptr);
    void EnableMultiSelect(bool bEnable);

    int getColCount(){return model->columnCount();};
    int getRowCount(){return model->rowCount();}

    int addCol(const QString itemText, int colWidth = CELL_WIDTH);
    QString getColText(int colNum);

    int addRow();
    int delRow(int rowNum);
    int delAllRow();
    int delAllRowCol();

    void setItemText(int rowNum, int colNum, const QString itemText);
    QString getItemText(int rowNum, int colNum);

    void setItemChecked(int rowNum, int colNum, bool checked);
    bool getItemChecked(int rowNum, int colNum);

    void setItemEdit(int rowNum, int colNum, const QString itemText = "");
    void setItemButton(int rowNum, int colNum, const QString itemText = "");
    void setItemComboBox(int rowNum, int colNum, const QStringList itemTextList, const QString selItemText);
    void setItemCheckBox(int rowNum, int colNum, const QString itemText, const bool checked = false);

    void enableAllWidget(bool bEnable);
    void enableRowWidget(int rowNum, bool bEnable);
    void enableItemWidget(int rowNum, int colNum, bool bEnable);

signals:
    void lostFocus();
    void buttonClicked(int rowNum, int colNum);
    void checkBoxClicked(int row, int column, bool checked);

public slots:
    void focusOutEvent(QFocusEvent *event) override;
    void handlePushButtonClicked();
    void handleLineEditTextChanged(const QString& text);
    void handleLineEditEditingFinished();
    void handleComboBoxCurrentIndexChanged(int index);
    void handleCheckBoxCurrentIndexChanged(bool checked);

protected:
    int     m_colCount;
    int     m_rowCount;

    int     m_rowHight;
    int     m_colWidthArray[DF_MAX_COL];

private:
    QStandardItemModel *model;
};




#endif // MYTABLEVIEW_H
