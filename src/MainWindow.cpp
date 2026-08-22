/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   メイン画面
****************************************************************************/
#include "MainWindow.h"                 // メイン画面
#include "ui_MainWindow.h"              // メイン画面UI

#include <QAudioOutput>                 // 音声出力チャンネル
#include <QDateTime>                    // 日付･時刻
#include <QMessageBox>                  // メッセージボックス
#include <QRandomGenerator>             // 乱数発生
#include <QScreen>                      // 画面プロパティ
#include <QSettings>                    // アプリケーション設定情報
#include <QTimer>                       // 時間管理

// staticメンバ初期化
MainWindow* MainWindow::s_instance = nullptr;
QtMessageHandler MainWindow::s_prevMsgHandler = nullptr;

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
    // HOT情報非表示
    ui->HotGroup->hide();

    // 起動画面生成
    this->startup = new StartupDialog();
    // 勝者初期化
    this->win = WINNER::CONTINUE;

    // 起動画面実行
    if(this->startup->exec()){
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 初期位置取得
            this->ui->Board->team_pos[i] = this->startup->map.team_first_point[i];
            // デバッグ出力
            qDebug() << this->ui->Board->team_pos[i];
        }

        // UI初期化
        this->ui->Board->setMap(this->startup->map);            // マップ設定
        this->ui->TimeBar->setMaximum(this->startup->map.turn); // 進捗バー最大値
        this->ui->TimeBar->setValue  (this->startup->map.turn); // 進捗バー現在値
        // 残りターン数
        this->ui->TurnLabel->setText("残りターン : " + QString::number(this->ui->TimeBar->value()));
        // COOL名
        this->ui->CoolNameLabel->setText(
            this->startup->team_client[static_cast<int>(TEAM::COOL)]->client->Name == "" ?
                "Cool" :
                this->startup->team_client[static_cast<int>(TEAM::COOL)]->client->Name);
    }else{
        // アプリケーション終了
        exit(0);
    }

    QString path;                       // ログ保存パス
    // サーバー設定
    QSettings mSettings("setting.ini", QSettings::IniFormat);
    QVariant v;                         // 設定値
    // ログ保存パス
    v = mSettings.value( "LogFilepath" );
    if (v.typeId() != QMetaType::UnknownType)path = v.toString();
    // フレームレート
    v = mSettings.value( "Gamespeed" );
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)frame_rate = v.toInt();
    // 消音モード
    v = mSettings.value( "Silent" );
    if (v.typeId() != QMetaType::UnknownType)silent = v.toBool();
    else silent = false;

    // デザイン設定
    QSettings dSettings("design.ini", QSettings::IniFormat);
    QVariant v2;                        // 設定値
    // 暗闇モード
    v2 = dSettings.value( "Dark" );
    if (v2.typeId() != QMetaType::UnknownType)dark = v2.toBool();
    else dark = false;
    // ボット戦モード
    v2 = dSettings.value( "Bot" );
    if (v2.typeId() != QMetaType::UnknownType)isbotbattle = v2.toBool();
    else isbotbattle = false;
    // 暗闇モードならば、マップ描画時間を調整
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
    connect(startup_anime, &QTimer::timeout, this, &MainWindow::DrawMapAnimation);
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
        ui->Board->team_pos[i].setX(-1);
        ui->Board->team_pos[i].setY(-1);
    }

    // マップ縦サイズ分
    for(int i=0;i<startup->map.size.y();i++){
        // マップ横サイズ分
       for(int j=0;j<startup->map.size.x();j++){
            // アイテム数取得
            if(startup->map.field[i][j] == GameMap::OBJECT::ITEM)this->ui->Board->leave_items++;
        }
    }

    // アイテム数表示
    ui->ItemLeaveLabel->setText(QString::number(this->ui->Board->leave_items));

    // 最大化設定取得
    v = mSettings.value( "Maximum" );
    // 設定有効 and ウィンドウ最大化
    if (v.typeId() != QMetaType::UnknownType && v.toBool())
        // ウィンドウ最大化
        setWindowState(Qt::WindowMaximized);
    

    // アニメーション設定
    QSettings aSettings( "AnimationTime.ini", QSettings::IniFormat );
    // マップ描画時間
    v = aSettings.value( "Map" );
    // マップ描画時間設定
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        anime_map_time = v.toInt();
    // チーム配置時間
    v = aSettings.value("Team");
    if (v.typeId() != QMetaType::UnknownType && v.toInt() > 0)
        anime_team_time = v.toInt();

    // プレイヤー情報ログ出力(プレイヤー名、IPアドレス)
    log << "[ Cool Player : Name = " + this->startup->team_client[static_cast<int>(TEAM::COOL)]->client->Name
               + " , IP = " + this->startup->team_client[static_cast<int>(TEAM::COOL)]->client->IP
        + " ]\r\n";
    log << "[ Hot  Player : Name = " + this->startup->team_client[static_cast<int>(TEAM::HOT)]->client->Name
               + " , IP = " + this->startup->team_client[static_cast<int>(TEAM::HOT)]->client->IP
        + " ]\r\n";

    // マップ情報ログ出力
    log << "[ Map : \r\n";
    ui->Board->map.Export(log.filename());
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
*   マップ描画
****************************************************************************/
void MainWindow::DrawMapAnimation()
{
    // 描画数
    static int timer = 1;
    // 描画対象/描画無効情報
    static Field<GameMap::EFFECT> f(this->startup->map.size.y(),
                                    QVector<GameMap::EFFECT>(this->startup->map.size.x(),
                                    GameMap::EFFECT::ERASE));
    // アニメーション種類
    static int ANIMATION_SIZE = 4;
    // アニメーションタイプ
    static int ANIMATION_TYPE = QRandomGenerator::global()->generate() % ANIMATION_SIZE;

    // カウンタ初期化
    int count = 0;

    // 行動効果初期化
    ui->Board->ResetEffect();

    // 位置
    QPoint pos[3];
    // ランダム座標でフィールド描画
    if(ANIMATION_TYPE == 0){
        // 表示数
        int i_max = 2;
        // 初回ならばループ上限補正
        if(timer == 1)i_max++;
        // 指定個数分
        for(int i=0;i<i_max;i++){
            do{
                // ランダム座標取得
                pos[i].setX(QRandomGenerator::global()->generate() % this->startup->map.size.x());
                pos[i].setY(QRandomGenerator::global()->generate() % this->startup->map.size.y());
            // マップサイズ分の回数未到達 and 取得座標が選択済の間
            }while(timer < startup->map.size.x() * startup->map.size.y() && f[pos[i].y()][pos[i].x()] != GameMap::EFFECT::ERASE);
            // 描画対象設定
            f[pos[i].y()][pos[i].x()] = GameMap::EFFECT::NOTHING;
        }
        // マップY軸数分
        for(int i=0;i<this->startup->map.size.y();i++){
            // マップX軸数分
            for(int j=0;j<this->startup->map.size.x();j++){
                // 描画対象設定
                this->ui->Board->effect[i][j] = f[i][j];
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
                    // 描画無効対象設定
                    this->ui->Board->effect[j][k] = f[j][k];
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
                    // 下側描画対象設定
                    f[startup->map.size.y() - j - 1][startup->map.size.x() - k - 1] = GameMap::EFFECT::NOTHING;
                    // 上側描画対象設定
                    f[j][k] = GameMap::EFFECT::NOTHING;
                }
                // カウンタ更新
                count++;
            }
        }
        // マップY軸数分
        for(int i=0;i<this->startup->map.size.y();i++){
            // マップX軸数分
            for(int j=0;j<this->startup->map.size.x();j++){
                // 描画対象設定
                this->ui->Board->effect[i][j] = f[i][j];
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
                    // 描画対象設定
                    this->ui->Board->effect[j][k] = f[j][k];
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
        connect(teamshow_anime, &QTimer::timeout, this, &MainWindow::SetTeamAnimation);
        // タイマー始動
        teamshow_anime->start(anime_team_time/TEAM_COUNT);
        // シグナル･スロット解除
        disconnect(startup_anime, &QTimer::timeout, this, &MainWindow::DrawMapAnimation);
    }
    // 描画数更新
    timer += 2;
    // 描画更新
    repaint();
}

/****************************************************************************
*   チーム配置
****************************************************************************/
void MainWindow::SetTeamAnimation()
{
    static int team_count;              // カウンタ

    // チーム数到達
    if((team_count == TEAM_COUNT)||(static_cast<TEAM>(team_count)==TEAM::HOT)){
        // 暗闇モード
        if(dark == true){
            // タイマー
            blind_anime = new QTimer();
            // 暗闇描画シグナル･スロット設定
            connect(blind_anime, &QTimer::timeout, this, &MainWindow::BlindAnimation);
            // タイマー始動
            blind_anime->start(anime_blind_time / (startup->map.size.x()*startup->map.size.y()));
            // シグナル･スロット解除
            disconnect(teamshow_anime, &QTimer::timeout, this, &MainWindow::SetTeamAnimation);
        }else{
            // タイマー
            clock = new QTimer();
            // ゲーム進行シグナル･スロット設定
			connect(clock, &QTimer::timeout, this, &MainWindow::StepGame);
            // タイマー始動
            clock->start(frame_rate);
            // シグナル･スロット解除
            disconnect(teamshow_anime, &QTimer::timeout, this, &MainWindow::SetTeamAnimation);
        }
    }else{
        // チーム初期位置取得
        ui->Board->team_pos[team_count] = this->startup->map.team_first_point[team_count];
        // 探索済設定
        ui->Board->map.discover[ui->Board->team_pos[team_count].y()]
                [ui->Board->team_pos[team_count].x()] = GameMap::DISCOVER::EXPLORED;
    }
    // 描画更新
    repaint();
    // カウンタ更新
    team_count++;
}

/****************************************************************************
*   暗闇描画
****************************************************************************/
void MainWindow::BlindAnimation()
{
    // カウンタ
    static int timer = 1;
    // アニメーション種類
    static int ANIMATION_SIZE = 1;
    // アニメーションタイプ
    static int ANIMATION_TYPE = QRandomGenerator::global()->generate() % ANIMATION_SIZE;

    // 行動効果初期化
    ui->Board->ResetEffect();

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
                   ui->Board->map.discover[pos[i].y()][pos[i].x()] == GameMap::DISCOVER::UNKNOWN);
            // 未探索設定
            ui->Board->map.discover[pos[i].y()][pos[i].x()] = GameMap::DISCOVER::UNKNOWN;
        }
    }
    // マップサイズ分の回数到達
    if(timer >= startup->map.size.x() * startup->map.size.y()){
        // 探索状態初期化
        for(auto& v : this->ui->Board->map.discover)v = QVector<GameMap::DISCOVER>
                (this->ui->Board->map.size.x(),GameMap::DISCOVER::UNKNOWN);
        // タイマー
        clock = new QTimer();
        // ゲーム進行シグナル･スロット設定
        connect(clock, &QTimer::timeout, this, &MainWindow::StepGame);
        // タイマー始動
        clock->start(frame_rate);
        // シグナル･スロット解除
        disconnect(blind_anime, &QTimer::timeout, this,&MainWindow::BlindAnimation);
    }
    // カウンタ更新
    timer += 2;
    // 描画更新
    repaint();
}

