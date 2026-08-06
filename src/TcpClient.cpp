/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   TCPクライアント
****************************************************************************/
#include "TcpClient.h"                  // TCPクライアント

#include <QFont>                        // フォント
#include <QLabel>                       // ラベル
#include <QRegularExpression>           // 正規表現
#include <QSettings>                    // アプリケーション設定情報

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
TCPClient::TCPClient(QObject *parent)
    : BaseClient(parent)
{
    // 設定情報
    QSettings *mSettings;
    // 設定情報読込
    mSettings = new QSettings("setting.ini", QSettings::IniFormat);
    // 通信タイムアウト取得
    QVariant v = mSettings->value("Timeout");
    // 設定有効
    if (v.typeId() != QMetaType::UnknownType) {
        // 通信タイムアウト取得
        TIMEOUT = v.toInt();
    }
    // 設定情報破棄
    delete mSettings;

    // TCPサーバー
    this->server = new QTcpServer(this);
    // TCPクライアント
    this->client = nullptr;
    // 接続可能最大数
    this->server->setMaxPendingConnections(1);
    // TCPサーバー接続時のシグナル･スロット設定
    connect(this->server, &QTcpServer::newConnection, this, &TCPClient::NewConnection);
}

/****************************************************************************
*   周辺情報要求
*
*   @return 処理結果
****************************************************************************/
bool TCPClient::WaitGetReady()
{
    // 切断
    if (is_disconnected)
        // 戻り値[異常]
        return false;
    // TCPクライアント無効
    if (client == nullptr)
        // 戻り値[異常]
        return false;
    // "@"+改行送信
    client->write(QString("@\r\n").toUtf8());

    // 応答待機
    QString response = WaitResponce();
    // デバッグ情報
    qDebug() << "res:" + response;
    qDebug() << (response == "gr\r\n");
    // 戻り値[gr+改行]
    return (response == "gr\r\n");
}

/****************************************************************************
*   周辺情報応答
*
*   @return 行動･方向情報
*
*   @param data 周辺情報
****************************************************************************/
GameSystem::Method TCPClient::WaitReturnMethod(GameSystem::AroundData data)
{
    // 切断
    if (is_disconnected)
        // 戻り値[行動無効]
        return GameSystem::Method();
    // TCPクライアント無効
    if (client == nullptr)
        // 戻り値[チーム名:不明 行動:不明 方向:不明]
        return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                                  GameSystem::Method::ACTION::UNKNOWN,
                                  GameSystem::Method::ROTE::UNKNOWN};
    // 周辺情報送信
    client->write(QString(data.toString() + "\r\n").toUtf8());
    // デバッグ情報
    qDebug() << "WRM";
    qDebug() << data.toString();
    // 応答待機
    QString response = WaitResponce();
    // 応答受信
    if (response != QString())
        // 戻り値[行動･方向情報]
        return GameSystem::Method::fromString(response);
    else
        // 戻り値[チーム名:不明 行動:不明 方向:不明]
        return GameSystem::Method{GameSystem::TEAM::UNKNOWN,
                                  GameSystem::Method::ACTION::UNKNOWN,
                                  GameSystem::Method::ROTE::UNKNOWN};
}

/****************************************************************************
*   行動に対する周辺情報送信及び受信完了待機
*
*   @return 受信完了結果
*
*   @param data 周辺情報
****************************************************************************/
bool TCPClient::WaitEndSharp(GameSystem::AroundData data)
{
    // 切断
    if (is_disconnected)
        // 戻り値[異常]
        return false;
    // TCPクライアント無効
    if (client == nullptr)
        // 戻り値[異常]
        return false;
    // 周辺情報文字列
    client->write(QString(data.toString() + "\r\n").toUtf8());
    // デバッグ情報
    qDebug() << "WES";
    qDebug() << data.toString();
    // #+改行受信待機
    return (WaitResponce() == "#\r\n");
}

