/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   設定画面
****************************************************************************/
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>                      // Qtダイアログ
#include <QMessageBox>                  // メッセージボックス

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT
public:

    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

    void Export();                      // 設定保存

public slots:
    void openDirectory();               // ログ保存フォルダ[選択]ボタン押下

private:
    Ui::SettingDialog *ui;              // UI
};

#endif // SETTINGDIALOG_H
