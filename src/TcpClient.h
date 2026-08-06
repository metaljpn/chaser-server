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
    // 無効ライン無視最大回数
    const static int IGNORE_INVALD = 10;
    // クライアント名1行分の最大ピクセルサイズ
    const static int LIMIT_NAME_PIXEL_SIZE = 250;

    // タイムアウト
    int TIMEOUT = 5000;

    QTcpServer*  server;                // TCPサーバー
    QTcpSocket*  client;                // TCPクライアント

public:
    bool OpenSocket(int Port);          // ソケットオープン
    bool CloseSocket();                 // ソケットクローズ
    bool isConnecting();                // 接続待機中

    // 改行変換
    QString VisibilityString(QString str);
    // 応答待機
    QString WaitResponce();
    // 周辺情報要求
    virtual bool WaitGetReady();
    // 周辺情報応答
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    // 行動に対する周辺情報送信及び受信完了待機
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    explicit TCPClient(QObject *parent = 0);
    virtual ~TCPClient();

private slots:
    void NewConnection ();              // 新規接続
    void DisConnected ();               // 切断

public slots:
    QString GetTeamName();              // チーム名取得
};

#endif // TCPCLIENT_H
