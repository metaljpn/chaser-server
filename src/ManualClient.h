/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   手動クライアント
****************************************************************************/
#ifndef MANUALCLIENT_H
#define MANUALCLIENT_H

#include "BaseClient.h"                 // 通信クライアント
#include "ManualClientDialog.h"         // 手動クライアント画面

class ManualClient : public BaseClient
{
    Q_OBJECT

public:
    ManualClientDialog* diag;           // 手動クライアント画面

    // 周辺情報要求
    bool WaitGetReady()override;
    // 周辺情報応答
    GameSystem::Method WaitReturnMethod(GameSystem::AroundData data)override;
    // 行動に対する周辺情報送信及び受信完了待機
    bool WaitEndSharp(GameSystem::AroundData data)override;
    // シグナルスロット設定後処理
    void Startup() override;

    ManualClient(QWidget *parent=0);
    ~ManualClient();

public slots:
    // クローズ
    void closeEvent();
};

#endif // MANUALCLIENT_H
