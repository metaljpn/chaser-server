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

public:
    explicit DesignDialog(QWidget *parent = 0);

    bool GetCheckDark() const;          // ステージの暗転設定取得
    bool GetCheckBot() const;           // ボット戦設定取得
    void Export();                      // 設定保存

    ~DesignDialog();

private:
    Ui::DesignDialog *ui;               // UI
};

#endif // DESIGNDIALOG_H
