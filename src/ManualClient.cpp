/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   手動クライアント
****************************************************************************/
#include "ManualClient.h"               // 手動クライアント

#include <QEventLoop>                   // イベントループ
#include <QScreen>                      // 画面プロパティ

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
ManualClient::ManualClient(QWidget* parent):
    BaseClient(parent)
{
    // 名称
    Name = "ManualClient";
    // IP
    IP   = "ローカル";
    // 手動クライアント画面
    diag = new ManualClientDialog();
    // クローズ時シグナル･スロット設定
    connect(diag, &ManualClientDialog::CloseWindow, this, &ManualClient::closeEvent);
}

/****************************************************************************
*   周辺情報要求
*
*   @return true固定
****************************************************************************/
bool ManualClient::WaitGetReady(){
    return true;
}

/****************************************************************************
*   周辺情報応答
*
*   @return 行動情報
*
*   @param data 周辺情報
****************************************************************************/
Operation ManualClient::WaitReturnOperation(AroundData data){
    // 自動返答ログ追加
    diag->AppendLog(QString("[GetReady  Response] ") + data.toString());

    // イベントループ
    QEventLoop eventLoop;
    // 手動クライアント画面シグナル･スロット設定
    QDialog::connect(diag, &ManualClientDialog::ReadyAction, &eventLoop, &QEventLoop::quit);
    // 手動クライアントシグナル･スロット設定
    QDialog::connect(this, &ManualClient::Disconnected, &eventLoop, &QEventLoop::quit);
    // GUI応答待機
    eventLoop.exec();

    // 戻り値[行動情報]
    return diag->next_method;
}

/****************************************************************************
*   行動に対する周辺情報送信及び受信完了待機
*
*   @return true固定
*
*   @param data 周辺情報
****************************************************************************/
bool ManualClient::WaitEndSharp(AroundData data){
    // 自動返答ログ追加
    diag->AppendLog(QString("[Operation Response] ") + data.toString());
    return true;
}

/****************************************************************************
*   シグナルスロット設定後処理
****************************************************************************/
void ManualClient::Startup(){
    // 接続
    emit Connected();
    // チーム名設定
    emit WriteTeamName();
    // 待機
    emit Ready();

    // 常に手前に表示
    diag->setWindowFlags(Qt::WindowStaysOnTopHint);
    // 画面サイズ取得
    auto displaySize = QGuiApplication::primaryScreen()->size();
    // ダイアログサイズ取得
    auto diagSize = diag->size();
    // 画面サイズに合わせて表示位置移動
    diag->move((displaySize.width()-diagSize.width())*0.5, displaySize.height()*0.65);
    // 画面表示
    diag->show();
}

/****************************************************************************
*   クローズ
****************************************************************************/
void ManualClient::closeEvent(){
    // 切断
    emit Disconnected();
    is_disconnected=true;
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
ManualClient::~ManualClient()
{
    // 画面非表示
    diag->hide();
}
