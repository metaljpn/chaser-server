/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   メイン画面
****************************************************************************/
#include "mainwindow.h"                 // メイン画面
#include "ui_mainwindow.h"              // メイン画面UI
#include <QSettings>                    // アプリケーション設定情報
#include <QFileInfo>                    // ファイルシステム
#include <QMediaPlayer>                 // メディアファイル再生
#include <QAudioOutput>                 // 音声出力チャンネル
#include <QRandomGenerator>             // 乱数発生

// staticメンバ変数
MainWindow* MainWindow::s_instance = nullptr;
QtMessageHandler MainWindow::s_prevMsgHandler = nullptr;

/****************************************************************************
*   日時取得
*
*   @return 括弧付き現在日時文字列
****************************************************************************/
QString MainWindow::getTime()
{
    // 戻り値[括弧付き現在日時文字列]
    return QString("[") + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + QString("]");
}

/****************************************************************************
*   クライアント行動文字列変換
*
*   @return 動作文字列
*
*   @param method クライアント行動情報
****************************************************************************/
QString MainWindow::convertString(GameSystem::Method method)
{
    QString str;                        // 文字列

    // 行動内容に対応した文字列を取得
    if(method.action == GameSystem::Method::ACTION::GETREADY)str += "GetReady";
    if(method.action == GameSystem::Method::ACTION::LOOK)    str += "Look";
    if(method.action == GameSystem::Method::ACTION::PUT)     str += "Put";
    if(method.action == GameSystem::Method::ACTION::SEARCH)  str += "Search";
    if(method.action == GameSystem::Method::ACTION::WALK)    str += "Walk";

    // 方向に対応した文字列を取得
    if(method.rote == GameSystem::Method::ROTE::UP)   str += "Up";
    if(method.rote == GameSystem::Method::ROTE::RIGHT)str += "Right";
    if(method.rote == GameSystem::Method::ROTE::LEFT) str += "Left";
    if(method.rote == GameSystem::Method::ROTE::DOWN) str += "Down";

    // 戻り値[動作文字列]
    return str;
}

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // UI設定
    ui->setupUi(this);
    // 起動画面生成
    this->startup = new StartupDialog();
    // 勝者初期化
    this->win = GameSystem::WINNER::CONTINUE;

    // 起動画面実行
    if(this->startup->exec()){
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 初期位置取得
            this->ui->Field->team_pos[i] = this->startup->map.team_first_point[i];
            // デバッグ出力
            qDebug() << this->ui->Field->team_pos[i];
        }
        // UI初期化
        this->ui->Field  ->setMap(this->startup->map);          // マップ設定
        this->ui->TimeBar->setMaximum(this->startup->map.turn); // 進捗バー最大値
        this->ui->TimeBar->setValue  (this->startup->map.turn); // 進捗バー現在値
        // 残りターン数
        this->ui->TurnLabel->setText("残りターン : " + QString::number(this->ui->TimeBar->value()));
        // COOL名
        this->ui->CoolNameLabel->setText(
            this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->Name == "" ?
                "Cool" :
                this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->Name);
        // HOT名
        this->ui->HotNameLabel->setText(
            this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT )]->client->Name == "" ?
                "Hot" :
                this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT )]->client->Name);

    }else{
        // アプリケーション終了
        exit(0);
    }

    QString path;                       // ログ保存パス
    QSettings* mSettings;               // サーバー設定
    QVariant v;                         // 設定値
    // サーバー設定
    mSettings = new QSettings( "setting.ini", QSettings::IniFormat );
    // ログ保存パス
    v = mSettings->value( "LogFilepath" );
    if (v.typeId() != QMetaType::UnknownType)path = v.toString();
    // フレームレート
    v = mSettings->value( "Gamespeed" );
    if (v.typeId() != QMetaType::UnknownType)FRAME_RATE = v.toInt();
    // 消音モード
    v = mSettings->value( "Silent" );
    if (v.typeId() != QMetaType::UnknownType)silent = v.toBool();
    else silent = false;
    // チーム配置時間
    v = mSettings->value( "Team" );
    if (v.typeId() != QMetaType::UnknownType)anime_team_time = v.toInt();

    QSettings* dSettings;               // デザイン設定
    QVariant v2;                        // 設定値
    // デザイン設定
    dSettings = new QSettings( "design.ini", QSettings::IniFormat );
    // 暗転モード
    v2 = dSettings->value( "Dark" );
    if (v2.typeId() != QMetaType::UnknownType)dark = v2.toBool();
    else dark = false;
    // ボット戦モード
    v2 = dSettings->value( "Bot" );
    if (v2.typeId() != QMetaType::UnknownType)isbotbattle = v2.toBool();
    else isbotbattle = false;
    // 暗転モードならば、マップ描画時間を調整
    if(dark == true)this->anime_map_time -= this->anime_blind_time;

    // ボット戦モード
    if(this->isbotbattle){
        // 最高点数ラベル名変更
        this->ui->HotScoreLabel ->setText(QString::number(this->startup->map.turn) + "(ITEM:0)");
        this->ui->CoolScoreLabel->setText(QString::number(this->startup->map.turn) + "(ITEM:0)");
    }else{
        // 点数初期化
        this->ui->HotScoreLabel ->setText("0");
        this->ui->CoolScoreLabel->setText("0");
    }

    // ログファイルオープン
    if(path == "")path = ".";
    log = StableLog(path + "/log" + getTime() + ".txt");

    // メッセージハンドラ設定
	s_instance = this;
	s_prevMsgHandler = qInstallMessageHandler(MainWindow::s_messageHandler);

    // 画面の最大高さをもとに、ウインドウの最大高さを決める
    int window_height = static_cast<int>(QGuiApplication::primaryScreen()->size().height()*0.8);
    // 画面のサイズに合わせてリサイズ
    resize(QSize(window_height+350, window_height));

    // 次ターン行動チーム初期化
    player = 0;

    // タイマー
    startup_anime = new QTimer();
    // マップ描画時シグナル･スロット設定
    connect(startup_anime, &QTimer::timeout, this, &MainWindow::StartAnimation);
    // マップ描画間隔設定
    startup_anime->start(anime_map_time / (startup->map.size.x()*startup->map.size.y()));

    // メディア再生
    bgm = new QMediaPlayer;
    // オーディオ出力チャンネル
    audio_output = new QAudioOutput;

    // 消音モード and BGM有効
    if(!silent && this->startup->music_text != "None"){
        // 出力チャンネル設定
        bgm->setAudioOutput(audio_output);
        // BGMファイル設定
        bgm->setSource(QUrl::fromLocalFile("./Music/" + this->startup->music_text));
        // 音量設定
        audio_output->setVolume(audio_volume);
        // 無限再生
        bgm->setLoops(QMediaPlayer::Infinite);
        // 再生開始
        bgm->play();
    }
    // BGMログ出力
    log << "[ Music : " + this->startup->music_text + " ]" + "\r\n";

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 位置初期化
        ui->Field->team_pos[i].setX(-1);
        ui->Field->team_pos[i].setY(-1);
    }

    // マップ縦サイズ分
    for(int i=0;i<startup->map.size.y();i++){
        // マップ横サイズ分
       for(int j=0;j<startup->map.size.x();j++){
           // アイテム数取得
           if(startup->map.field[i][j] == GameSystem::MAP_OBJECT::ITEM)this->ui->Field->leave_items++;
        }
    }
    // アイテム数表示
    ui->ItemLeaveLabel->setText(QString::number(this->ui->Field->leave_items));

    // 最大化設定取得
    v = mSettings->value( "Maximum" );
    // 設定有効 and ウィンドウ最大化
    if (v.typeId() != QMetaType::UnknownType && v.toBool()){
        // ウィンドウ最大化
        setWindowState(Qt::WindowMaximized);

    }


    // アニメーション設定
    mSettings = new QSettings( "AnimationTime.ini", QSettings::IniFormat );
    // マップ描画時間
    v = mSettings->value( "Map" );
    // マップ描画時間設定
    if (v.typeId() != QMetaType::UnknownType)anime_map_time = v.toInt();
    else{
        QSettings* mSettings;           // アニメーション設定
        // アニメーション設定
        mSettings = new QSettings( "AnimationTime.ini", QSettings::IniFormat );

        // マップ描画時間
        mSettings->setValue( "Map" , anime_map_time );

    }

    // プレイヤー情報ログ出力(プレイヤー名、IPアドレス)
    log << "[ Cool Player : Name = " + this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->Name
        + " , IP = " + this->startup->team_client[static_cast<int>(GameSystem::TEAM::COOL)]->client->IP
        + " ]\r\n";
    log << "[ Hot  Player : Name = " + this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT)]->client->Name
        + " , IP = " + this->startup->team_client[static_cast<int>(GameSystem::TEAM::HOT)]->client->IP
        + " ]\r\n";

    // マップ情報ログ出力
    log << "[ Map : \r\n";
    ui->Field->field.Export(log.filename());
    log << "]\r\n";

    // セットアップ完了ログ出力
    log << getTime() + "セットアップ完了　ゲームを開始します。\r\n";    
}

