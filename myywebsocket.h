#ifndef MYYWEBSOCKET_H
#define MYYWEBSOCKET_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketCorsAuthenticator>
#include <QSslPreSharedKeyAuthenticator>

class MyyWebSocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bindAddress READ bindAddress WRITE bindAddressSet)
    Q_PROPERTY(int bindPort READ bindPort WRITE bindPortSet)
public:
    explicit MyyWebSocket(QObject *parent = nullptr);

    /* Address */
    QString bindAddress();
    void bindAddressSet(QString const &newAddress);

    /* Port */
    int bindPort();
    void bindPortSet(int newPort);

    Q_INVOKABLE void listen();
    Q_INVOKABLE void send_message(QString const &message);
    Q_INVOKABLE void send_bindata(QByteArray const &message);
signals:

private:
    enum ErrorType {
        Invalid,
        ServerGeneric,
        AuthRequired,
        CantAccept,
        SSL,
        SSLPeerVerifyError,
        PSKRequired,
    };
    QString bind_address;
    int bind_port;
    // Might need encapsulation
    QWebSocketServer server;
    QList<QWebSocket *> clients;
    void ws_client_left();
    void ws_error_handler(
        ErrorType error,
        void * impl_data);
    void send_message(
            QString const &message,
            QWebSocket * to);
    void send_bindata(
            QByteArray const &message,
            QWebSocket * to);

private slots:
    void ws_server_closed();
    void ws_client_ringing();

    void acceptError(QAbstractSocket::SocketError socketError);
    void originAuthenticationRequired(QWebSocketCorsAuthenticator *authenticator);
    void peerVerifyError(QSslError const &error);
    void preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *authenticator);
    void serverError(QWebSocketProtocol::CloseCode closeCode);
    void sslErrors(QList<QSslError> const &errors);

    void ws_client_sending_text_data(QString const &message);
    void ws_client_sending_bin_data(QByteArray const &data);

};

#endif // MYYWEBSOCKET_H
