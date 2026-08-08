/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   サーバー設定画面
****************************************************************************/
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>                      // Qtダイアログ

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::SettingDialog *ui;              // UI

public:
    void Export();                      // 設定書込

    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

private slots:
    void openDirectory();               // ログ保存フォルダ[選択]ボタン押下
};

#endif // SETTINGDIALOG_H
