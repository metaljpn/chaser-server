/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   手動クライアント画面
****************************************************************************/
#ifndef MANUALCLIENTDIALOG_H
#define MANUALCLIENTDIALOG_H

#include <QDialog>                      // ダイアログ
#include "GameSystem.h"                 // 動作管理

namespace Ui {
class ManualClientDialog;
}

class ManualClientDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ManualClientDialog *ui;         // UI

    void keyPressEvent(QKeyEvent* event);   // キー押下
    void closeEvent(QCloseEvent* ce);       // クローズ
    Operation::ACTION GetAction();          // 行動定数取得

public:
    Operation next_method;              // 実行行動

    void AppendLog(const QString& str); // ログ追加

    explicit ManualClientDialog(QWidget *parent = 0);
    ~ManualClientDialog();

signals:
    void ReadyAction();                 // 行動決定
    void CloseWindow();                 // クローズ

private slots:
    void UPButtonClicked();             // [↑]ボタン押下
    void DOWNButtonClicked();           // [↓]ボタン押下
    void RIGHTButtonClicked();          // [→]ボタン押下
    void LEFTButtonClicked();           // [←]ボタン押下
};

#endif // MANUALCLIENT_H
