#include "mainclass.h"

MainClass::MainClass(QString configFileName, QObject *parent) : QObject(parent)
{
    cfg = readCFG(configFileName);

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("TIME2SOCK"), QWebSocketServer::NonSecureMode,  this);

    if(m_pWebSocketServer->listen(QHostAddress::Any, cfg.websocketPort))
    {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &MainClass::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed,
                this, &QCoreApplication::quit);
    }
}

void MainClass::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::disconnected, this, &MainClass::socketDisconnected);
    m_clients << pSocket;

    ProcessThread *prThread = new ProcessThread(pSocket);
    if(!prThread->isRunning())
    {
        prThread->start();
        connect(pSocket, &QWebSocket::disconnected, prThread, &ProcessThread::quit);
        connect(prThread, &ProcessThread::finished, prThread, &ProcessThread::deleteLater);
        connect(prThread, &ProcessThread::finished, this, &MainClass::socketDisconnected);
    }
}

void MainClass::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

    if(pClient){
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}





ProcessThread::ProcessThread(QWebSocket *socket, QWidget *parent)
{
    pSocket = socket;

    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(doRepeatWork()));
    timer->start(1000);
}

ProcessThread::~ProcessThread()
{
}

void ProcessThread::doRepeatWork()
{
    QDateTime systemTimeUTC = QDateTime::currentDateTimeUtc();
    pSocket->sendTextMessage(QString::number(systemTimeUTC.toTime_t()));
}
