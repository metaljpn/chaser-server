/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   通信クライアント 自動くん
****************************************************************************/
#ifndef COMCLIENT_H
#define COMCLIENT_H

#include <QObject>                      // Qtオブジェクト
#include "BaseClient.h"                 // 通信クライアント

class ComClient : virtual public BaseClient
{
private:
    // 周辺情報要求
    bool WaitGetReady() override;
    // 周辺情報応答
    Operation WaitReturnOperation(AroundData data) override;
    // 行動に対する周辺情報送信及び受信完了待機
    bool WaitEndSharp(AroundData data) override;
    // シグナルスロット設定後処理
    void Startup() override;

public:
    explicit ComClient(QObject *parent = 0);
    ~ComClient();
};

#endif // COMCLIENT_H
