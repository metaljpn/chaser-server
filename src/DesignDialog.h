/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   デザイン設定画面
****************************************************************************/
#ifndef DESIGNDIALOG_H
#define DESIGNDIALOG_H

#include <QDialog>                      // ダイアログ

namespace Ui {
class DesignDialog;
}

class DesignDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::DesignDialog *ui;               // UI

public:
    bool GetCheckDark() const;          // ステージの暗転設定取得
    bool GetCheckBot() const;           // ボット戦設定取得
    void Export();                      // 設定保存

    explicit DesignDialog(QWidget *parent = 0);
    ~DesignDialog();
};

#endif // DESIGNDIALOG_H
