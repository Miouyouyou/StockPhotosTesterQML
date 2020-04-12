#include "myywebsocket.h"

#include "QtWebSockets/QWebSocket"

#include <QFile> // TODO remove

#define LOG(fmt, ...) fprintf(stderr, "[C++] " fmt "\n", ##__VA_ARGS__)

MyyWebSocket::MyyWebSocket(QObject *parent)
    : QObject(parent)
    , server("StockPhotosSelector", QWebSocketServer::SslMode::NonSecureMode)
{
    connect(&server,
            &QWebSocketServer::acceptError,
            this,
            &MyyWebSocket::acceptError);
    connect(&server,
            &QWebSocketServer::closed,
            this,
            &MyyWebSocket::ws_server_closed);
    connect(&server,
            &QWebSocketServer::newConnection,
            this,
            &MyyWebSocket::ws_client_ringing);
    connect(&server,
            &QWebSocketServer::originAuthenticationRequired,
            this,
            &MyyWebSocket::originAuthenticationRequired);
    connect(&server,
            &QWebSocketServer::peerVerifyError,
            this,
            &MyyWebSocket::peerVerifyError);
    connect(&server,
            &QWebSocketServer::preSharedKeyAuthenticationRequired,
            this,
            &MyyWebSocket::preSharedKeyAuthenticationRequired);
    connect(&server,
            &QWebSocketServer::serverError,
            this,
            &MyyWebSocket::serverError);
    connect(&server,
            &QWebSocketServer::sslErrors,
            this,
            &MyyWebSocket::sslErrors);
}

void MyyWebSocket::listen() {
    QHostAddress address(bind_address);
    LOG("Starting to listen : %d\n", server.listen(address, bind_port));
}


QString MyyWebSocket::bindAddress() {
    return bind_address;
}

void MyyWebSocket::bindAddressSet(QString const &newAddress) {
    bind_address = newAddress;
}

/* Port */
int MyyWebSocket::bindPort() {
    return bind_port;
}

void MyyWebSocket::bindPortSet(int newPort) {
    bind_port = newPort;
}

void MyyWebSocket::ws_client_ringing() {
    QWebSocket * incoming_client =
        server.nextPendingConnection();
    connect(incoming_client,
            &QWebSocket::textMessageReceived,
            this,
            &MyyWebSocket::ws_client_sending_text_data);
    connect(incoming_client,
            &QWebSocket::binaryMessageReceived,
            this,
            &MyyWebSocket::ws_client_sending_bin_data);
    connect(incoming_client,
            &QWebSocket::disconnected,
            this,
            &MyyWebSocket::ws_client_left);
    clients.append(incoming_client);
    LOG("Incoming client '%s' from %s:%d",
        incoming_client->peerName().toUtf8().data(),
        incoming_client->peerAddress().toString().toUtf8().data(),
        incoming_client->peerPort());
}

void MyyWebSocket::ws_client_left() {
    QWebSocket * const disconnected_client =
        qobject_cast<QWebSocket *>(sender());
    LOG("Client [%s] %s:%d disconnected",
        disconnected_client->peerName().toUtf8().data(),
        disconnected_client->peerAddress().toString().toUtf8().data(),
        disconnected_client->peerPort());
    clients.removeOne(disconnected_client);
    delete disconnected_client;

}

void MyyWebSocket::ws_server_closed() {
    LOG("Server closed :C");
}

void MyyWebSocket::ws_error_handler(
    ErrorType error,
    void * impl_data)
{
    switch(error) {
    case Invalid:
    {
        LOG("Invalid error type");
    }
    break;
    case ServerGeneric:
    {
        LOG("Server generic error");
    }
    break;
    case AuthRequired:
    {
        LOG("Auth Required");
    }
    break;
    case CantAccept: {
        LOG("Could not accept client");
    }
    break;
    case SSL: {
        LOG("SSL Issue");
    }
    break;
    case SSLPeerVerifyError: {
        LOG("SSL Peer Verify Error");
    }
    break;
    case PSKRequired: {
        LOG("PSK issue");
    }
    break;
    }
}

void MyyWebSocket::acceptError(
        QAbstractSocket::SocketError socketError)
{
    ws_error_handler(CantAccept, (void *) socketError);
}

void MyyWebSocket::originAuthenticationRequired(
        QWebSocketCorsAuthenticator *authenticator)
{
    ws_error_handler(AuthRequired, (void *) authenticator);
}

void MyyWebSocket::peerVerifyError(
        QSslError const &error)
{
    ws_error_handler(SSLPeerVerifyError, (void *) &error);
}

void MyyWebSocket::preSharedKeyAuthenticationRequired(
        QSslPreSharedKeyAuthenticator *authenticator)
{
    ws_error_handler(PSKRequired, (void *) authenticator);
}

void MyyWebSocket::serverError(
        QWebSocketProtocol::CloseCode closeCode)
{
    ws_error_handler(ServerGeneric, (void *) &closeCode);
}

void MyyWebSocket::sslErrors(QList<QSslError> const &errors)
{
    ws_error_handler(SSL, (void *) &errors);
}

static inline void clients_for_each(
        QList<QWebSocket *> const * __restrict const list,
        std::function<void (QWebSocket *)> predicate)
{
    int const n_clients = list->length();
    for (int i = 0; i < n_clients; i++) {
        predicate(list->at(i));
    }
}

void MyyWebSocket::send_message(
        QString const &message,
        QWebSocket * const to)
{
    to->sendTextMessage(message);
    LOG("Sending %s to [%s] %s:%d",
        message.toUtf8().data(),
        to->peerName().toUtf8().data(),
        to->peerAddress().toString().toUtf8().data(),
        to->peerPort());
}

void MyyWebSocket::send_message(
        QString const &message)
{
    clients_for_each(&clients,
        [=](QWebSocket * client) -> void {
            send_message(message, client);
        }
    );

}

void MyyWebSocket::send_bindata(
        QByteArray const &data)
{
    clients_for_each(&clients,
        [=](QWebSocket * client) -> void {
            send_bindata(data, client);
        }
    );
}

void MyyWebSocket::send_bindata(
        QByteArray const &data,
        QWebSocket * const to)
{
    to->sendBinaryMessage(data);
    LOG("Sending %d bytes to [%s] %s:%d",
        data.length(),
        to->peerName().toUtf8().data(),
        to->peerAddress().toString().toUtf8().data(),
        to->peerPort());
}

void MyyWebSocket::ws_client_sending_text_data(
        QString const &message)
{
    QWebSocket * const sending_client =
        qobject_cast<QWebSocket *>(sender());
    LOG("Received the following message from [%s] %s:%d : %s\n",
        sending_client->peerName().toUtf8().data(),
        sending_client->peerAddress().toString().toUtf8().data(),
        sending_client->peerPort(),
        message.toUtf8().data());
    LOG("Sending back an appropriate answer");
    QFile faile("/tmp/test.jpg");
    faile.open(QFile::ReadOnly);
    send_bindata(faile.readAll(), sending_client);
    faile.close();
}

void MyyWebSocket::ws_client_sending_bin_data(
        QByteArray const &data)
{
    QWebSocket * const sending_client =
        qobject_cast<QWebSocket *>(sender());
    LOG("Ignoring %d bytes of data sent by [%s] %s:%d.",
        data.length(),
        sending_client->peerName().toUtf8().data(),
        sending_client->peerAddress().toString().toUtf8().data(),
        sending_client->peerPort());
}
