/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   起動画面
****************************************************************************/
#include "startupdialog.h"              // 起動画面
#include "ui_startupdialog.h"           // 起動画面UI

#include <QFileDialog>                  // ファイルダイアログ
#include <QHostInfo>                    // ホスト情報
#include <QMessageBox>                  // メッセージボックス
#include <QNetworkInterface>            // ネットワーク一覧

#include "DesignDialog.h"               // デザイン設定
#include "MapEditerDialog.h"            // マップ編集画面
#include "SettingDialog.h"              // 設定画面

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
StartupDialog::StartupDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StartupDialog)
    , map_standby(false)
{
    // UI初期化
    ui->setupUi(this);
    // BGMファイルリスト生成
    setMusicFileList();
    // テクスチャリスト生成
    setImageThemeList();
    // BGM名取得
    music_text = ui->GameMusicCombo->currentText();
    // COOL側ポート番号初期値設定
    ui->CoolGroupBox->SetPortSpin(2009);
    // HOT側ポート番号初期値設定
    ui->HotGroupBox->SetPortSpin(2010);

    // クライアント初期化
    this->team_client[static_cast<int>(GameSystem::TEAM::COOL)] = ui->CoolGroupBox;
    this->team_client[static_cast<int>(GameSystem::TEAM::HOT)] = ui->HotGroupBox;
    // チーム数分
    for (int i = 0; i < TEAM_COUNT; i++) {
        // チーム準備状態初期化
        team_standby[i] = false;
    }

    // HOT側ポート番号シグナル･スロット設定
    connect(ui->HotGroupBox,  &ClientSettingForm::Standby, this, &StartupDialog::ClientStandby);
    // COOL側ポート番号シグナル･スロット設定
    connect(ui->CoolGroupBox, &ClientSettingForm::Standby, this, &StartupDialog::ClientStandby);

    // ローカルIPの探索
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        // IPv4プロトコル and ローカルホスト以外
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)){
            // ローカルIP表示
            this->ui->LocalIPLabel->setText(address.toString());
            // ループ脱出
            break;
        }
    }
    // ホスト名表示
    this->ui->HostName->setText(QHostInfo::localHostName());

    // ランダムマップ生成
    map.CreateRandomMap();
    // マップ準備完了
    map_standby = true;

    // コマンドライン引数取得
    QStringList argv = QCoreApplication::arguments();
    // 引数有効
    if(argv.size() > 1){
        // マップファイル存在
        if(QFile::exists(argv[1])){
            // マップ読込 and 準備設定
            SetMapStandby(MapRead(argv[1]));
            // マップフルパス表示
            this->ui->MapDirEdit->setText(argv[1]);
        } else {
            // 警告通知
            QMessageBox::information(this, tr("警告"), tr("コマンドライン引数で指定されたマップファイルが存在しません。\nランダムマップが使用されます。"));
        }
    }
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
StartupDialog::~StartupDialog()
{
    // UI破棄
    delete ui;
}

/****************************************************************************
*   BGM選択
*
*   @param text 選択項目文字列
****************************************************************************/
void StartupDialog::ChangeMusicCombo(QString text)
{
    // BGM名取得
    music_text = text;
}

/****************************************************************************
*   ゲーム開始可否確認
****************************************************************************/
void StartupDialog::CheckStandby()
{
    // 全体準備状態
    bool all_of = true;
    // チーム数分
    for (int i = 0; i < TEAM_COUNT; i++) {
        // 準備未完了
        if (!team_standby[i])
            // 準備未完了
            all_of = false;
    }
    // [ゲーム開始]ボタン有効/無効設定
    this->ui->ServerStartButton->setEnabled(all_of && map_standby);
}

/****************************************************************************
*   マップ編集画面表示
****************************************************************************/
void StartupDialog::ShowMapEditDialog()
{
    // マップ編集画面
    MapEditerDialog diag(map);
    // マップ編集実行
    if (diag.exec()) {
        // ファイルパス無効
        if (diag.filepath == "") {
            // マップフルパス表示
            this->ui->MapDirEdit->setText("[CUSTOM MAP]");
            // マップ情報取得
            map = diag.GetMap();
        } else {
            // 再読込
            if (MapRead(diag.filepath))
                // マップフルパス表示
                this->ui->MapDirEdit->setText(diag.filepath);
        }
    }
    // マップ準備完了
    SetMapStandby(true);
}

/****************************************************************************
*   マップ読込
*
*   @return true固定
*
*   @param dir マップフルパス
****************************************************************************/
bool StartupDialog::MapRead(const QString &dir)
{
    // マップファイル読込
    this->map.Import(dir);
    // マップサイズ設定
    map.size.setY(map.field.size());
    // 戻り値[true]
    return true;
}

