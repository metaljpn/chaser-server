/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   メイン画面
****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>                  // メインウィンドウ
#include <QMediaPlayer>                 // メディアファイル再生
#include "GameSystem.h"                 // 動作管理
#include "StartupDialog.h"              // 起動画面
#include "StableLog.h"                  // ログ管理

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // 勝者
    enum class WINNER{
        TEAMS,                          // チーム
        DRAW,                           // 引き分け
        CONTINUE,                       // 継続
    };

    int player;                         // 次ターン行動チーム
    int frame_rate = 150;               // ゲームフレームレート
    QTimer* clock;                      // ゲームクロック
    QTimer* startup_anime;              // 開始アニメーション
    QTimer* teamshow_anime;             // チーム表示アニメーション
    QTimer* blind_anime;                // 暗闇アニメーション
    StartupDialog* startup;             // 起動ダイアログ
    QMediaPlayer* bgm;                  // BGM
    QAudioOutput* audio_output;         // 音声出力チャンネル
    bool silent;                        // 消音モード
    bool dark;                          // 暗闇モード
    bool isbotbattle;                   // ボット戦モード
    StableLog log;                      // ログストリーム
    int anime_map_time   = 6000;        // マップ描画時間
    int anime_team_time  = 2000;        // チーム配置アニメーション時間
    int anime_blind_time = 1000;        // 暗闇アニメーション時間
    float audio_volume = 0.8f;          // 音量
    WINNER win;                         // 勝者

    Ui::MainWindow *ui;                 // UI

    // インスタンス
    static MainWindow* s_instance;
    // メッセージハンドラ退避
    static QtMessageHandler s_prevMsgHandler;
    // メッセージハンドラ
    static void s_messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    // 日時取得
    static QString getTime();

    // 得点更新
    void ScoreUpdate(Operation method);
    // クライアント行動文字列変換
    QString convertString(Operation method);
    // 勝敗判定
    WINNER Judge();
    // 勝敗通知
    void Finish(WINNER win);
    // チーム名取得
    QString getTeamName(GameSystem::TEAM team);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void DrawMapAnimation();            // マップ描画
    void SetTeamAnimation();            // チーム配置
    void BlindAnimation();              // 暗闇描画
    void StepGame();                    // ゲーム進行
};

#endif // MAINWINDOW_H
