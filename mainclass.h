#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QtWebSockets/QtWebSockets>
#include <QTimer>

#include "KGEEWLIBS_global.h"
#include "kgeewlibs.h"

#define TIME2WSOCK_VERSION 0.1

class MainClass : public QObject
{
    Q_OBJECT
public:
    explicit MainClass(QString conFile = nullptr, QObject *parent = nullptr);

private:
    _CONFIGURE cfg;

    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;

private slots:
    void onNewConnection();
    void socketDisconnected();
};


class ProcessThread : public QThread
{
    Q_OBJECT
public:
    ProcessThread(QWebSocket *websocket = nullptr, QWidget *parent = nullptr);
    ~ProcessThread();

private slots:
    void doRepeatWork();

private:
    QWebSocket *pSocket;
};


#endif // MAINCLASS_H