/****************************************************************************
*   メッセージハンドラ
*
*   @param type    メッセージタイプ
*   @param context ログメッセージ追加情報
*   @param msg     メッセージ文字列
****************************************************************************/
void MainWindow::s_messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // 再入検出（同スレッド内での再帰呼び出しを防ぐ）
    static thread_local bool inHandler = false;
    // 再入
    if (inHandler) {
        // 既にハンドラ内なら前のハンドラだけ呼ぶ（再帰で無限ループするのを防ぐ）
        if (s_prevMsgHandler) s_prevMsgHandler(type, context, msg);
        return;
    }

    // ハンドラ実行中
    inHandler = true;

    // インスタンス有効
    if (s_instance) {
        // メッセージタイプにより分岐
        switch (type) {
        case QtWarningMsg:              // 警告
            s_instance->log << getTime() + "Warning : " + msg + "\r\n";
            break;
        case QtCriticalMsg:             // 重大
            s_instance->log << getTime() + "Critical Error : " + msg + "\r\n";
            break;
        case QtFatalMsg:                // 致命的
            s_instance->log << getTime() + "Fatal Error : " + msg + "\r\n";
            break;
        default:
            // その他のメッセージタイプは無視
            break;
        }
    }

    // 既存のメッセージハンドラを実行
    if (s_prevMsgHandler) {
        s_prevMsgHandler(type, context, msg);
    }

    // 異常の場合は終了
    if (type == QtFatalMsg) abort();

    // ハンドラ実行終了
    inHandler = false;
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
MainWindow::~MainWindow()
{
    // ゲーム停止
    this->clock->stop();

    // BGM停止
    if(!silent && this->startup->music_text != "None")bgm->stop();

    // メッセージハンドラ復旧
	qInstallMessageHandler(s_prevMsgHandler);
    s_prevMsgHandler = nullptr;
    s_instance = nullptr;

    // サーバー再起動確認
    auto ret = QMessageBox::information(this, "", tr("続けてサーバーを起動しますか。"), QMessageBox::Yes, QMessageBox::No);

    // UI破棄
    delete ui;

    // サーバー再起動
    if(ret == QMessageBox::Yes){
        // 引数取得
        QStringList argv = QApplication::arguments();

        // サーバーのファイルパスを取得
        QString command = argv.takeAt(0);

        // MainWindowを閉じた後に、
        // コマンドライン引数付きでサーバー再起動
        QProcess::startDetached(command, argv);
    }

}

