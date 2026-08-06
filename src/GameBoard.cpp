/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲームボード管理
****************************************************************************/
#include "GameBoard.h"                  // ゲームボード管理
#include "ui_GameBoard.h"               // ゲームボード管理UI

#include <QPainter>                     // ペイント
#include <QResizeEvent>                 // リサイズイベント

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
GameBoard::GameBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameBoard)
{
    // イメージサイズ
    image_part = QSize(32.0,32.0);
    // テクスチャ読込
    ReloadTexture(":/Image/Light");
    // UI初期化
    ui->setupUi(this);
    // チーム数分
    for(int i = 0; i < TEAM_COUNT; i++){
        // 得点初期化
        this->team_score[i] = 0;
    }
    // 残アイテム数初期化
    leave_items = 0;
}

/****************************************************************************
*   描画更新
*
*   @param event 描画情報(未使用)
****************************************************************************/
void GameBoard::paintEvent([[maybe_unused]] QPaintEvent *event){
    // 描画オブジェクト
    QPainter painter(this);
    // アンチエイリアス･レンダリング設定
    painter.setRenderHints( painter.renderHints() | QPainter::Antialiasing );
    // 背景描画
    painter.fillRect(QRect(0,0,width(),height()),Qt::white);

    // アニメーション無効
    if(!animation){

        // フィールド高さ分
        for(int i = 0;i < field.size.y();i++){
            // フィールド幅分
            for(int j = 0;j < field.size.x();j++){
                // マップ描画済
                if(overlay[i][j] != GameSystem::MAP_OVERLAY::ERASE){
                    // 物体が存在しない
                    if(field.field[i][j] == GameSystem::MAP_OBJECT::NOTHING){
                        // 空白の描画
                        painter.drawPixmap(j * image_part.width(),
                                           i * image_part.height(),
                                           field_resource[static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]);

                    }
                }
            }
        }

        // チーム数分
        for(int i=0;i<TEAM_COUNT;i++){
            // 座標有効
            if(0 <= team_pos[i].x() && team_pos[i].x() < field.size.x() &&
                0 <= team_pos[i].y() && team_pos[i].y() < field.size.y()){
                // チーム描画
                painter.drawPixmap(team_pos[i].x() * image_part.width(),team_pos[i].y() * image_part.height(),team_resource[i]);

            }
        }

        // フィールド高さ分
        for(int i = 0;i < field.size.y();i++){
            // フィールド幅分
            for(int j = 0;j < field.size.x();j++){
                // マップ描画済
                if(overlay[i][j] != GameSystem::MAP_OVERLAY::ERASE){
                    // 物体有り
                    if(field.field[i][j] != GameSystem::MAP_OBJECT::NOTHING){
                        // 物体上書き
                        painter.drawPixmap(j * image_part.width(),
                                           i * image_part.height(),
                                           field_resource[static_cast<int>(field.field[i][j])]);
                    }
                    // (暗転時)未探索
                    if(field.discover[i][j] == GameSystem::Discoverer::Unknown){
                        // 暗闇描画
                        painter.drawPixmap(j * image_part.width() ,
                                           i * image_part.height(),
                                           overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::BLIND)]);
                    }
                    // オーバーレイ(行動効果)有り
                    if(overlay[i][j] != GameSystem::MAP_OVERLAY::NOTHING){
                        // オーバーレイ(行動効果)描画
                        painter.drawPixmap(j * image_part.width() ,
                                           i * image_part.height(),
                                           overray_resource[static_cast<int>(overlay[i][j])]);
                    }
                }
            }
        }
    }
}

/****************************************************************************
*   リサイズ
*
*   @param event リサイズ情報
****************************************************************************/
void GameBoard::resizeEvent(QResizeEvent *event){

    // イメージ幅
    image_part.setWidth (event->size().width() / field.size.x());
    // イメージ高さ
    image_part.setHeight(event->size().height() / field.size.y());
    // イベント実行済
    event->ignore();
    // リサイズ
    resize(image_part.width() * field.size.x(), image_part.height() * field.size.y());
    // テクスチャ再読込
    ReloadTexture(texture_dir_path);
}