/****************************************************************************
*   ゲーム進行(1ターン)
****************************************************************************/
void MainWindow::StepGame()
{
    // クライアント行動情報
    static Operation team_mehod[TEAM_COUNT];
    static int turn_count;              // ターン数
    static bool getready_flag=true;     // ターン状態

    // 行動効果初期化
    this->ui->Board->ResetEffect();

    // ターンログ出力
    if(ui->TimeBar->value() != turn_count){
        // ターン数取得
       turn_count = ui->TimeBar->value();
        // ターン数ログ出力
       log << QString("-----残") + QString::number(turn_count) + "ターン-----" + "\r\n";
        // デバッグ情報
       qDebug() << QString("-----残") + QString::number(turn_count) + "ターン-----";
    }

    player = 0;

    // ターン開始
    if(getready_flag){
        // 開始異常
        if(!startup->team_client[player]->client->WaitGetReady()){
            // 異常ログ出力
            log << getTime() + "[停止]" + getTeamName(static_cast<TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\r\n";
            // 切断
            startup->team_client[player]->client->is_disconnected = true;
        }else{
            // フィールド周辺確認
            AroundData buffer = ui->Board->FieldAccessAround(Operation{static_cast<TEAM>(player),
                                                                                   Operation::ACTION::GETREADY,
                                                                                   Operation::ROTE::UNKNOWN},
                                                             ui->Board->team_pos[player]);
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnOperation(buffer);

            // ターン開始動作
            if(team_mehod[player].action == Operation::ACTION::GETREADY){
                // 異常ログ出力
                log << getTime() + "[停止]" + getTeamName(static_cast<TEAM>(player)) + "が二度GetReadyを行いました!" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }
            // チーム取得
            team_mehod[player].team = static_cast<TEAM>(player);
        }
        // 得点更新
        ScoreUpdate(team_mehod[player]);
        // 勝者判定
        this->win = Judge();
        // 勝敗決定
        if(win != WINNER::CONTINUE){
            // チーム更新
            player++;
            player %= TEAM_COUNT;

            // ターン開始
            startup->team_client[player]->client->WaitGetReady();
            // フィールド周辺確認
            AroundData buffer = ui->Board->FieldAccessAround(Operation{static_cast<TEAM>(player),
                                                                                   Operation::ACTION::GETREADY,
                                                                                   Operation::ROTE::UNKNOWN},
                                                             ui->Board->team_pos[player]);
            // 接続終了
            buffer.connect = AroundData::CONNECTING_STATUS::FINISHED;
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnOperation(buffer);
            // 終了
            Finish(win);
        }
    }else{
        // 行動後フィールド周辺情報取得
        AroundData around = ui->Board->FieldAccessOperation(team_mehod[player]);
        // 得点更新
        ScoreUpdate(team_mehod[player]);
        // 勝者判定
        this->win = Judge();
        // 勝敗決定
        if(this->win != WINNER::CONTINUE){
            // 接続終了
            around.connect = AroundData::CONNECTING_STATUS::FINISHED;
        }
        // 周辺情報受信完了待機
        if(startup->team_client[player]->client->WaitEndSharp(around)){
            // 行動不明
            if(team_mehod[player].action == Operation::ACTION::UNKNOWN){
                // 行動不明ログ出力
                log << getTime() + "[停止]" + getTeamName(static_cast<TEAM>(player)) + "が不正なメソッドを呼んでいます！" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }
            // 方向不明
            if(team_mehod[player].rote   == Operation::ROTE::UNKNOWN){
                // 方向不明ログ出力
                log << getTime() + "[停止]" + getTeamName(static_cast<TEAM>(player)) + "の行動メソッドが不正な方向を示しています！" << "\r\n";
                // 切断
                startup->team_client[player]->client->is_disconnected = true;
            }

            // 行動ログ出力
            log << getTime() + "[行動]" + getTeamName(static_cast<TEAM>(player)) + "が" + convertString(team_mehod[player]) + "を行いました。" << "\r\n";

            // フィールド情報
            GameBoard*& board = this->ui->Board;
            // チーム周辺情報
            AroundData team_around = board->FieldAccessAround(static_cast<TEAM>(player));
            // チーム周辺情報ログ出力
            log << getTime() + getTeamName(static_cast<TEAM>(player)) + ":" + team_around.toString() << "\r\n";

            // 最終チーム
            if((player ==  TEAM_COUNT-1)||(static_cast<TEAM>(player)==TEAM::COOL)){
                // 進捗バー更新
                ui->TimeBar->setValue(this->ui->TimeBar->value() - 1);
                ui->TimeBar->repaint();
                // 残りターン表示
                this->ui->TurnLabel->setText("残りターン : " + QString::number(ui->TimeBar->value()));

                // ボット戦モード
                if(this->isbotbattle){
                    // COOL側スコア取得
                    int ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::COOL)];
                    // COOL側スコア表示
                    ui->CoolScoreLabel->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
                    // HOT側スコア取得
                    ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::HOT)];
                    // HOT側スコア表示
                    ui->HotScoreLabel ->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
                }
            }
        }else{
            // 周辺情報受信異常ログ出力
            log << getTime() + "[停止]" + getTeamName(static_cast<TEAM>(player)) + "が正常にGetReadyを返しませんでした!" << "\r\n";
            // 切断
            startup->team_client[player]->client->is_disconnected = true;
        }

        // チーム更新
        player++;
        player %= TEAM_COUNT;
        // 勝敗決定
        if(win != WINNER::CONTINUE){
            // 周辺情報要求
            startup->team_client[player]->client->WaitGetReady();
            // フィールド周辺確認
            AroundData buffer = ui->Board->FieldAccessAround(Operation{static_cast<TEAM>(player),
                                                                                   Operation::ACTION::GETREADY,
                                                                                   Operation::ROTE::UNKNOWN},
                                                             ui->Board->team_pos[player]);
            // 接続終了
            buffer.connect = AroundData::CONNECTING_STATUS::FINISHED;
            // 周辺情報応答結果取得
            team_mehod[player] = startup->team_client[player]->client->WaitReturnOperation(buffer);
            // 終了
            Finish(win);
        }
    }

    // ボード盤再描画
    ui->Board->repaint();
    // ターン状態更新
    getready_flag = !getready_flag;
}

