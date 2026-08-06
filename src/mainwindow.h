/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   メイン画面
****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>                  // メインウィンドウ
#include <QMediaPlayer>                 // メディアファイル再生
#include "startupdialog.h"              // 起動画面
#include "StableLog.h"                  // ログ管理

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:

    int player;                         // 次ターン行動チーム

    int FRAME_RATE = 150;               // ゲームフレームレート
    QTimer* clock;                      // ゲームクロック
    QTimer* startup_anime;              // 開始アニメーション
    QTimer* teamshow_anime;             // チーム表示アニメーション
    QTimer* blind_anime;                // まっくらアニメーション
    StartupDialog* startup;             // スタートアップダイアログ
    QMediaPlayer* bgm;                  // BGM
    QAudioOutput* audio_output;         // 音声出力チャンネル

    bool silent;                        // 消音モード

    bool dark;                          // 暗転モード
    bool isbotbattle;                   // ボット戦モード

    StableLog log;                      // ログストリーム
    int anime_map_time   = 6000;        // マップ描画時間
    int anime_team_time  = 2000;        // チーム配置アニメーション時間
    int anime_blind_time = 1000;        // 暗闇アニメーション時間
    float audio_volume = 0.8f;          // 音量

    GameSystem::WINNER win;             // 勝者

    // 日時取得
    static QString getTime();
    // クライアント行動文字列変換
    static QString convertString(GameSystem::Method method);

    // qInstallMessageHandler 用
    static MainWindow* s_instance;

	// 前のメッセージハンドラを保存するための変数
    static QtMessageHandler s_prevMsgHandler;

	// メッセージハンドラ(ファイルへのログの書き出しを追加)
    static void s_messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;                 // UI

private slots:
    // ゲーム進行
    void StepGame();
    // アイテム取得判定
    void RefreshItem(GameSystem::Method method);
    // 勝敗判定
    GameSystem::WINNER Judge();
    // 勝敗通知
    void Finish(GameSystem::WINNER win);

    // マップ描画
    void StartAnimation();
    // チーム配置
    void ShowTeamAnimation();
    // 暗転描画
    void BlindAnimation();
};

#endif // MAINWINDOW_H