/****************************************************************************
*   ゲーム進行(1ターン)
****************************************************************************/
void MainWindow::StepGame()
{
    // クライアント行動情報
    static GameSystem::Method team_mehod[TEAM_COUNT];
    // フィールドオーバーレイ(行動効果)消去
    this->ui->Field->RefreshOverlay();
    static int turn_count;              // ターン数
    static bool getready_flag=true;     // ターン状態

    // ターンログ出力
    if(ui->TimeBar->value() != turn_count){
       // ターン数取得
       turn_count = ui->TimeBar->value();
       // ターン数ログ出力
       log << QString("-----残") + QString::number(turn_count) + "ターン-----" + "\r\n";
       // デバッグ情報
       qDebug() << QString("-----残") + QString::number(turn_count) + "ターン-----";
    }

    // ターン開始
    if(getready_flag){
        // 開始異常
        if(!startup->team_client[player]->client->WaitGetReady()){
            // 異常ログ出力
            log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\r\n";
            // 切断
            startup->team_client[player]->client->is_disconnected = true;
        }else{
            // フィールド周辺確認
            GameSystem::AroundData buffer = ui->Field->FieldAccessAround(GameSystem::Method{static_cast<GameSystem::TEAM>(player),
                                                                         GameSystem::Method::ACTION::GETREADY,
                                                                         GameSystem::Method::ROTE::UNKNOWN},
                                                                         ui->Field->team_pos[player]);
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnMethod(buffer);


            // ターン開始動作
            if(team_mehod[player].action == GameSystem::Method::ACTION::GETREADY){
                // 異常ログ出力
                log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が二度GetReadyを行いました!" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }
            // チーム取得
            team_mehod[player].team = static_cast<GameSystem::TEAM>(player);
        }
        // アイテム回収
        RefreshItem(team_mehod[player]);
        // 勝者判定
        this->win = Judge();
        // 勝敗決定
        if(win != GameSystem::WINNER::CONTINUE){
            // チーム更新
            player++;
            player %= TEAM_COUNT;

            // ターン開始
            startup->team_client[player]->client->WaitGetReady();
            // フィールド周辺確認
            GameSystem::AroundData buffer = ui->Field->FieldAccessAround(GameSystem::Method{static_cast<GameSystem::TEAM>(player),
                                                                         GameSystem::Method::ACTION::GETREADY,
                                                                         GameSystem::Method::ROTE::UNKNOWN},
                                                                         ui->Field->team_pos[player]);
            // 接続終了
            buffer.connect = GameSystem::CONNECTING_STATUS::FINISHED;
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnMethod(buffer);
            // 終了
            Finish(win);
        }

    }else{

        // 行動後フィールド周辺情報取得
        GameSystem::AroundData around = ui->Field->FieldAccessMethod(team_mehod[player]);
        // アイテム回収
        RefreshItem(team_mehod[player]);
        // 勝者判定
        this->win = Judge();
        // 勝敗決定
        if(this->win != GameSystem::WINNER::CONTINUE){
            // 接続終了
            around.connect = GameSystem::CONNECTING_STATUS::FINISHED;
        }
        // 周辺情報受信完了待機
        if(startup->team_client[player]->client->WaitEndSharp(around)){
            // 行動不明
            if(team_mehod[player].action == GameSystem::Method::ACTION::UNKNOWN){
                // 行動不明ログ出力
                log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が不正なメソッドを呼んでいます！" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }
            // 方向不明
            if(team_mehod[player].rote   == GameSystem::Method::ROTE::UNKNOWN){
                // 方向不明ログ出力
                log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "の行動メソッドが不正な方向を示しています！" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }

            // 行動ログ出力
            log << getTime() + "[行動]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が" + convertString(team_mehod[player]) + "を行いました。" << "\r\n";

            // フィールド情報
            GameBoard*& board = this->ui->Field;
            // チーム周辺情報
            GameSystem::AroundData team_around = board->FieldAccessAround(static_cast<GameSystem::TEAM>(player));
            // チーム周辺情報ログ出力
            log << getTime() + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + ":" + team_around.toString() << "\r\n";

            // 最終チーム
            if(player ==  TEAM_COUNT-1){
                // 進捗バー更新
                ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
                ui->TimeBar->repaint();
                // 残りターン表示
                this->ui->TurnLabel->setText("残りターン : " + QString::number(ui->TimeBar->value()));

                // ボット戦モード
                if(this->isbotbattle){
                    // COOL側スコア取得
                    int ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::COOL)];
                    // COOL側スコア表示
                    ui->CoolScoreLabel->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
                    // HOT側スコア取得
                    ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::HOT)];
                    // HOT側スコア表示
                    ui->HotScoreLabel ->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");

                }
            }
        }else{
            // 周辺情報受信異常ログ出力
            log << getTime() + "[停止]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\r\n";
            // 切断
            startup->team_client[player]->client->is_disconnected = true;
        }

        // チーム更新
        player++;
        player %= TEAM_COUNT;
        // 勝敗決定
        if(win != GameSystem::WINNER::CONTINUE){
            // 周辺情報要求
            startup->team_client[player]->client->WaitGetReady();
            // フィールド周辺確認
            GameSystem::AroundData buffer = ui->Field->FieldAccessAround(GameSystem::Method{static_cast<GameSystem::TEAM>(player),
                                                                         GameSystem::Method::ACTION::GETREADY,
                                                                         GameSystem::Method::ROTE::UNKNOWN},
                                                                         ui->Field->team_pos[player]);
            // 接続終了
            buffer.connect = GameSystem::CONNECTING_STATUS::FINISHED;
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnMethod(buffer);
            // 終了
            Finish(win);
        }

    }

    // フィールド再描画
    ui->Field->repaint();
    // ターン状態更新
    getready_flag = !getready_flag;
}

