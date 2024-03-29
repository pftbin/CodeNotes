#ifndef QHTTPTESTER_H
#define QHTTPTESTER_H

#include <QLabel>
#include <QMainWindow>
#include "HttpHelper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QHttpTester : public QMainWindow
{
    Q_OBJECT

public:
    QHttpTester(QWidget *parent = nullptr);
    ~QHttpTester();

protected:
    void InitCtrl();

    HttpHelper m_httpHelper;
    QString    m_strHttpUrl;
    QString    m_strHttpType;

private slots:
    void on_timerUpdateUI();
    void on_m_cmbHttpType_activated(int index);
    void on_m_btnRequest_clicked();
    void on_m_btnDownload_clicked();

private:
    Ui::MainWindow      *ui;
    QTimer              *m_timerUpdateUI;

    QLabel              *m_statebar_message;
    QLabel              *m_statebar_time;

};
#endif // QHTTPTESTER_H
