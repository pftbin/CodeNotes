#include "QMyTableView.h"
#include <QDebug>

QMyTableView::QMyTableView(QWidget *parent)
    : QTableView(parent)
{
    Qt::Alignment alignment = Qt::AlignCenter; //居中对齐
    QHeaderView *pVerticalHeader = verticalHeader();
    pVerticalHeader->setDefaultAlignment(alignment);
    pVerticalHeader->setVisible(false);
    QHeaderView *pHorizontalHeader = horizontalHeader();
    pHorizontalHeader->setVisible(true);
    pHorizontalHeader->setDefaultAlignment(alignment);

    //创建模型
    model = new QStandardItemModel(this);  
    setModel(model);
    setEditTriggers(QAbstractItemView::NoEditTriggers);  // 设置表格不可编辑
    setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    EnableMultiSelect(false);

    m_rowHight = CELL_HEIGHT;
    memset(m_colWidthArray,0,sizeof(int)*DF_MAX_COL);
}
void QMyTableView::EnableMultiSelect(bool bEnable)
{
    if (bEnable)
        setSelectionMode(QAbstractItemView::MultiSelection);// 多行选中
    else
        setSelectionMode(QAbstractItemView::SingleSelection);// 单行选中
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

    //修复无行数据时列宽不生效问题
    if (rowCount == 0)
    {
        addRow();
        for(int i = 0; i < model->columnCount(); i++)
        {
            setItemText(0,i,QString(""));
        }
        delRow(0);
    }
    //添加已有行
    for(int j = 0; j < rowCount; j++)
    {
        model->setItem(j, colCount, new QStandardItem(""));
    }

    //记录当前列数
    m_colCount = model->columnCount();
    return m_colCount;
}
QString QMyTableView::getColText(int colNum)
{
    if (colNum >= m_colCount)
        return QString();

    QVariant headerData = model->headerData(colNum, Qt::Horizontal);
    QString strColName = headerData.toString();

    return strColName;
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
        QStandardItem *item = new QStandardItem(QString(""));
        item->setTextAlignment(Qt::AlignCenter); //居中对齐
        model->setItem(rowCount, i, item);
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

    QStandardItem *item = new QStandardItem(itemText);
    item->setTextAlignment(Qt::AlignCenter); //居中对齐
    model->setItem(rowNum, colNum, item);

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

        QCheckBox *pCheckBox = qobject_cast<QCheckBox*>(pWidget);
        if (pCheckBox) {
            pCheckBox->setText(itemText);
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

        QCheckBox *pCheckBox = qobject_cast<QCheckBox*>(pWidget);
        if (pCheckBox) {
            itemText = pCheckBox->text();
        }
    }

    return itemText;
}

void QMyTableView::setItemChecked(int rowNum, int colNum, bool checked)
{
    QWidget *pWidget = indexWidget(model->index(rowNum, colNum));
    if (pWidget != nullptr)
    {
        QCheckBox *pCheckBox = qobject_cast<QCheckBox*>(pWidget);
        if (pCheckBox) {
            pCheckBox->setChecked(checked);
        }
    }
}
bool QMyTableView::getItemChecked(int rowNum, int colNum)
{
    bool checked = false;
    QWidget *pWidget = indexWidget(model->index(rowNum, colNum));
    if (pWidget != nullptr)
    {
        QCheckBox *pCheckBox = qobject_cast<QCheckBox*>(pWidget);
        if (pCheckBox) {
            checked = pCheckBox->isChecked();
        }
    }

    return checked;
}

void QMyTableView::setItemEdit(int rowNum, int colNum, const QString itemText)
{
    //创建控件
    QLineEdit *pEdit = new QLineEdit(itemText, this);
    pEdit->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    //插入到指定单元格
    setIndexWidget(model->index(rowNum, colNum), pEdit);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //添加槽关联
    QObject::connect(pEdit, (&QLineEdit::textChanged), this, (&QMyTableView::handleLineEditTextChanged));
    QObject::connect(pEdit, (&QLineEdit::editingFinished), this, (&QMyTableView::handleLineEditEditingFinished));
}
void QMyTableView::setItemButton(int rowNum, int colNum, const QString itemText)
{
    //创建控件
    QPushButton *pButton = new QPushButton(itemText, this);
    pButton->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    //将插入到指定单元格
    setIndexWidget(model->index(rowNum, colNum), pButton);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //连接按钮的点击信号到槽函数
    QObject::connect(pButton, (&QPushButton::clicked), this, (&QMyTableView::handlePushButtonClicked));
}
void QMyTableView::setItemComboBox(int rowNum, int colNum, const QStringList itemTextList, const QString selItemText)
{
    //创建控件
    QComboBox *pComboBox = new QComboBox(this);
    pComboBox->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);

    int selItemIdx = -1;
    int itemCount = itemTextList.count();
    for(int i = 0; i < itemCount; i++)
    {
        QString text = itemTextList.at(i);
        pComboBox->addItem(text);
        if (selItemText == text)
        {
            selItemIdx = i;
        }
    }

    //插入到指定单元格
    pComboBox->setCurrentIndex(selItemIdx);
    setIndexWidget(model->index(rowNum, colNum), pComboBox);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);
}
void QMyTableView::setItemCheckBox(int rowNum, int colNum, const QString itemText, const bool checked)
{
    //创建控件
    QCheckBox *pCheckBox = new QCheckBox(this);
    pCheckBox->setBaseSize(columnWidth(colNum)-1,rowHeight(rowNum)-1);
    pCheckBox->setText(itemText);
    pCheckBox->setChecked(checked);

    //插入到指定单元格
    setIndexWidget(model->index(rowNum, colNum), pCheckBox);
    setRowHeight(rowNum,m_rowHight);
    setColumnWidth(colNum,m_colWidthArray[colNum]);

    //连接勾选框的点击信号到槽函数
    QObject::connect(pCheckBox, (&QCheckBox::clicked), this, (&QMyTableView::handleCheckBoxCurrentIndexChanged));
}

void QMyTableView::enableAllWidget(bool bEnable)
{
    for (int i = 0; i < m_rowCount; i++)
    {
        for (int j = 0; j < m_colCount; j++)
        {
            enableItemWidget(i, j, bEnable);
        }
    }
}
void QMyTableView::enableRowWidget(int rowNum, bool bEnable)
{
    if (rowNum >= m_rowCount) return;

    for (int j = 0; j < m_colCount; j++)
    {
        enableItemWidget(rowNum, j, bEnable);
    }
}
void QMyTableView::enableItemWidget(int rowNum, int colNum, bool bEnable)
{
    if (rowNum >= m_rowCount) return;
    if (colNum >= m_colCount) return;

    QWidget *pWidget = indexWidget(model->index(rowNum, colNum));
    if (pWidget != nullptr)
    {
        pWidget->setEnabled(bEnable);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
void QMyTableView::focusOutEvent(QFocusEvent *event)
{
    //调用基类的 focusOutEvent 函数以确保正确处理焦点事件
    QTableView::focusOutEvent(event);

    emit lostFocus(); //发出自定义信号，表示控件失去焦点
}
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
void QMyTableView::handleCheckBoxCurrentIndexChanged(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    if (checkBox)
    {
        int x = checkBox->mapToParent(QPoint(0,0)).x();
        int y = checkBox->mapToParent(QPoint(0,0)).y();
        QModelIndex index = this->indexAt(QPoint(x,y));
        int row = index.row();
        int col = index.column();

        emit checkBoxClicked(row,col,checked);
    }
}