/****************************************************************************
*   アイテム取得判定
*
*   @param method クライアント行動情報
****************************************************************************/
void MainWindow::RefreshItem(GameSystem::Method method)
{
    // 前回アイテム数
    static int leave_item = 0;
    // アイテム数取得
    if(leave_item == 0)leave_item = this->ui->Field->leave_items;
    // アイテム数が変化
    if(this->ui->Field->leave_items != leave_item){
        // 残りアイテム数表示
        ui->ItemLeaveLabel->setText(QString::number(this->ui->Field->leave_items));
        // アイテム取得をログ出力
        log << getTime() + "[取得]" + GameSystem::TEAM_PROPERTY::getTeamName(method.team) + "がアイテムを取得しました。" << "\r\n";
        // ボット戦
        if(this->isbotbattle){
            // COOL側点数表示
            int ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::COOL)];
            ui->CoolScoreLabel->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
            // HOT側点数表示
            ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::HOT)];
            ui->HotScoreLabel ->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }else{
            // 点数表示
            ui->CoolScoreLabel->setText(QString::number(this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::COOL)]));
            ui->HotScoreLabel ->setText(QString::number(this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::HOT)]));
        }
        // 前回アイテム数更新
        leave_item = this->ui->Field->leave_items;
    }


}

/****************************************************************************
*   勝敗通知
*
*   @param winner 勝者
****************************************************************************/
void MainWindow::Finish(GameSystem::WINNER winner)
{
    // ゲーム停止
    this->clock->stop();
    // 通信切断情報
    QString append_str = "";
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 通信切断
        if(startup->team_client[i]->client->is_disconnected){
            append_str.append("\r\n[" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + " 切断により]");
            // 通信切断による終了をログ出力
            log << getTime() + "[終了]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + "との通信が切断されています。" << "\r\n";
        }
    }
    // 勝者をログ出力
    log << this->ui->WinnerLabel->text() << "\r\n";

    // BGM停止
    if(!silent && this->startup->music_text != "None")bgm->stop();

    // 消音モード以外
    if(!silent){
        // 試合終了時のファンファーレを再生
        bgm = new QMediaPlayer;
        audio_output = new QAudioOutput;
        bgm->setAudioOutput(audio_output);
        bgm->setSource(QUrl("qrc:/Sound/ji_023.mp3"));
        audio_output->setVolume(audio_volume);
        bgm->play();
    }

    // 勝者=COOL
    if(winner == GameSystem::WINNER::COOL){
        // 勝利通知
        this->ui->WinnerLabel->setText("COOL WIN!" + append_str);
        // 勝利ログ出力
        log << getTime() + "[決着]COOLが勝利しました。" << "\r\n";
        // ボット戦
        if(this->isbotbattle){
            // 負けチームのスコア更新（ターン数分減らす）
            int ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::HOT)];
            ui->HotScoreLabel ->setText(QString::number(ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }
    }
    // 勝者=HOT
    if(winner == GameSystem::WINNER::HOT){
        // 勝利通知
        this->ui->WinnerLabel->setText("HOT WIN!"  + append_str);
        // 勝利ログ出力
        log << getTime() + "[決着]HOTが勝利しました。" << "\r\n";
        // ボット戦
        if(this->isbotbattle){
            // 負けチームのスコア更新（ターン数分減らす）
            int ScoreBuf = this->ui->Field->team_score[static_cast<int>(GameSystem::TEAM::COOL)];
            ui->CoolScoreLabel->setText(QString::number(ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }
    }
    // 引き分け
    if(winner == GameSystem::WINNER::DRAW){
        // 引き分け通知
        this->ui->WinnerLabel->setText("DRAW");
        // 引き分けログ出力
        log << getTime() + "[決着]引き分けです。" << "\r\n";
    }
}

/****************************************************************************
*   勝敗判定
*
*   @return 勝者(要素No.)
****************************************************************************/
GameSystem::WINNER MainWindow::Judge()
{
    bool team_lose[TEAM_COUNT];         // 敗者チーム
    int _player = player;               // 次ターンのチーム
    // 敗者チーム初期化
    for(int i=0;i<TEAM_COUNT;i++)team_lose[i] = false;
    // ボード盤
    GameBoard*& board = this->ui->Field;

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 現在のチームを取得
        _player = (_player + 1) % TEAM_COUNT;
        // フィールド周辺確認
        GameSystem::AroundData team_around = board->FieldAccessAround(static_cast<GameSystem::TEAM>(_player));

        // ブロック下敷き
        if(team_around.data[4] == GameSystem::MAP_OBJECT::BLOCK){
            // 敗因ログ出力
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(_player)) + "ブロック下敷き" << "\r\n";
            // ゲーム終了
            team_around.finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

        // ブロック囲まれ
        if(team_around.data[1] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[3] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[5] == GameSystem::MAP_OBJECT::BLOCK &&
           team_around.data[7] == GameSystem::MAP_OBJECT::BLOCK){
            // 敗因ログ出力
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(_player)) + "ブロック囲まれ" << "\r\n";
            // ゲーム終了
            team_around.finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

        // 通信切断
        if(startup->team_client[_player]->client->is_disconnected){
            // 敗因ログ出力
            log << getTime() + "[死因]" + GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(_player)) + "通信切断" << "\r\n";
            // ゲーム終了
            team_around.finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

    }

    // 相打ち or 時間切れ
    if(!std::find(team_lose,team_lose+TEAM_COUNT,false) || ui->TimeBar->value()==0){
        // アイテム判定ログ出力
        log << getTime() + "[情報]相打ちまたは、タイムアップのためアイテム判定を行います" + "\r\n";
        log << getTime() + "[得点]";
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // チーム名､得点をログ出力
            log << GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i)) + ":" + QString::number(this->ui->Field->team_score[i]) + "  ";
            // 敗者チーム初期化
            team_lose[i] = false;
        }
        // 改行ログ出力
        log << "\r\n";

        QSet<int> team_score_set;       // 得点
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 得点取得
            team_score_set.insert(this->ui->Field->team_score[i]);
        }
        // 同点ならば、戻り値[引き分け]
        if(team_score_set.size()==1)return GameSystem::WINNER::DRAW;

        int index=0;                    // 勝者
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 最高得点更新ならば勝者
            if(this->ui->Field->team_score[index] < this->ui->Field->team_score[i])index = i;
        }
        // 戻り値[勝者]
        return static_cast<GameSystem::WINNER>(index);
    }

    // 戻り値[HOT]
    if(team_lose[0])return GameSystem::WINNER::HOT;
    // 戻り値[COOL]
    else if(team_lose[1])return GameSystem::WINNER::COOL;
    // 戻り値[継続]
    else return GameSystem::WINNER::CONTINUE;
}

