/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   サーバー設定画面
****************************************************************************/
#include "SettingDialog.h"              // 設定画面
#include "ui_SettingDialog.h"           // 設定画面UI

#include <QFileDialog>                  // ファイルダイアログ
#include <QSettings>                    // アプリケーション設定情報

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
    // 設定情報
    QSettings mSettings("setting.ini", QSettings::IniFormat);
    // UI設定
    ui->setupUi(this);

    // ログファイルパス取得
    QVariant v = mSettings.value("LogFilepath");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->Log->setText(v.toString());

    // 通信タイムアウト取得
    v = mSettings.value("Timeout");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        // 設定表示
        ui->Timeout->setValue(v.toInt());

    // フレームレート取得
    v = mSettings.value("Gamespeed");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        // 設定表示
        ui->Gamespeed->setValue(v.toInt());

    // 消音モード設定取得
    v = mSettings.value("Silent");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->SilentCheck->setChecked(v.toBool());

    // ゲーム開始時最大化設定取得
    v = mSettings.value("Maximum");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->MaximumCheck->setChecked(v.toBool());

    //AnimationTime設定
    QSettings aSettings("AnimationTime.ini", QSettings::IniFormat);
    v = aSettings.value( "Map" );
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->AnimeMapTime->setValue(v.toInt());

    v = aSettings.value("Team");
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        ui->AnimeTeamTime->setValue(v.toInt());
}

/****************************************************************************
*   ログ保存フォルダ[選択]ボタン押下
****************************************************************************/
void SettingDialog::openDirectory()
{
    // ログファイル保存フォルダ選択
    ui->Log->setText(QFileDialog::getExistingDirectory(this, tr("ログファイル保存先を選択")));
}

/****************************************************************************
*   設定書込
****************************************************************************/
void SettingDialog::Export()
{
    // サーバー設定
    QSettings mSettings("setting.ini", QSettings::IniFormat);

    // ログファイルパス取得
    mSettings.setValue("LogFilepath", ui->Log->text());
    // 通信タイムアウト取得
    mSettings.setValue("Timeout", ui->Timeout->value());
    // フレームレート取得
    mSettings.setValue("Gamespeed", ui->Gamespeed->value());
    // 消音モード設定取得
    mSettings.setValue("Silent", ui->SilentCheck->isChecked());
    // ゲーム開始時最大化設定取得
    mSettings.setValue("Maximum", ui->MaximumCheck->isChecked());

    // アニメーション設定
    QSettings aSettings("AnimationTime.ini", QSettings::IniFormat );
    // マップ描画時間
    aSettings.setValue("Map", ui->AnimeMapTime->value());
    // チーム描画時間
    aSettings.setValue("Team", ui->AnimeTeamTime->value());
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
SettingDialog::~SettingDialog()
{
    // UI破棄
    delete ui;
}
