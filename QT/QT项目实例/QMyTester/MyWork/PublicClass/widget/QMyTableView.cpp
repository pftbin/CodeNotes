#include "QMyTableView.h"
#include <QDebug>

QMyTableView::QMyTableView(QWidget *parent)
    : QTableView(parent)
{
    QHeaderView *pHorizontalHeader = horizontalHeader();
    pHorizontalHeader->setVisible(true);

    QHeaderView *pVerticalHeader = verticalHeader();
    pVerticalHeader->setVisible(false);

    // 创建模型
    model = new QStandardItemModel(this);
    setModel(model);
    setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置表格不可编辑
    setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    setSelectionMode(QAbstractItemView::SingleSelection);// 单行选中

    m_rowHight = CELL_HEIGHT;
    memset(m_colWidthArray,0,sizeof(int)*DF_MAX_COL);
}

int QMyTableView::addCol(const QString itemText, int colWidth)
{
    int colCount = model->columnCount();
    int rowCount = model->rowCount();
    if (colCount >= DF_MAX_COL) return -1;
    if (rowCount >= DF_MAX_ROW) return -1;

    //记录列宽
    colWidth = fmax(CELL_WIDTH,colWidth);
    m_colWidthArray[colCount] = colWidth;

    //添加表头
    model->setHorizontalHeaderItem(colCount, new QStandardItem(itemText));
    verticalHeader()->resizeSection(colCount, colWidth);

    //添加已有行
    for(int j = 0; j < rowCount; j++)
    {
        model->setItem(j, colCount, new QStandardItem(""));
    }

    //记录当前列数
    m_colCount = model->columnCount();
    return m_colCount;
}

int QMyTableView::addRow()
{
    int colCount = model->columnCount();
    int rowCount = model->rowCount();
    if (colCount >= DF_MAX_COL) return -1;
    if (rowCount >= DF_MAX_ROW) return -1;

    //添加一行
    for(int i = 0; i < colCount; i++)
    {
        model->setItem(rowCount, i, new QStandardItem(""));
    }

    //记录当前行数
    m_rowCount = model->rowCount();
    return m_rowCount;
}

int QMyTableView::delRow(int rowNum)
{
    int rowCount = model->rowCount();
    if (rowNum >= rowCount) return -1;

    model->removeRow(rowNum);

    //记录当前行数
    m_rowCount = model->rowCount();
    return m_rowCount;
}

int QMyTableView::delAllRow()
{
    int rowCount = model->rowCount();
    if (rowCount <= 0) return -1;

    for(int i = 0; i < rowCount; i++)
    {
        model->removeRow(i);
    }

    //更新行数
    model->setRowCount(0);
    m_rowCount = model->rowCount();
    return m_rowCount;
}

int QMyTableView::delAllRowCol()
{
    model = new QStandardItemModel(this);
    setModel(model);

    m_colCount = 0;
    m_rowCount = 0;

    return 0;
}

void QMyTableView::setItemText(int rowNum, int colNum, const QString itemText)
{
    int colCount = model->columnCount();
    int rowCount = model->rowCount();
    if (rowNum >= rowCount) return;
    if (colNum >= colCount) return;

    model->setItem(rowNum, colNum, new QStandardItem(itemText));
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //指定行列控件文本
    QWidget *pWidget = indexWidget(model->index(rowNum, colNum));
    if(pWidget != nullptr)
    {
        QLineEdit *pEdit = qobject_cast<QLineEdit*>(pWidget);
        if (pEdit) {
            pEdit->setText(itemText);
        }

        QPushButton *pButton = qobject_cast<QPushButton*>(pWidget);
        if (pButton) {
            pButton->setText(itemText);
        }

        QComboBox *pCombox = qobject_cast<QComboBox*>(pWidget);
        if (pCombox) {
            pCombox->setCurrentText(itemText);
        }
    }
}