/****************************************************************************
*   指定位置物体種別取得
*
*   @return 物体種別
*
*   @param method クライアント行動情報
*   @param pos    位置情報
****************************************************************************/
GameSystem::MAP_OBJECT GameBoard::FieldAccess(GameSystem::Method method, const QPoint& pos){
    // 場外ならば戻り値[ブロック]
    if(pos.x() <  0              || pos.y() <  0)             return GameSystem::MAP_OBJECT::BLOCK;
    if(pos.x() >= field.size.x() || pos.y() >= field.size.y())return GameSystem::MAP_OBJECT::BLOCK;

    // 探索済設定
    field.discover[pos.y()][pos.x()] = GameSystem::Discoverer::Cool;

    // オーバーレイ(行動効果)設定
    if(method.action == GameSystem::Method::ACTION::LOOK    )overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::LOOK;
    if(method.action == GameSystem::Method::ACTION::SEARCH   )overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::SEARCH;
    if(method.action == GameSystem::Method::ACTION::GETREADY)overlay[pos.y()][pos.x()] = GameSystem::MAP_OVERLAY::GETREADY;

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 自チームは対象外
        if(static_cast<GameSystem::TEAM>(i) == method.team)continue;
        // 戻り値[ターゲット位置]
        if(team_pos[i] == pos)return GameSystem::MAP_OBJECT::TARGET;
    }

    // 戻り値[物体種別]
    return this->field.field[pos.y()][pos.x()];
}

/****************************************************************************
*   周辺確認
*
*   @return 周辺情報
*
*   @param team チーム
****************************************************************************/
GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::TEAM team){
    // 戻り値[周辺情報]
    return FieldAccessAround(GameSystem::Method{team,GameSystem::Method::ACTION::UNKNOWN,GameSystem::Method::ROTE::UNKNOWN},
                             team_pos[static_cast<int>(team)]);
}

/****************************************************************************
*   指定位置周辺確認
*
*   @return 周辺情報
*
*   @param method クライアント行動情報
*   @param center 中央位置
****************************************************************************/
GameSystem::AroundData GameBoard::FieldAccessAround(GameSystem::Method method, const QPoint& center){
    // 周辺情報
    GameSystem::AroundData around;
    // 接続状態:継続
    around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
    // 周辺情報取得
    for(int i=0;i<9;i++){
        // 物体種別取得
        around.data[i] = FieldAccess(method,QPoint(center.x() + (i % 3) - 1,center.y() + (i / 3) - 1));
    }
    // 戻り値[周辺情報]
    return around;
}

/****************************************************************************
*   行動後周辺確認
*
*   @return 周辺情報
*
*   @param method クライアント行動情報
****************************************************************************/
GameSystem::AroundData GameBoard::FieldAccessMethod(GameSystem::Method method){
    // 行動により分岐
    switch(method.action){
        case GameSystem::Method::ACTION::PUT:       // 指定方向ブロック配置
            // 指定座標が有効
            if((team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() < field.size.y() &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() < field.size.x()){
                // 指定座標
                QPoint get_pos = team_pos[static_cast<int>(method.team)] + method.GetRoteVector();
                // PUT先にアイテムが存在
                if(this->field.field[get_pos.y()][get_pos.x()] == GameSystem::MAP_OBJECT::ITEM){
                    // アイテム数更新
                    this->leave_items --;
                }
                // ブロック配置
                this->field.field[get_pos.y()][get_pos.x()] = GameSystem::MAP_OBJECT::BLOCK;
            }
            // 戻り値[周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::LOOK:      // 指定方向周囲確認
            // 戻り値[指定座標を中心とした周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * 2);
        case GameSystem::Method::ACTION::WALK:      // 指定方向移動
            // 指定座標
            team_pos[static_cast<int>(method.team)] += method.GetRoteVector();
            // アイテム取得
            this->PickItem(method);
            // 戻り値[周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case GameSystem::Method::ACTION::SEARCH:    // 指定方向直線確認
            // 周辺情報
            GameSystem::AroundData around;
            // 接続状態=継続
            around.connect = GameSystem::CONNECTING_STATUS::CONTINUE;
            // 確認座標数分
            for(int i=1;i<10;i++){
                // 物体情報取得
                around.data[i-1] = FieldAccess(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * i);
            }
            // 戻り値[指定方向直線上の物体情報]
            return around;
        default:                                    // 不明
            // 戻り値[空の周辺情報]
            return GameSystem::AroundData();
    }
}

/****************************************************************************
*   アイテム取得
*
*   @param method クライアント行動情報
****************************************************************************/
void GameBoard::PickItem(GameSystem::Method method){

    // 移動元
    QPoint pos = this->team_pos[static_cast<int>(method.team)];
    // 移動先がアイテム
    if(this->FieldAccess(method,pos) == GameSystem::MAP_OBJECT::ITEM){
        // 移動元のフィールドを初期化
        this->field.field[ pos                        .y()][ pos                        .x()] = GameSystem::MAP_OBJECT::NOTHING;
        // 移動先のフィールドを壁に設定
        this->field.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameSystem::MAP_OBJECT::BLOCK;

        // 点数を１増やす
        this->team_score[static_cast<int>(method.team)]++;
        // 残りアイテム数更新
        this->leave_items--;
    }
}

/****************************************************************************
*   接続終了
*
*   @return 周辺情報
*
*   @param team チーム
****************************************************************************/
GameSystem::AroundData GameBoard::FinishConnecting(GameSystem::TEAM team){
    // 周辺情報
    GameSystem::AroundData around = FieldAccessAround(GameSystem::Method{team,GameSystem::Method::ACTION::UNKNOWN,GameSystem::Method::ROTE::UNKNOWN},
                                                      team_pos[static_cast<int>(team)]);
    // 終了
    around.finish();
    // 戻り値[周辺情報]
    return around;
}

/****************************************************************************
*   マップ設定
*
*   @param map マップ情報
****************************************************************************/
void GameBoard::setMap(const GameSystem::Map& map){
    // フィールド状態取得
    field = map;
    // 使用テクスチャディレクトリ取得
    this->texture_dir_path = map.texture_dir_path;
    // テクスチャ読込
    ReloadTexture(texture_dir_path);
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // チーム位置取得
        team_pos[i] = field.team_first_point[i];
    }
    // マップ高さ取得
    map_height = field.field.size();
    // マップ幅取得
    map_width  = field.field[0].size();
    // オーバーレイ(行動効果)サイズ設定
    overlay.resize(map_height);
    // オーバーレイ(行動効果)初期化
    for(auto& v : overlay)v = QVector<GameSystem::MAP_OVERLAY>(map_width,GameSystem::MAP_OVERLAY::NOTHING);
    // 探索状態サイズ設定
    field.discover.resize(map_height);
    // 探索状態初期化
    for(auto& v : field.discover)v = QVector<GameSystem::Discoverer>(map_width,GameSystem::Discoverer::Cool);
}

