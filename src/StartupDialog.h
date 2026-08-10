/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   起動画面
****************************************************************************/
#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>                      // ダイアログ
#include "ClientSettingForm.h"          // クライアント設定画面
#include "GameMap.h"                    // マップ管理

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

private:
    bool team_standby[TEAM_COUNT];      // チーム準備状態
    bool map_standby;                   // マップ準備状態

    Ui::StartupDialog *ui;              // UI

    void setMusicFileList();            // BGMリスト生成
    void setImageThemeList();           // テクスチャリスト生成
    // クライアント準備設定
    void ClientStandby(ClientSettingForm* client,bool complate);
    void SetMapStandby(bool state);     // マップ準備設定
    bool MapRead(const QString& dir);   // マップ読込
    void CheckStandby();                // ゲーム開始可否確認

public:
    GameMap map;                        // マップ情報
    QString music_text;                 // BGM名

    // クライアント設定画面
    ClientSettingForm* team_client[TEAM_COUNT];

    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

private slots:
    void PushedMapSelect();                 // マップ選択
    void ShowMapEditDialog();               // [マップの編集]ボタン押下
    void ChangedTexture(QString text);      // テクスチャ選択
    void ChangeMusicCombo(QString text);    // BGM選択
    void ShowDesignDialog();                // [デザイン設定]ボタン押下
    void Setting();                         // [サーバー設定]ボタン押下
};

#endif // STARTUPDIALOG_H