/****************************************************************************
*   マップ描画
****************************************************************************/
void MainWindow::StartAnimation()
{
    // 描画数
    static int timer = 1;
    // フィールド情報
    static Field<GameSystem::MAP_OVERLAY> f(this->startup->map.size.y(),
                                            QVector<GameSystem::MAP_OVERLAY>(this->startup->map.size.x(),GameSystem::MAP_OVERLAY::ERASE));
    // アニメーション種類
    static int ANIMATION_SIZE = 4;
    // アニメーションタイプ
    static int ANIMATION_TYPE = QRandomGenerator::global()->generate() % ANIMATION_SIZE;

    // カウンタ初期化
    int count = 0;

    // フィールドオーバーレイ(行動効果)消去
    ui->Field->RefreshOverlay();

    // 位置
    QPoint pos[2];
    // ランダム座標でフィールド描画
    if(ANIMATION_TYPE == 0){
        // 表示数
        int i_max = 2;
        // 初回ならばループ上限補正
        if(timer == 1)i_max ++;
        // 指定個数分
        for(int i=0;i<i_max;i++){
            do{
                // ランダム座標取得
                pos[i].setX(QRandomGenerator::global()->generate() % this->startup->map.size.x());
                pos[i].setY(QRandomGenerator::global()->generate() % this->startup->map.size.y());
            // マップサイズ分の回数未到達 and 対象位置が未選択以外の間
            }while(timer < startup->map.size.x() * startup->map.size.y() && f[pos[i].y()][pos[i].x()] != GameSystem::MAP_OVERLAY::ERASE);
            // フィールド状態=無し(検出)
            f[pos[i].y()][pos[i].x()] = GameSystem::MAP_OVERLAY::NOTHING;
        }
        // マップY軸数分
        for(int i=0;i<this->startup->map.size.y();i++){
            // マップX軸数分
            for(int j=0;j<this->startup->map.size.x();j++){
                // フィールドオーバーレイ状態設定
                this->ui->Field->overlay[i][j] = f[i][j];
            }
        }
    // 上から下に向かってフィールド描画
    }else if(ANIMATION_TYPE == 1){
        // マップY軸数分
        for(int j=0;j<this->startup->map.size.y();j++){
            // マップX軸数分
            for(int k=0;k<this->startup->map.size.x();k++){
                // 初回以外
                if(count >= timer){
                    // フィールドオーバーレイ状態設定
                    this->ui->Field->overlay[j][k] = f[j][k];
                }
                // カウンタ更新
                count++;
            }
        }
    // 上下から中心に向かってフィールド描画
    }else if(ANIMATION_TYPE == 2){
        // マップY軸数分
        for(int j=0;j<this->startup->map.size.y();j++){
            // マップX軸数分
            for(int k=0;k<this->startup->map.size.x();k++){
                // 描画数分
                if(count*2 < timer){
                    // 下側フィールド状態=無し(検出)
                    f[startup->map.size.y() - j - 1][startup->map.size.x() - k - 1] = GameSystem::MAP_OVERLAY::NOTHING;
                    // 上側フィールド状態=無し(検出)
                    f[j][k] = GameSystem::MAP_OVERLAY::NOTHING;
                }
                // カウンタ更新
                count++;
            }
        }

        // マップY軸数分
        for(int i=0;i<this->startup->map.size.y();i++){
            // マップX軸数分
            for(int j=0;j<this->startup->map.size.x();j++){
                // フィールドオーバーレイ状態設定
                this->ui->Field->overlay[i][j] = f[i][j];
            }
        }
    // 下から上に向かってフィールド描画
    }else if(ANIMATION_TYPE == 3){
        // マップY軸数分
        for(int j=this->startup->map.size.y()-1;j>=0;j--){
            // マップX軸数分
            for(int k=this->startup->map.size.x()-1;k>=0;k--){
                // 初回以外
                if(count >= timer){
                    // フィールドオーバーレイ状態設定
                    this->ui->Field->overlay[j][k] = f[j][k];
                }
                // カウンタ更新
                count++;
            }
        }
    }

    // フィールド描画完了
    if(timer >= startup->map.size.x() * startup->map.size.y()){
        // タイマー
        teamshow_anime = new QTimer();
        // チーム配置シグナル･スロット設定
        connect(teamshow_anime, &QTimer::timeout, this, &MainWindow::ShowTeamAnimation);
        // タイマー始動
        teamshow_anime->start(anime_team_time/TEAM_COUNT);
        // シグナル･スロット解除
        disconnect(startup_anime, &QTimer::timeout, this, &MainWindow::StartAnimation);
    }
    // 描画数更新
    timer += 2;
    // 描画更新
    repaint();
}