/****************************************************************************
*   得点更新
*
*   @param method クライアント行動情報
****************************************************************************/
void MainWindow::ScoreUpdate(Operation method)
{
    // 前回アイテム数
    static int leave_item = 0;

    // アイテム数取得
    if(leave_item == 0)leave_item = this->ui->Board->leave_items;
    // アイテム数が変化
    if(this->ui->Board->leave_items != leave_item){
        // 残りアイテム数表示
        ui->ItemLeaveLabel->setText(QString::number(this->ui->Board->leave_items));
        // アイテム取得をログ出力
        log << getTime() + "[取得]" + getTeamName(method.team) + "がアイテムを取得しました。" << "\r\n";
        // ボット戦モード
        if(this->isbotbattle){
            // COOL側点数表示
            int ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::COOL)];
            ui->CoolScoreLabel->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
            // HOT側点数表示
            ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::HOT)];
            ui->HotScoreLabel ->setText(QString::number(ui->TimeBar->value() + ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }else{
            // 点数表示
            ui->CoolScoreLabel->setText(QString::number(this->ui->Board->team_score[static_cast<int>(TEAM::COOL)]));
            ui->HotScoreLabel ->setText(QString::number(this->ui->Board->team_score[static_cast<int>(TEAM::HOT)]));
        }
        // 前回アイテム数更新
        leave_item = this->ui->Board->leave_items;
    }
}