/****************************************************************************
*   応答待機
*
*   @return 応答文字列
****************************************************************************/
QString TCPClient::WaitResponce()
{
    int ignore = 0;                     // 無効行カウンタ

    // 無効行無視最大回数到達まで
    while (ignore != this->IGNORE_INVALD) {
        // デバッグ情報
        qDebug() << "WaitStart:";
        // 受信待機
        if (this->client->waitForReadyRead(this->TIMEOUT)) {
            QString response = "";      // 受信文字列

            do {
                // 受信データ結合
                response += client->readLine();
                // 改行未検出 and タイムアウトまで
            } while (*(response.end() - 1) != '\n' && this->client->waitForReadyRead(this->TIMEOUT));

            // デバッグ情報
            qDebug() << "" + VisibilityString(response);

            // 空文字(改行のみ)
            if (response == "" || response == "\n" || response == "\r" || response == "\r\n") {
                // 無効行カウンタ更新
                ignore++;
                // ループ終了
                continue;
            }
            // 文字列有効 and 改行なし(タイムアウト)
            if (response.size() > 0 && *(response.end() - 1) != '\n') {
                // 切断
                is_disconnected = true;
                // デバッグ情報
                qDebug() << QString("[Port") + QString::number(this->client->localPort())
                                + "]:Noting \\n";
                // 戻り値[空文字]
                return QString();
            }

            // デバッグ情報
            qDebug() << "test:" + response;
            // 戻り値[応答文字列]
            return response;
        } else {
            // 切断
            is_disconnected = true;
            // 戻り値[空文字]
            return QString();
        }
    }
    // 切断
    is_disconnected = true;
    // 戻り値[空文字]
    return QString();
}

/****************************************************************************
*   改行変換
*
*   @return 変換後文字列
*
*   @param str 対象文字列
****************************************************************************/
QString TCPClient::VisibilityString(QString str)
{
    QString answer;                     // 文字列バッファ
    // 文字列長分
    for (int i = 0; i < str.size(); i++) {
        // 改行
        if (str[i] == '\n')
            // '\'付加
            answer.push_back("\\n");
        // キャリッジリターン
        else if (str[i] == '\r')
            // '\'付加
            answer.push_back("\\r");
        else
            // 文字取得
            answer.push_back(str[i]);
    }
    // 戻り値[文字列]
    return answer;
}

/****************************************************************************
*   ソケットオープン
*
*   @return true固定
*
*   @param Port ポート番号
****************************************************************************/
bool TCPClient::OpenSocket(int Port)
{
    // 指定ポートで待機
    this->server->listen(QHostAddress::Any, Port);
    return true;
}

/****************************************************************************
*   ソケットクローズ
*
*   @return true固定
****************************************************************************/
bool TCPClient::CloseSocket()
{
    // TCPクライアント有効 and ソケットオープン
    if (this->client != nullptr && this->client->isOpen()) {
        // 切断
        this->client->close();
    }
    // 接続待機中
    if (this->server->isListening()) {
        // 待機終了
        this->server->close();
    }
    return true;
}

/****************************************************************************
*   新規接続
****************************************************************************/
void TCPClient::NewConnection()
{
    // TCPクライアント破棄
    delete this->client;
    // 保留中の接続をTCPクライアントとする
    this->client = this->server->nextPendingConnection();
    // IP取得
    this->IP = this->client->peerAddress().toString();
    // TCPクライアント受信のシグナル･スロット設定
    connect(this->client, &QTcpSocket::readyRead,    this, &TCPClient::GetTeamName);
    // TCPクライアント切断のシグナル･スロット設定
    connect(this->client, &QTcpSocket::disconnected, this, &TCPClient::DisConnected);
    // 接続
    is_disconnected = false;

    // 接続待機終了
    this->server->close();
    // 接続
    emit Connected();
}