QString QMyTableView::getItemText(int rowNum, int colNum)
{
    QString itemText = "";
    QWidget *pWidget = indexWidget(model->index(rowNum, colNum));
    if (pWidget == nullptr)
    {
        QVariant data = model->data(model->index(rowNum, colNum));
        itemText = data.toString();
    }
    else
    {
        QLineEdit *pEdit = qobject_cast<QLineEdit*>(pWidget);
        if (pEdit) {
            itemText = pEdit->text();
        }

        QPushButton *pButton = qobject_cast<QPushButton*>(pWidget);
        if (pButton) {
            itemText = pButton->text();
        }

        QComboBox *pCombox = qobject_cast<QComboBox*>(pWidget);
        if (pCombox) {
            itemText = pCombox->currentText();
        }
    }

    return itemText;
}



void QMyTableView::setItemEdit(int rowNum, int colNum, const QString itemText)
{
    //创建控件
    QLineEdit *edit = new QLineEdit(itemText, this);
    edit->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    //插入到指定单元格
    setIndexWidget(model->index(rowNum, colNum), edit);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //添加槽关联
    QObject::connect(edit, (&QLineEdit::textChanged), this, (&QMyTableView::handleLineEditTextChanged));
    QObject::connect(edit, (&QLineEdit::editingFinished), this, (&QMyTableView::handleLineEditEditingFinished));
}

void QMyTableView::setItemButton(int rowNum, int colNum, const QString itemText)
{
    //创建控件
    QPushButton *button = new QPushButton(itemText, this);
    button->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    //将插入到指定单元格
    setIndexWidget(model->index(rowNum, colNum), button);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //连接按钮的点击信号到槽函数
    QObject::connect(button, (&QPushButton::clicked), this, (&QMyTableView::handlePushButtonClicked));
}

void QMyTableView::setItemCombox(int rowNum, int colNum, const QStringList itemTextList)
{
    //创建控件
    QComboBox *comboBox = new QComboBox(this);
    comboBox->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    int itemCount = itemTextList.count();
    for(int i = 0; i < itemCount; i++)
    {
        QString text = itemTextList.at(i);
        comboBox->addItem(text);
    }

    //插入到指定单元格
    comboBox->setCurrentText(itemTextList.at(0));
    setIndexWidget(model->index(rowNum, colNum), comboBox);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //添加槽关联
    //QObject::connect(comboBox, (&QComboBox::currentIndexChanged), this, (&QMyTableView::handleComboBoxCurrentIndexChanged));
}

/////////////////////////////////////////////////////////////////////////////////////////////

void QMyTableView::handlePushButtonClicked()
{
    QPushButton *pushButton = qobject_cast<QPushButton *>(sender());
    if (pushButton)
    {
        int x = pushButton->mapToParent(QPoint(0,0)).x();
        int y = pushButton->mapToParent(QPoint(0,0)).y();
        QModelIndex index = this->indexAt(QPoint(x,y));
        int row = index.row();
        int col = index.column();

        emit buttonClicked(row,col);
    }
}

void QMyTableView::handleLineEditTextChanged(const QString& text)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (lineEdit)
    {
        int x = lineEdit->mapToParent(QPoint(0,0)).x();
        int y = lineEdit->mapToParent(QPoint(0,0)).y();
        QModelIndex index = this->indexAt(QPoint(x,y));
        int row = index.row();
        int col = index.column();

        model->setItem(row, col, new QStandardItem(text));
    }
}

void QMyTableView::handleLineEditEditingFinished()
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
    if (lineEdit)
    {
        int x = lineEdit->mapToParent(QPoint(0,0)).x();
        int y = lineEdit->mapToParent(QPoint(0,0)).y();
        QModelIndex index = this->indexAt(QPoint(x,y));
        int row = index.row();
        int col = index.column();

        QString lineEditText = lineEdit->text();
        model->setItem(row, col, new QStandardItem(lineEditText));
    }
}

void QMyTableView::handleComboBoxCurrentIndexChanged(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    if (comboBox)
    {
        int x = comboBox->mapToParent(QPoint(0,0)).x();
        int y = comboBox->mapToParent(QPoint(0,0)).y();
        QModelIndex index = this->indexAt(QPoint(x,y));
        int row = index.row();
        int col = index.column();

        QString comboBoxText = comboBox->currentText();
        model->setItem(row, col, new QStandardItem(comboBoxText));
    }
}