/****************************************************************************
*   クライアント行動文字列変換
*
*   @return 動作文字列
*
*   @param method クライアント行動情報
****************************************************************************/
QString MainWindow::convertString(Operation method)
{
    QString str;                        // 文字列

    // 行動内容に対応した文字列を取得
    if(method.action == Operation::ACTION::GETREADY)str += "GetReady";
    if(method.action == Operation::ACTION::LOOK)    str += "Look";
    if(method.action == Operation::ACTION::PUT)     str += "Put";
    if(method.action == Operation::ACTION::SEARCH)  str += "Search";
    if(method.action == Operation::ACTION::WALK)    str += "Walk";

    // 方向に対応した文字列を取得
    if(method.rote == Operation::ROTE::UP)   str += "Up";
    if(method.rote == Operation::ROTE::RIGHT)str += "Right";
    if(method.rote == Operation::ROTE::LEFT) str += "Left";
    if(method.rote == Operation::ROTE::DOWN) str += "Down";

    // 戻り値[動作文字列]
    return str;
}

/****************************************************************************
*   勝敗判定
*
*   @return 勝者(要素No.)
****************************************************************************/
MainWindow::WINNER MainWindow::Judge()
{
    bool team_lose[TEAM_COUNT];             // 敗者チーム
    int _player = player;                   // 次ターンのチーム
    GameBoard*& board = this->ui->Board;    // ボード盤

    // 敗者チーム初期化
    for(int i=0;i<TEAM_COUNT;i++)team_lose[i] = false;

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 相手チームを取得
        _player = (_player + 1) % TEAM_COUNT;
        // 現在のチームを取得
        _player = (_player + 1) % TEAM_COUNT;
        // フィールド周辺確認
        AroundData team_around = board->FieldAccessAround(static_cast<TEAM>(_player));

        // ブロック下敷き
        if(team_around.data[4] == GameMap::OBJECT::BLOCK){
            // 敗因ログ出力
            log << getTime() + "[死因]" + getTeamName(static_cast<TEAM>(_player)) + "ブロック下敷き" << "\r\n";
            // ゲーム終了
            team_around.Finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

        // ブロック囲まれ
        if(team_around.data[1] == GameMap::OBJECT::BLOCK &&
           team_around.data[3] == GameMap::OBJECT::BLOCK &&
           team_around.data[5] == GameMap::OBJECT::BLOCK &&
           team_around.data[7] == GameMap::OBJECT::BLOCK){
            // 敗因ログ出力
            log << getTime() + "[死因]" + getTeamName(static_cast<TEAM>(_player)) + "ブロック囲まれ" << "\r\n";
            // ゲーム終了
            team_around.Finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

        // 通信切断
        if(startup->team_client[_player]->client->is_disconnected){
            // 敗因ログ出力
            log << getTime() + "[死因]" + getTeamName(static_cast<TEAM>(_player)) + "通信切断" << "\r\n";
            // ゲーム終了
            team_around.Finish();
            // 敗者チーム決定
            team_lose[_player]=true;
            // ループ脱出
            break;
        }

        // ループ脱出
        break;
    }

    // 相打ち or 時間切れ
    if(!std::find(team_lose,team_lose+TEAM_COUNT,false) || ui->TimeBar->value()==0){
        // アイテム判定ログ出力
        log << getTime() + "[情報]相打ちまたは、タイムアップのためアイテム判定を行います" + "\r\n";
        log << getTime() + "[得点]";
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // チーム名､得点をログ出力
            log << getTeamName(static_cast<TEAM>(i)) + ":" + QString::number(this->ui->Board->team_score[i]) + "  ";
            // 敗者チーム初期化
            team_lose[i] = false;
        }
        // 改行ログ出力
        log << "\r\n";

        QSet<int> team_score_set;       // 得点
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 得点取得
            team_score_set.insert(this->ui->Board->team_score[i]);
        }
        // 同点ならば、戻り値[引き分け]
        if(team_score_set.size()==1)return WINNER::DRAW;

        int index=0;                    // 勝者
        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 最高得点更新ならば勝者
            if(this->ui->Board->team_score[index] < this->ui->Board->team_score[i])index = i;
        }
        // COOL強制勝利
        index = 0;
        // 戻り値[勝者]
        return static_cast<WINNER>(index);
    }

    // 戻り値[HOT]
    if(team_lose[0])return WINNER::HOT;
    // 戻り値[COOL]
    else if(team_lose[1])return WINNER::COOL;
    // 戻り値[継続]
    else return WINNER::CONTINUE;
}

