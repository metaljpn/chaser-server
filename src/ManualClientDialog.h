/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   手動クライアント画面
****************************************************************************/
#ifndef MANUALCLIENTDIALOG_H
#define MANUALCLIENTDIALOG_H

#include <QDialog>                      // ダイアログ
#include "GameSystem.h"                 // マップ管理

namespace Ui {
class ManualClientDialog;
}

class ManualClientDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::ManualClientDialog *ui;         // UI

    // 行動定数取得
    GameSystem::Method::ACTION GetAction();

protected:
    // キー押下
    void keyPressEvent(QKeyEvent* event);
    // クローズ
    void closeEvent(QCloseEvent* ce);

public:
    GameSystem::Method next_method;     // 実行行動

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

public slots:
    void AppendLog(const QString& str); // ログ追加

};

#endif // MANUALCLIENT_H
