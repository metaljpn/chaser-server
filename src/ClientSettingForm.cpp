/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   クライアント設定画面
****************************************************************************/
#include "ClientSettingForm.h"          // クライアント設定画面
#include "ui_ClientSettingForm.h"       // クライアント設定画面UI

#include "ComClient.h"                  // 通信クライアント 自動くん
#include "ManualClient.h"               // 手動クライアント
#include "TcpClient.h"                  // TCPクライアント

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
ClientSettingForm::ClientSettingForm(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ClientSettingForm)
{
    // UI初期化
    ui->setupUi(this);
    // TCPクライアント
    this->client = new TCPClient();
    // 接続時のシグナル･スロット設定
    connect(this->client, &TCPClient::Connected,    this, &ClientSettingForm::Connected);
    // 待機中のシグナル･スロット設定
    connect(this->client, &TCPClient::Ready,        this, &ClientSettingForm::SetStandby);
    // 切断時のシグナル･スロット設定
    connect(this->client, &TCPClient::Disconnected, this, &ClientSettingForm::DisConnected);

    //Windowsでボットのexeファイルがあるときのみ、ボットプログラムを動かせるように設定
    #ifdef Q_OS_WINDOWS
        // ボット実行ファイル存在
        if(QFile::exists("./2019-U16asahikawaBot/u16asahikawaBot.exe"))
            // クライアント種別に追加
            ui->ComboBox->addItem("botV4");
    #endif
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
ClientSettingForm::~ClientSettingForm()
{
    delete ui;                          // UI破棄
    delete this->client;                // TCPクライアント破棄
    delete botProcess;                  // ボットプロセス破棄
}

/****************************************************************************
*   準備完了
****************************************************************************/
void ClientSettingForm::SetStandby()
{
    // 接続名
    this->ui->NameLabel ->setText(this->client->Name == "" ? "未設定" : this->client->Name);
    // IP
    this->ui->IPLabel   ->setText(this->client->IP);
    // 状態
    this->ui->StateLabel->setText("準備完了");
    //if(this->ui->ComboBox->currentText() != "TCPユーザー");
    // ボタンキャプション
    this->ui->ConnectButton->setText("　切断　");

    // 待機
    emit Standby(this,true);
}

/****************************************************************************
*   接続
****************************************************************************/
void ClientSettingForm::Connected()
{
    // IP
    this->ui->IPLabel      ->setText(this->client->IP);
    // 状態
    this->ui->StateLabel   ->setText("接続中");
    // ボタンキャプション
    this->ui->ConnectButton->setText("　切断　");
}

/****************************************************************************
*   切断
****************************************************************************/
void ClientSettingForm::DisConnected() {
    // 切断シグナル･スロット解除
    disconnect(this->client, &TCPClient::Disconnected, this, &ClientSettingForm::DisConnected);
    // TCPクライアント有効
    if (dynamic_cast<TCPClient*>(this->client) != nullptr) {
        // ソケットクローズ
        dynamic_cast<TCPClient*>(this->client)->CloseSocket();
        // 再生成
        this->client = new TCPClient(this);
    }

    // 接続時のシグナル･スロット設定
    connect(this->client, &TCPClient::Connected,    this, &ClientSettingForm::Connected);
    // 待機中のシグナル･スロット設定
    connect(this->client, &TCPClient::Ready,        this, &ClientSettingForm::SetStandby);
    // 切断時のシグナル･スロット設定
    connect(this->client, &TCPClient::Disconnected, this, &ClientSettingForm::DisConnected);

    // シグナル･スロット設定直後処理
    this->client->Startup();

    // ボタンキャプション
    this->ui->ConnectButton->setText("接続開始");
    // 状態
    this->ui->StateLabel->setText("非接続");
    // 接続名
    this->ui->NameLabel->setText("不明");
    // IP
    this->ui->IPLabel->setText("不明");
    // ポート番号設定有効
    this->ui->PortSpinBox->setEnabled(true);

    // 状態解除
    if (this->ui->ConnectButton->isChecked())
        this->ui->ConnectButton->toggle();

    // 待機
    emit Standby(this, false);
}

/****************************************************************************
*   待機切替
*
*   @param state 待機状態
****************************************************************************/
void ClientSettingForm::ConnectionToggled(bool state)
{
    // 待機
    if(state){
        // ソケットオープン
        dynamic_cast<TCPClient*>(this->client)->OpenSocket(ui->PortSpinBox->value());
        // ボタンキャプション
        this->ui->ConnectButton->setText("待機終了");
        // 状態
        this->ui->StateLabel->setText("TCP接続待ち状態");
        // ポート番号設定無効
        this->ui->PortSpinBox->setEnabled(false);
    }else{
        // ソケットクローズ
        dynamic_cast<TCPClient*>(this->client)->CloseSocket();
        //this->client->Startup();

        // ボタンキャプション
        this->ui->ConnectButton->setText("接続開始");
        // 状態
        this->ui->StateLabel->setText("非接続");
        // 接続名
        this->ui->NameLabel->setText("不明");
        // IP
        this->ui->IPLabel->setText("不明");
        // ポート番号設定有効
        this->ui->PortSpinBox->setEnabled(true);
        // 待機
        emit Standby(this,false);
    }
}

/****************************************************************************
*   クライアント種別切替
*
*   @param text 待機対象
****************************************************************************/
void ClientSettingForm::ComboBoxChenged(QString text)
{
    // TCPクライアント破棄
    delete client;
    // クライアント種別=TCPユーザー
    if(text=="TCPユーザー"){
        // TCPクライアント
        this->client = new TCPClient(this);
        // 状態
        this->ui->StateLabel->setText("非接続");
        // 接続名
        this->ui->NameLabel->setText("不明");
        // ボタンキャプション
        this->ui->ConnectButton->setText("接続開始");
        // ポート番号設定有効
        this->ui->PortSpinBox->setEnabled(true);
        // [接続開始]ボタン有効
        this->ui->ConnectButton->setEnabled(true);
    }else {
        // ポート番号設定無効
        this->ui->PortSpinBox->setEnabled(false);
        // ボタン無効
        this->ui->ConnectButton->setEnabled(false);
        // クライアント種別=自動くん
        if(text=="自動くん")this->client = new ComClient(this);
        // クライアント種別=手動クライアント
        if(text=="ManualClient")this->client = new ManualClient(this);

        // クライアント種別=ボット
        if(text=="botV4"){
            // TCPクライアント
            this->client = new TCPClient(this);

            // 待機開始
            ConnectionToggled(true);

            // ボット用外部プログラム
            botProcess = new QProcess();

            // 実行ファイルパス
            QString command = "./2019-U16asahikawaBot/u16asahikawaBot.exe";
            // 引数
            QStringList option;

            // 引数設定
            option << "a:127.0.0.1" << "p:" + QString::number(ui->PortSpinBox->value()) << "n:botV4";

            // ボット用外部プログラム開始
            botProcess->start(command, option);
        }
    }

    // 待機
    emit Standby(this,false);
    // 接続時のシグナル･スロット設定
    connect(this->client, &TCPClient::Connected,    this, &ClientSettingForm::Connected);
    // 待機中のシグナル･スロット設定
    connect(this->client, &TCPClient::Ready,        this, &ClientSettingForm::SetStandby);
    // 切断時のシグナル･スロット設定
    connect(this->client, &TCPClient::Disconnected, this, &ClientSettingForm::DisConnected);
    // シグナル･スロット設定直後処理
    this->client->Startup();
}

/****************************************************************************
*   ポート番号設定
*
*   @param num ポート番号
****************************************************************************/
void ClientSettingForm::SetPortSpin(int num)
{
    // ポート番号設定
    ui->PortSpinBox->setValue(num);
}
