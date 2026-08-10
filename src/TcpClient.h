/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   TCPクライアント
****************************************************************************/
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QTcpServer>                   // TCPサーバー
#include <QTcpSocket>                   // TCPソケット
#include "BaseClient.h"                 // 通信クライアント

class TCPClient : public BaseClient
{
    Q_OBJECT

private:
    // 無効行無視最大回数
    const static int IGNORE_INVALD = 10;
    // チーム名1行分の最大ピクセルサイズ
    const static int LIMIT_NAME_PIXEL_SIZE = 250;

    int timeout = 5000;                 // タイムアウト

    QTcpServer*  server;                // TCPサーバー
    QTcpSocket*  client;                // TCPクライアント

    // 周辺情報要求
    bool WaitGetReady() override;
    // 周辺情報応答
    Operation WaitReturnOperation(AroundData data) override;
    // 行動に対する周辺情報送信及び受信完了待機
    bool WaitEndSharp(AroundData data) override;
    // 応答待機
    QString WaitResponce();
    // 改行変換
    QString VisibilityString(QString str);
    // 接続待機中
    bool isConnecting();

public:
    bool OpenSocket(int Port);          // ソケットオープン
    bool CloseSocket();                 // ソケットクローズ

    explicit TCPClient(QObject *parent = 0);
    virtual ~TCPClient();

private slots:
    void NewConnection();               // 新規接続
    QString GetTeamName();              // チーム名取得
    void DisConnected();                // 切断
};

#endif // TCPCLIENT_H
