#ifndef QJSONXMLTESTER_H
#define QJSONXMLTESTER_H

#include <QMainWindow>
#include "JsonHelper.h"
#include "XmlHelper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QJsonXmlTester : public QMainWindow
{
    Q_OBJECT

public:
    QJsonXmlTester(QWidget *parent = nullptr);
    ~QJsonXmlTester();

protected:
    JsonHelper  m_jsonHelper;
    XmlHelper   m_xmlHelper;


    QJsonObject CreateRandomJson(bool isRoot = true);
    QDomDocument CreateRandomXml(bool isRoot = true);

private slots:
    void on_m_btnCreateJson_clicked();

    void on_m_btnCreateXml_clicked();

    void on_m_btnSaveJson_clicked();

    void on_m_btnSaveXml_clicked();

    void on_m_btnJson2Xml_clicked();

    void on_m_btnXml2Json_clicked();

    void on_m_btnParseJson_clicked();

    void on_m_btnParseXml_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // QJSONXMLTESTER_H
