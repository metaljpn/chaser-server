/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   クライアント設定画面
****************************************************************************/
#ifndef CLIENTSETTINGFORM_H
#define CLIENTSETTINGFORM_H

#include <QFile>                        // ファイルアクセス
#include <QGroupBox>                    // グループボックス
#include <QProcess>                     // 外部プログラム起動
#include "BaseClient.h"                 // 通信クライアント

namespace Ui {
class ClientSettingForm;
}

class ClientSettingForm : public QGroupBox
{
    Q_OBJECT

private:
    Ui::ClientSettingForm *ui;          // UI
    QProcess *botProcess;               // ボットプロセス

public:
    BaseClient* client;                 // TCPクライアント

    explicit ClientSettingForm(QWidget *parent = 0);
    ~ClientSettingForm();

signals:
    // 準備完了
    void Standby(ClientSettingForm* client,bool complate);

public slots:
    void SetStandby ();                 // 準備完了
    void Connected  ();                 // 接続
    void DisConnected();                // 切断
    void ConnectionToggled(bool state); // 待機切替
    void ComboBoxChenged(QString text); // クライアント種別切替
    void SetPortSpin(int num);          // ポート番号設定
};

#endif // CLIENTSETTINGFORM_H
