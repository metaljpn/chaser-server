/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   設定画面
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
    QSettings *mSettings;
    // 設定情報読込
    mSettings = new QSettings("setting.ini", QSettings::IniFormat);
    // UI設定
    ui->setupUi(this);

    // ログファイルパス取得
    QVariant v = mSettings->value("LogFilepath");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->Log->setText(v.toString());

    // 通信タイムアウト取得
    v = mSettings->value("Timeout");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->Timeout->setValue(v.toInt());

    // フレームレート取得
    v = mSettings->value("Gamespeed");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->Gamespeed->setValue(v.toInt());

    // 消音モード設定取得
    v = mSettings->value("Silent");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->SilentCheck->setChecked(v.toBool());

    // ゲーム開始時最大化設定取得
    v = mSettings->value("Maximum");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
        // 設定表示
        ui->MaximumCheck->setChecked(v.toBool());
}

/****************************************************************************
*   設定保存
****************************************************************************/
void SettingDialog::Export()
{
    // 設定
    QSettings *mSettings;
    // 設定読込
    mSettings = new QSettings("setting.ini", QSettings::IniFormat);

    // ログファイルパス取得
    mSettings->setValue("LogFilepath", ui->Log->text());
    // 通信タイムアウト取得
    mSettings->setValue("Timeout", ui->Timeout->value());
    // フレームレート取得
    mSettings->setValue("Gamespeed", ui->Gamespeed->value());
    // 消音モード設定取得
    mSettings->setValue("Silent", ui->SilentCheck->isChecked());
    // ゲーム開始時最大化設定取得
    mSettings->setValue("Maximum", ui->MaximumCheck->isChecked());
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
*   デストラクタ
****************************************************************************/
SettingDialog::~SettingDialog()
{
    // UI破棄
    delete ui;
}
