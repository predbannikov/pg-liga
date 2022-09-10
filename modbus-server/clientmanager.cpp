#include "clientmanager.h"

ClientManager::ClientManager(QObject *parent) : QObject(parent)
{

}

ClientManager::~ClientManager()
{
    qDebug() << Q_FUNC_INFO;
}

void ClientManager::addClient(qintptr id, SocketThread *sock_thr)
{
    connect(sock_thr, &SocketThread::getResponse, this, &ClientManager::sendResponse);
    clients.insert(id, sock_thr);
}

//void ClientManager::run()
//{
//    qDebug() << "run ClientManager" << QThread::currentThreadId();
//    while(!abort) {
//        QMutexLocker lock(&mutex);
//        while (responses.empty()) {
//            notEmpty.wait(&mutex);
//        }

//        QJsonObject jfront(responses.takeFirst());
//        if (jfront["status"] == "") {
//            abort = true;
//            continue;
//        }
//        clients[jfront["id"].toInt()]->put(jfront);
//    }
//    exec();
//}

void ClientManager::put(QJsonObject jobj)
{
//    QMutexLocker lock(&mutex);
    clients[jobj["id"].toInt()]->put(jobj);

//    responses.push_back(std::move(jobj));

//    notEmpty.notify_one();
}
