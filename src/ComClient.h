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
public:
    // 周辺情報要求
    virtual bool WaitGetReady();
    // 周辺情報応答
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data);
    // 行動に対する周辺情報送信及び受信完了待機
    virtual bool WaitEndSharp(GameSystem::AroundData data);

    // シグナルスロット設定後処理
    virtual void Startup();

    explicit ComClient(QObject *parent = 0);
    ~ComClient();
};

#endif // COMCLIENT_H