/****************************************************************************
*   チーム配置
****************************************************************************/
void MainWindow::ShowTeamAnimation()
{
    static int team_count;              // カウンタ

    // チーム初期位置取得
    ui->Field->team_pos[team_count] = this->startup->map.team_first_point[team_count];

    // チーム数到達
    if(team_count == TEAM_COUNT){
        // 暗転モード
        if(dark == true){
            // タイマー
            blind_anime = new QTimer();
            // 暗転描画シグナル･スロット設定
            connect(blind_anime, &QTimer::timeout, this, &MainWindow::BlindAnimation);
            // タイマー始動
            blind_anime->start(anime_blind_time / (startup->map.size.x()*startup->map.size.y()));
            // シグナル･スロット解除
            disconnect(teamshow_anime, &QTimer::timeout, this, &MainWindow::ShowTeamAnimation);
        }else{
            // タイマー
            clock = new QTimer();
            // ゲーム進行シグナル･スロット設定
            connect(clock, &QTimer::timeout, this, &MainWindow::StepGame);
            // タイマー始動
            clock->start(FRAME_RATE);
            // シグナル･スロット解除
			disconnect(teamshow_anime, &QTimer::timeout, this, &MainWindow::ShowTeamAnimation);
        }
    }else{
        // 探索済設定
        ui->Field->field.discover[ui->Field->team_pos[team_count].y()]
                [ui->Field->team_pos[team_count].x()] = GameSystem::Discoverer::Cool;
    }
    // 描画更新
    repaint();
    // カウンタ更新
    team_count++;
}

