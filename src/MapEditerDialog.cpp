/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ編集画面
****************************************************************************/
#include "MapEditerDialog.h"            // マップ編集画面
#include "ui_MapEditerDialog.h"         // マップ編集画面UI

#include <QFileDialog>                  // ファイルダイアログ
#include <QMessageBox>                  // メッセージボックス
#include <QMouseEvent>                  // マウスイベント

/****************************************************************************
*   コンストラクタ
*
*   @param map    マップ情報
*   @param parent 親ウィジェット
****************************************************************************/
MapEditerDialog::MapEditerDialog(GameSystem::Map map, QWidget *parent) :
    QDialog(parent),
    clicking(false),
    ui(new Ui::MapEditerDialog),
    filepath("")
{
    // UI初期化
    ui->setupUi(this);

    // マウス追跡有効化
    setMouseTracking(true);

    // ブロック、アイテムの数をカウントするリストを追加
    ui->ObjectCounter->addItem(new QListWidgetItem("×" + QString(QString::number(0))));
    ui->ObjectCounter->addItem(new QListWidgetItem("×" + QString(QString::number(0))));
    // カウント部分のアイコンを設定
    ui->ObjectCounter->setIconSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8));
    ui->ObjectCounter->item(0)->setIcon(QIcon(map.texture_dir_path + "/Block.png"));
    ui->ObjectCounter->item(1)->setIcon(QIcon(map.texture_dir_path + "/Item.png"));

    // エリア初期化
    ComboChanged();
    // マップ設定
    ui->widget->setMap(map);
    // オブジェクトリスト生成
    ui->listWidget->addItem(new QListWidgetItem("Nothing"));
    ui->listWidget->addItem(new QListWidgetItem("Target"));
    ui->listWidget->addItem(new QListWidgetItem("Block"));
    ui->listWidget->addItem(new QListWidgetItem("Item"));
    // アイコンサイズ設定
    ui->listWidget->setIconSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8));
    // アイコンリスト生成
    ui->listWidget->item(0)->setIcon(QIcon(map.texture_dir_path + "/Floor.png"));
    ui->listWidget->item(1)->setIcon(QIcon(map.texture_dir_path + "/Cool.png"));
    ui->listWidget->item(2)->setIcon(QIcon(map.texture_dir_path + "/Block.png"));
    ui->listWidget->item(3)->setIcon(QIcon(map.texture_dir_path + "/Item.png"));

    // オブジェクト数更新
    ReCount();

    // オブジェクトリスト変更時シグナル･スロット設定
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &MapEditerDialog::SelectItem);
    // ターン変更時シグナル･スロット設定
    connect(ui->TurnSpin, &QSpinBox::valueChanged, this, &MapEditerDialog::SpinChanged);

    // オブジェクトリスト先頭選択
    ui->listWidget->setCurrentRow(0);
    // ターン数設定
    ui->TurnSpin->setValue(ui->widget->field.turn);

    // [ランダム生成]ボタンイベント設定
    connect(ui->randomGenerateButton, &QPushButton::pressed, this, &MapEditerDialog::randomGenerateButtonPressed);
}

/****************************************************************************
*   オブジェクト数更新
****************************************************************************/
void MapEditerDialog::ReCount()
{
    // ブロック数表示
    int counter = 0;
    counter = ui->widget->GetMapObjectCount(GameSystem::MAP_OBJECT::BLOCK);
    ui->ObjectCounter->item(0)->setText("×" + QString(QString::number(counter)));
    // アイテム数表示
    counter = ui->widget->GetMapObjectCount(GameSystem::MAP_OBJECT::ITEM);
    ui->ObjectCounter->item(1)->setText("×" + QString(QString::number(counter)));
}

/****************************************************************************
*   マウスクリック
****************************************************************************/
void MapEditerDialog::mousePressEvent(QMouseEvent *event)
{
    // 左クリック
    if(event->button() == Qt::LeftButton){
        // クリック有効
        this->clicking = true;
        // オブジェクト配置
        FillItem(event->pos());
    }
}

/****************************************************************************
*   マウス移動
****************************************************************************/
void MapEditerDialog::mouseMoveEvent(QMouseEvent* event)
{
    // クリック中
    if(clicking)
        FillItem(event->pos());         // オブジェクト配置

}

/****************************************************************************
*   オブジェクト配置
*
*   @param pos 位置情報
****************************************************************************/
void MapEditerDialog::FillItem(const QPoint& pos)
{
    // 左マージン、上マージン
    int left_m,top_m;
    // マージン取得
    this->layout()->getContentsMargins(&left_m, &top_m, nullptr, nullptr);
    // 配置位置
    QPoint fill_point((pos.x() - left_m)/ui->widget->image_part.width(), (pos.y() - top_m)/ui->widget->image_part.height());

    // 有効範囲外ならば関数終了
    if(fill_point.x() < 0 || fill_point.x() >= ui->widget->field.size.x() ||
        fill_point.y() < 0 || fill_point.y() >= ui->widget->field.size.y())return;

    // 物体
    GameSystem::MAP_OBJECT obj = GameSystem::MAP_OBJECT::NOTHING;
    // 文字列→物体定数に変換
    if     (ui->listWidget->selectedItems().first()->text() == "Nothing")obj = GameSystem::MAP_OBJECT::NOTHING;
    else if(ui->listWidget->selectedItems().first()->text() == "Block"  )obj = GameSystem::MAP_OBJECT::BLOCK;
    else if(ui->listWidget->selectedItems().first()->text() == "Item"   )obj = GameSystem::MAP_OBJECT::ITEM;
    // ターゲット(相手)
    if(ui->listWidget->selectedItems().first()->text() == "Target" ){
        // 初期位置変更
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = fill_point;
        this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = ui->widget->field.MirrorPoint(fill_point);
    }else{
        // オブジェクト配置
        this->ui->widget->field.field[fill_point.y()][fill_point.x()] = obj;
        // 対称設定有効
        if(ui->SymmetryCheck->isChecked()){
            // 対称位置
            QPoint r_fill_point(ui->widget->field.MirrorPoint(fill_point));
            // オブジェクト配置
            this->ui->widget->field.field[r_fill_point.y()][r_fill_point.x()] = obj;
        }
    }
    // オブジェクト数更新
    ReCount();
    // 再描画
    update();
}