/****************************************************************************
*   勝敗通知
*
*   @param winner 勝者
****************************************************************************/
void MainWindow::Finish(WINNER winner)
{
    // ゲーム停止
    this->clock->stop();
    // 通信切断情報
    QString append_str = "";
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 通信切断
        if(startup->team_client[i]->client->is_disconnected){
            append_str.append("\r\n[" + getTeamName(static_cast<TEAM>(i)) + " 切断により]");
            // 通信切断による終了をログ出力
            log << getTime() + "[終了]" + getTeamName(static_cast<TEAM>(i)) + "との通信が切断されています。" << "\r\n";
        }
    }
    // 勝者をログ出力
    log << this->ui->WinnerLabel->text() << "\r\n";

    // BGM停止
    if(!silent && this->startup->music_text != "None")bgm->stop();

    // 消音モード以外
    if(!silent){
        // 試合終了時のファンファーレを再生
        bgm = new QMediaPlayer(this);
        audio_output = new QAudioOutput(this);
        bgm->setAudioOutput(audio_output);
        bgm->setSource(QUrl("qrc:/Sound/ji_023.mp3"));
        audio_output->setVolume(audio_volume);
        bgm->play();
    }

    // 勝者=COOL
    if(winner == WINNER::COOL){
        // 勝利通知
        this->ui->WinnerLabel->setText("COOL WIN!" + append_str);
        this->ui->WinnerLabel->setText("GAME END" + append_str);
        // 勝利ログ出力
        log << getTime() + "[決着]COOLが勝利しました。" << "\r\n";
        // ボット戦モード
        if(this->isbotbattle){
            // 敗者チームのスコア更新（ターン数分減らす）
            int ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::HOT)];
            ui->HotScoreLabel ->setText(QString::number(ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }
    }
    // 勝者=HOT
    if(winner == WINNER::HOT){
        // 勝利通知
        this->ui->WinnerLabel->setText("HOT WIN!"  + append_str);
        this->ui->WinnerLabel->setText("GAME OVER" + append_str);
        // 勝利ログ出力
        log << getTime() + "[決着]HOTが勝利しました。" << "\r\n";
        // ボット戦モード
        if(this->isbotbattle){
            // 敗者チームのスコア更新（ターン数分減らす）
            int ScoreBuf = this->ui->Board->team_score[static_cast<int>(TEAM::COOL)];
            ui->CoolScoreLabel->setText(QString::number(ScoreBuf*3) + "(ITEM:" + QString::number(ScoreBuf) + ")");
        }
    }
    // 引き分け
    if(winner == WINNER::DRAW){
        // 引き分け通知
        this->ui->WinnerLabel->setText("DRAW");
        this->ui->WinnerLabel->setText("GAME END" + append_str);
        // 引き分けログ出力
        log << getTime() + "[決着]引き分けです。" << "\r\n";
    }
}

/****************************************************************************
*   チーム名取得
*
*   @return 文字列
*
*   @param team チーム種別
****************************************************************************/
QString MainWindow::getTeamName(TEAM team){
    // COOL文字列
    if(team == TEAM::COOL)return "COOL";
    // HOT文字列
    if(team == TEAM::HOT) return "HOT";
    // 不明
    return "UNKNOWN";
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