/****************************************************************************
*   暗転描画
****************************************************************************/
void MainWindow::BlindAnimation()
{
    // カウンタ
    static int timer = 1;
    // アニメーション種類
    static int ANIMATION_SIZE = 1;
    // アニメーションタイプ
    static int ANIMATION_TYPE = QRandomGenerator::global()->generate() % ANIMATION_SIZE;

    // フィールドオーバーレイ(行動効果)消去
    ui->Field->RefreshOverlay();

    // 位置
    QPoint pos[2];
    // タイプ0
    if(ANIMATION_TYPE == 0){
        // 座標取得回数分
        for(int i=0;i<2;i++){
            do{
                // ランダム座標取得
                pos[i].setX(QRandomGenerator::global()->generate() % this->startup->map.size.x());
                pos[i].setY(QRandomGenerator::global()->generate() % this->startup->map.size.y());
            // マップサイズ分の回数未到達 and 未探索(座標取得済)の間
            }while(timer < startup->map.size.x() * startup->map.size.y() &&
                   ui->Field->field.discover[pos[i].y()][pos[i].x()] == GameSystem::Discoverer::Unknown);
            // 未探索設定
            ui->Field->field.discover[pos[i].y()][pos[i].x()] = GameSystem::Discoverer::Unknown;
        }
    }
    // マップサイズ分の回数到達
    if(timer >= startup->map.size.x() * startup->map.size.y()){
        // 探索状態初期化
        for(auto& v : this->ui->Field->field.discover)v = QVector<GameSystem::Discoverer>
                (this->ui->Field->field.size.x(),GameSystem::Discoverer::Unknown);
        // タイマー
        clock = new QTimer();
        // ゲーム進行シグナル･スロット設定
        connect(clock, &QTimer::timeout, this, &MainWindow::StepGame);
        // タイマー始動
        clock->start(FRAME_RATE);
        // シグナル･スロット解除
        disconnect(blind_anime, &QTimer::timeout, this,&MainWindow::BlindAnimation);
    }
    // カウンタ更新
    timer += 2;
    // 描画更新
    repaint();
}