/****************************************************************************
*   マウスクリック解放
****************************************************************************/
void MapEditerDialog::mouseReleaseEvent(QMouseEvent *event)
{
    // クリック無効
    if(event->button() == Qt::LeftButton)this->clicking = false;
}

/****************************************************************************
*   エリア選択
****************************************************************************/
void MapEditerDialog::ComboChanged()
{
    // [ランダム生成]ボタン押下
    randomGenerateButtonPressed();

    // ウインドウ縦横比算出
    double window_width_per_height = static_cast<double>(ui->widget->field.size.x())/ui->widget->field.size.y();
    // 画面の最大高さをもとに、ウインドウ高さを算出
    int window_height = static_cast<int>(QGuiApplication::primaryScreen()->size().height()*0.8);
    // リサイズ
    resize(QSize(window_height*window_width_per_height+134, window_height+4));

    // 描画更新
    paintEvent(nullptr);
    // 再描画
    update();
}

/****************************************************************************
*   ターン数変更
*
*   @param value 数値
****************************************************************************/
void MapEditerDialog::SpinChanged(int value){
    // ターン数変更
    ui->widget->field.turn = value;
}

/****************************************************************************
*   [ランダム生成]ボタン押下
****************************************************************************/
void MapEditerDialog::randomGenerateButtonPressed()
{
    // エリア文字列取得
    auto fieldSizeText = ui->comboBox->currentText();

    // 広域
    if(fieldSizeText=="広域(21x17)"){
        // フィールドサイズ設定
        this->ui->widget->field.SetSize(QPoint(21,17), ui->BlockSpin->value(), ui->ItemSpin->value());
    }
    // 決戦
    else if(fieldSizeText=="決戦(15x17)"){
        // フィールドサイズ設定
        this->ui->widget->field.SetSize(QPoint(15,17), ui->BlockSpin->value(), ui->ItemSpin->value());
    }

    // COOL側初期位置
    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::COOL)] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::COOL)];
    // HOT側初期位置
    this->ui->widget->team_pos[static_cast<int>(GameSystem::TEAM::HOT )] = this->ui->widget->field.team_first_point[static_cast<int>(GameSystem::TEAM::HOT )];

    // マップ設定
    ui->widget->setMap(ui->widget->field);
    // 描画更新
    paintEvent(nullptr);
    // オブジェクト数更新
    ReCount();
    // 再描画
    update();

    // ブロック数が奇数
    if(this->ui->BlockSpin->value() % 2 != 0)
        // 警告通知
        QMessageBox::information(this, tr("警告"), tr("ブロックは必ず偶数個で生成されます"));

    // アイテム数が偶数
    if(this->ui->ItemSpin->value() % 2 == 0)
        // 警告通知
        QMessageBox::information(this, tr("警告"), tr("アイテムは必ず奇数個で生成されます"));
}

/****************************************************************************
*   [全消し]ボタン押下
****************************************************************************/
void MapEditerDialog::Clear()
{
    // フィールド内
    for(auto& v1 : ui->widget->field.field){
        for(auto& v2 : v1){
            // オブジェクト破棄
            v2 = GameSystem::MAP_OBJECT::NOTHING;
        }
    }
    // オブジェクト数更新
    ReCount();
    // 再描画
    update();
}

/****************************************************************************
*   [書き出し]ボタン押下
****************************************************************************/
void MapEditerDialog::Export()
{
    // マップ保存フルパス取得
    filepath = QFileDialog::getSaveFileName(
        this,
        tr("マップを保存"),
        QDir::currentPath()+"/Map/",
        tr("マップデータ (*.map)"));
    // マップ保存
    if(filepath != "")ui->widget->field.Export(filepath);
}

/****************************************************************************
*   オブジェクト選択
*
*   @param next 次のオブジェクト
****************************************************************************/
void MapEditerDialog::SelectItem(QListWidgetItem *next)
{
    // 次のオブジェクトが有効
    if(next){
        // アイコン
        QIcon icon = next->icon();
        // カーソル
        QCursor myCursor = QCursor(icon.pixmap(icon.actualSize(QSize(ICON_SIZE*0.8, ICON_SIZE*0.8))));
        // カーソル設定
        ui->widget->setCursor(myCursor);
    }
}

/****************************************************************************
*   マップ取得
*
*   @return マップ
****************************************************************************/
GameSystem::Map MapEditerDialog::GetMap(){
    // 戻り値[マップ]
    return ui->widget->field;
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
MapEditerDialog::~MapEditerDialog()
{
    // UI破棄
    delete ui;
}
