/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   起動画面
****************************************************************************/
#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>                      // ダイアログ
#include <QFile>                        // ファイルアクセス
#include <QMessageBox>                  // メッセージボックス
#include <QFileDialog>                  // ファイルダイアログ
#include "SettingDialog.h"              // 設定画面
#include "ClientSettingForm.h"          // クライアント設定画面

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::StartupDialog *ui;              // UI
    bool team_standby[TEAM_COUNT];      // チーム準備状態
    bool map_standby;                   // マップ準備状態

public:
    QString music_text;                 // BGM名
    GameSystem::Map map;                // マップ情報

    // クライアント設定画面
    ClientSettingForm* team_client[TEAM_COUNT];

public:
    bool MapRead(const QString& dir);   // マップ読込
    void setMusicFileList();            // BGMリスト生成
    void setImageThemeList();           // テクスチャリスト生成

    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();

public slots:

    void ChangeMusicCombo(QString text);    // BGM選択
    void CheckStandby();                    // ゲーム開始可否確認
    void ShowMapEditDialog();               // マップ編集画面表示
    void PushedMapSelect();                 // マップ選択
    void ShowDesignDialog();                // [デザイン設定]ボタン押下

    // クライアント準備設定
    void ClientStandby(ClientSettingForm* client,bool complate);
    void SetMapStandby (bool state);        // マップ準備設定

    void ChangedTexture(QString text);      // テクスチャ変更

    void Setting();                         // [サーバー設定]ボタン押下

};

#endif // STARTUPDIALOG_H
