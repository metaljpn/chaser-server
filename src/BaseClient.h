/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   通信クライアント(純粋仮想クラス)
****************************************************************************/
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>                      // Qtオブジェクト
#include "GameSystem.h"                 // マップ管理

class BaseClient : public QObject
{
    Q_OBJECT

public:
    QString Name;                       // チーム名
    QString IP;                         // IP
    bool is_disconnected;               // 切断フラグ

    // 周辺情報要求
    virtual bool WaitGetReady()=0;
    // 周辺情報応答
    virtual GameSystem::Method WaitReturnMethod(GameSystem::AroundData data)=0;
    // 行動に対する周辺情報送信及び受信完了待機
    virtual bool WaitEndSharp(GameSystem::AroundData data)=0;
    // シグナルスロット設定後処理
    virtual void Startup();

    explicit BaseClient(QObject *parent = nullptr);
    virtual ~BaseClient();

signals:
    void Connected();                   // 接続
    void WriteTeamName();               // チーム名表示
    void Ready();                       // 準備完了
    void Disconnected();                // 切断
};

#endif // CLIENT_H
