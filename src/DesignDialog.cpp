/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   デザイン設定画面
****************************************************************************/
#include "DesignDialog.h"               // デザイン設定画面
#include "ui_DesignDialog.h"            // デザイン設定画面UI
#include "qsettings.h"                  // アプリケーション設定情報
#include "qmessagebox.h"                // メッセージボックス

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
DesignDialog::DesignDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DesignDialog)
{
    // UI初期化
    ui->setupUi(this);

    // デザイン設定
    QSettings* mSettings;
    // 設定読込
    mSettings = new QSettings( "design.ini", QSettings::IniFormat );
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    // ステージの暗転取得
    QVariant v = mSettings->value( "Dark" );
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType)
    {
        // 設定表示
        ui->DarkBox->setChecked(v.toBool());
    }
    // ボット戦取得
    v = mSettings->value( "Bot" );
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType){
        // 設定表示
        ui->BotBox->setChecked(v.toBool());
    }
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
DesignDialog::~DesignDialog()
{
    // UI破棄
    delete ui;
}

/****************************************************************************
*   ステージの暗転設定取得
*
*   @return 設定内容
****************************************************************************/
bool DesignDialog::GetCheckDark() const {
    return ui->DarkBox->isChecked();
}

/****************************************************************************
*   ボット戦設定取得
*
*   @return 設定内容
****************************************************************************/
bool DesignDialog::GetCheckBot() const {
    return ui->BotBox->isChecked();
}

/****************************************************************************
*   設定保存
****************************************************************************/
void DesignDialog::Export(){
    // デザイン設定
    QSettings* mSettings;
    // 設定読込
    mSettings = new QSettings( "design.ini", QSettings::IniFormat );
    // mSettings->setIniCodec( "UTF-8" ); // iniファイルの文字コード

    // ステージの暗転取得
    mSettings->setValue( "Dark"    , GetCheckDark());
    // ボット戦取得
    mSettings->setValue( "Bot"     , GetCheckBot());
    //QMessageBox::information(this, tr("警告"), tr("設定は再起動後有効になります。"));
}