/****************************************************************************
*   切断
****************************************************************************/
void TCPClient::DisConnected()
{
    // TCPクライアント接続中
    if (this->client->isOpen()) {
        // 切断
        this->client->close();
    }
    // 接続待機中
    if (this->server->isListening()) {
        // 待機終了
        this->server->close();
    }
    // IP初期化
    this->IP = "";
    // チーム名初期化
    this->Name = "";
    // 切断
    is_disconnected = true;
    // TCPクライアント受信のシグナル･スロット設定
    connect(this->client, &QTcpSocket::readyRead,    this, &TCPClient::GetTeamName);
    // TCPクライアント切断のシグナル･スロット設定
    connect(this->client, &QTcpSocket::disconnected, this, &TCPClient::DisConnected);
    // 切断
    emit Disconnected();
}

/****************************************************************************
*   チーム名取得
*
*   @return チーム名
****************************************************************************/
QString TCPClient::GetTeamName()
{
    // チーム名無効
    if (this->Name == "") {
        // 全受信データ
        QByteArray bytebuf = client->readAll();

        // 文字コードをUTFに変換
        QString namebuf = QString::fromUtf8(bytebuf);

        // 名前のエスケープ処理(改行やタブなどの文字の効果を削除)
        static auto REX = QRegularExpression(
            "["
             "\r\n\u2028\u2029\u202A\u202B\u202C\u202D\u202E\u202F" //改行
             "\u200B" //ゼロ幅スペース
             "\a\b\f\t\v\uFFFD" //その他
             "]"
            );
        namebuf = namebuf.replace(REX, "");

        // ラベル
        QLabel nameLabel;
        // HTMLタグ無効
        nameLabel.setTextFormat(Qt::PlainText);
        // フォント設定
        nameLabel.setFont(QFont("Yu Gothic UI", 20));

        // チーム名
        QString clientName = "", lineText = "";
        bool insertNewLineFlag = false;

        // 文字数分
        for(int i = 0; i < namebuf.length(); i++){
            // 対象1文字
            QString nextChar = namebuf[i];
            // 文字連結
            nameLabel.setText(lineText + nextChar);

            // サロゲート文字
            if(namebuf[i].isHighSurrogate()){
                // 次の1文字(コード)を取得
                i++;
                nextChar += namebuf[i];
                // 文字連結
                nameLabel.setText(lineText + nextChar);
            }
            // 次の文字(コード)が字形選択子 or 絵文字用の異体字セレクタ
            else if(i + 1 < namebuf.length() && (QString(namebuf[i + 1]) == "\uFE0E" || QString(namebuf[i + 1]) == "\uFE0F")){
                // 次の1文字(コード)を取得
                i++;
                nextChar += namebuf[i];
                // 文字連結
                nameLabel.setText(lineText + nextChar);
            }
            // 文字サイズ調整
            nameLabel.adjustSize();

            // 文字幅が既定値未満
            if(nameLabel.size().width() < LIMIT_NAME_PIXEL_SIZE){
                // チーム名として連結
                clientName += nextChar;
                // 行テキスト追記
                lineText += nextChar;
            // 改行分割未実施
            } else if (!insertNewLineFlag){
                // 改行分割実施
                insertNewLineFlag = true;
                // 改行追加
                clientName += "\n";
                // チーム名として連結
                clientName += nextChar;
                // 行テキスト初期化
                lineText = nextChar;
            } else {
                // ループ脱出
                break;
            }

        }
        // チーム名取得
        this->Name = clientName;
        // TCPクライアント受信のシグナル･スロット解除
        disconnect(this->client, &QTcpSocket::readyRead, this, &TCPClient::GetTeamName);
        // チーム名送信
        emit WriteTeamName();
        // 準備完了
        emit Ready();
        // 戻り値[チーム名]
        return this->Name;
    }
    // 戻り値[チーム名]
    return this->Name;
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
TCPClient::~TCPClient()
{
    // 接続待機中
    if (isConnecting()) {
        // ソケットクローズ
        CloseSocket();
    }
    // TCPクライアント破棄
    delete this->client;
    // TCPサーバー破棄
    delete this->server;
}

/****************************************************************************
*   接続待機確認
*
*   @return 接続待機状態
****************************************************************************/
bool TCPClient::isConnecting()
{
    // 戻り値[接続待機状態]
    return this->server->isListening();
}