/****************************************************************************
*   オーバーレイ(行動効果)消去
****************************************************************************/
void GameBoard::RefreshOverlay(){
    // フィールド高さ分
    for(int i = 0;i < field.size.y();i++){
        // フィールド幅分
        for(int j = 0;j < field.size.x();j++){
            // オーバーレイ(行動効果)消去
            overlay[i][j] = GameSystem::MAP_OVERLAY::NOTHING;
        }
    }
}

/****************************************************************************
*   オブジェクト数算出
*
*   @return オブジェクト数
*
*   @param mb 物体種別
****************************************************************************/
int GameBoard::GetMapObjectCount(GameSystem::MAP_OBJECT mb){
    // オブジェクト数
    int result = 0;
    // フィールド高さ分
    for(int i = 0; i < field.size.y(); i++){
        // フィールド幅分
        for(int j = 0; j < field.size.x(); j++){
            // 物体検出
            if(field.field[i][j] == mb){
                // オブジェクト数更新
                result ++;
            }
        }
    }

    // 戻り値[オブジェクト数]
    return result;
}

/****************************************************************************
*   テクスチャ再読込
*
*   @param tex_dir_path 使用テクスチャディレクトリ
****************************************************************************/
void GameBoard::ReloadTexture(QString tex_dir_path){
    // 使用テクスチャディレクトリ取得
    this->texture_dir_path = tex_dir_path;

    // チーム画像読込
    this->team_resource   [static_cast<int>(GameSystem::TEAM::COOL)]            = QPixmap(tex_dir_path + "/Cool.png");
    this->team_resource   [static_cast<int>(GameSystem::TEAM::HOT)]             = QPixmap(tex_dir_path + "/Hot.png");
    // フィールド画像読込
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::NOTHING)]   = QPixmap(tex_dir_path + "/Floor.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::TARGET)]    = QPixmap();
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::ITEM)]      = QPixmap(tex_dir_path + "/Item.png");
    this->field_resource  [static_cast<int>(GameSystem::MAP_OBJECT::BLOCK)]     = QPixmap(tex_dir_path + "/Block.png");
    // オーバーレイ画像読込
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::NOTHING)]  = QPixmap();
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::LOOK)]     = QPixmap(tex_dir_path + "/Look.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::GETREADY)] = QPixmap(tex_dir_path + "/Getready.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::SEARCH)]    = QPixmap(tex_dir_path + "/Search.png");
    this->overray_resource[static_cast<int>(GameSystem::MAP_OVERLAY::BLIND)]    = QPixmap(tex_dir_path + "/Blind.png");

    // チーム画像サイズ変更
    for(QPixmap& img:team_resource )img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    // フィールド画像サイズ変更
    for(QPixmap& img:field_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    // オーバーレイ画像サイズ変更
    for(QPixmap& img:overray_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
GameBoard::~GameBoard()
{
    // UI破棄
    delete ui;
}