/****************************************************************************
*   BGMリスト生成
****************************************************************************/
void StartupDialog::setMusicFileList()
{
    // BGMディレクトリ
    QDir dir("./Music");

    // BGM選択コンボボックス初期化
    ui->GameMusicCombo->clear();
    // ディレクトリ存在
    if (dir.exists()) {
        // 拡張子が".mp3"と".wav"のファイルリストを取得
        QStringList filelist = dir.entryList({"*.mp3", "*.wav"}, QDir::Files | QDir::NoSymLinks);
        // ファイル未存在
        if (filelist.isEmpty()) {
            // None項目追加
            ui->GameMusicCombo->addItem("None");
            // 選択不可
            ui->GameMusicCombo->setEnabled(false);
        } else
            // ファイルリスト登録
            ui->GameMusicCombo->addItems(filelist);
    } else {
        // None項目追加
        ui->GameMusicCombo->addItem("None");
        // 選択不可
        ui->GameMusicCombo->setEnabled(false);
    }
}

/****************************************************************************
*   テクスチャリスト生成
****************************************************************************/
void StartupDialog::setImageThemeList()
{
    // テクスチャ選択コンボボックス初期化
    ui->TextureThemeCombo->clear();
    // 選択項目追加
    ui->TextureThemeCombo->addItems({"ほうせき", "あっさり", "こってり"});
    // テクスチャディレクトリ
    QDir dir("./Image");

    // ディレクトリ存在
    if (dir.exists()) {
        // ファイルリスト取得
        QStringList filelist = dir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        //qDebug()<<filelist;
        // ファイルリスト登録
        ui->TextureThemeCombo->addItems(filelist);
    }
}

/****************************************************************************
*   マップ選択
****************************************************************************/
void StartupDialog::PushedMapSelect()
{
    // マップフォルダ
    QString folder = QDir::currentPath() + "/Map/";
    // キャプション
    QString cap = tr("マップを開く");
    // フィルタ
    QString filter = tr("マップファイル (*.map)");

    // ファイルパス取得
    QString filePath = QFileDialog::getOpenFileName(this, cap, folder, filter);
    // ファイルパス有効
    if(filePath != ""){
        // マップフルパス表示
        this->ui->MapDirEdit->setText(filePath);
        // マップ読込 and 準備設定
        SetMapStandby(MapRead(filePath));
    }
}

/****************************************************************************
*   クライアント準備設定
*
*   @param client   クライアント設定画面
*   @param complate 準備状態
****************************************************************************/
void StartupDialog::ClientStandby(ClientSettingForm *client, bool complate)
{
    // チーム数分
    for (int i = 0; i < TEAM_COUNT; i++) {
        // 該当チーム
        if (team_client[i] == client) {
            // 準備状態取得
            team_standby[i] = complate;
            // ゲーム開始可否確認
            CheckStandby();
            // 関数終了
            return;
        }
    }
}

/****************************************************************************
*   マップ準備設定
*
*   @param state マップ準備状態
****************************************************************************/
void StartupDialog::SetMapStandby(bool state)
{
    // マップ準備状態更新
    map_standby = state;
    // ゲーム開始可否確認
    CheckStandby();
}

/****************************************************************************
*   テクスチャ変更
*
*   @param text テクスチャ文字列
****************************************************************************/
void StartupDialog::ChangedTexture(QString text)
{
    // テクスチャ文字列に対応するパスを取得
    if (text == "あっさり")      this->map.texture_dir_path = ":/Image/Light";
    else if (text == "こってり") this->map.texture_dir_path = ":/Image/Heavy";
    else if (text == "ほうせき") this->map.texture_dir_path = ":/Image/Jewel";
    else                       this->map.texture_dir_path = "Image/" + text;
}

/****************************************************************************
*   [サーバー設定]ボタン押下
****************************************************************************/
void StartupDialog::Setting()
{
    // サーバー設定画面
    SettingDialog *diag;
    // 画面生成
    diag = new SettingDialog;
    // 画面実行 and 設定有効
    if (diag->exec() == QDialog::Accepted) {
        // 設定保存
        diag->Export();
    }
    // 画面破棄
    delete diag;
}

/****************************************************************************
*   [デザイン設定]ボタン押下
****************************************************************************/
void StartupDialog::ShowDesignDialog()
{
    // デザイン設定画面
    DesignDialog *diag;
    // 画面生成
    diag = new DesignDialog;
    // 画面実行 and 設定有効
    if (diag->exec() == QDialog::Accepted) {
        // 設定保存
        diag->Export();
    }
    // 画面破棄
    delete diag;
}
