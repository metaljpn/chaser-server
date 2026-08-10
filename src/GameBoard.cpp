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
    LoadTexture(":/Image/Light");
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

    // フィールド高さ分
    for(int i=0;i<map.size.y();i++){
        // フィールド幅分
        for(int j=0;j<map.size.x();j++){
            // 描画対象
            if(effect[i][j] != GameMap::EFFECT::ERASE){
                // 物体が存在しない
                if(map.field[i][j] == GameMap::OBJECT::NOTHING){
                    // 空白の描画
                    painter.drawPixmap(j * image_part.width(),
                                       i * image_part.height(),
                                       field_resource[static_cast<int>(GameMap::OBJECT::NOTHING)]);
                }
            }
        }
    }

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 座標有効
        if(0 <= team_pos[i].x() && team_pos[i].x() < map.size.x() &&
           0 <= team_pos[i].y() && team_pos[i].y() < map.size.y()){
            // チーム描画
            painter.drawPixmap(team_pos[i].x() * image_part.width(),team_pos[i].y() * image_part.height(),team_resource[i]);
        }
    }

    // フィールド高さ分
    for(int i=0;i<map.size.y();i++){
        // フィールド幅分
        for(int j=0;j<map.size.x();j++){
            // 描画対象
            if(effect[i][j] != GameMap::EFFECT::ERASE){
                // 物体有り
                if(map.field[i][j] != GameMap::OBJECT::NOTHING){
                    // 物体描画
                    painter.drawPixmap(j * image_part.width(),
                                       i * image_part.height(),
                                       field_resource[static_cast<int>(map.field[i][j])]);
                }
                // 暗闇モード時未探索
                if(map.discover[i][j] == GameMap::DISCOVER::UNKNOWN){
                    // 暗闇描画
                    painter.drawPixmap(j * image_part.width() ,
                                       i * image_part.height(),
                                       overray_resource[static_cast<int>(GameMap::EFFECT::BLIND)]);
                }
                // 行動効果有り
                if(effect[i][j] != GameMap::EFFECT::NOTHING){
                    // 行動効果描画
                    painter.drawPixmap(j * image_part.width() ,
                                       i * image_part.height(),
                                       overray_resource[static_cast<int>(effect[i][j])]);
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
    image_part.setWidth (event->size().width() / map.size.x());
    // イメージ高さ
    image_part.setHeight(event->size().height() / map.size.y());
    // イベント実行済
    event->ignore();
    // リサイズ
    resize(image_part.width() * map.size.x(), image_part.height() * map.size.y());
    // テクスチャ読込
    LoadTexture(texture_dir_path);
}

/****************************************************************************
*   指定位置物体種別取得
*
*   @return 物体種別
*
*   @param method クライアント行動情報
*   @param pos    位置情報
****************************************************************************/
GameMap::OBJECT GameBoard::FieldAccess(Operation method, const QPoint& pos){
    // 場外ならば戻り値[ブロック]
    if(pos.x() <  0            || pos.y() <  0)           return GameMap::OBJECT::BLOCK;
    if(pos.x() >= map.size.x() || pos.y() >= map.size.y())return GameMap::OBJECT::BLOCK;

    // 探索済設定
    map.discover[pos.y()][pos.x()] = GameMap::DISCOVER::EXPLORED;

    // 行動効果設定
    if(method.action == Operation::ACTION::LOOK    )effect[pos.y()][pos.x()] = GameMap::EFFECT::LOOK;
    if(method.action == Operation::ACTION::SEARCH  )effect[pos.y()][pos.x()] = GameMap::EFFECT::SEARCH;
    if(method.action == Operation::ACTION::GETREADY)effect[pos.y()][pos.x()] = GameMap::EFFECT::GETREADY;

    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // 自チームは対象外
        if(static_cast<GameSystem::TEAM>(i) == method.team)continue;
        // 戻り値[ターゲット位置]
        if(team_pos[i] == pos)return GameMap::OBJECT::TARGET;
    }

    // 戻り値[物体種別]
    return this->map.field[pos.y()][pos.x()];
}

/****************************************************************************
*   周辺確認
*
*   @return 周辺情報
*
*   @param team チーム
****************************************************************************/
AroundData GameBoard::FieldAccessAround(GameSystem::TEAM team){
    // 戻り値[周辺情報]
    return FieldAccessAround(Operation{team,Operation::ACTION::UNKNOWN,Operation::ROTE::UNKNOWN},
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
AroundData GameBoard::FieldAccessAround(Operation method, const QPoint& center){
    // 周辺情報
    AroundData around;
    // 接続状態:継続
    around.connect = AroundData::CONNECTING_STATUS::CONTINUE;
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
AroundData GameBoard::FieldAccessOperation(Operation method){
    // 行動により分岐
    switch(method.action){
        case Operation::ACTION::PUT:       // 指定方向ブロック配置
            // 指定座標が有効
            if((team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() >= 0 &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).y() < map.size.y() &&
               (team_pos[static_cast<int>(method.team)] + method.GetRoteVector()).x() < map.size.x()){
                // 指定座標
                QPoint get_pos = team_pos[static_cast<int>(method.team)] + method.GetRoteVector();
                // PUT先にアイテムが存在
                if(this->map.field[get_pos.y()][get_pos.x()] == GameMap::OBJECT::ITEM){
                    // アイテム数更新
                    this->leave_items --;
                }
                // ブロック配置
                this->map.field[get_pos.y()][get_pos.x()] = GameMap::OBJECT::BLOCK;
            }
            // 戻り値[周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case Operation::ACTION::LOOK:      // 指定方向周囲確認
            // 戻り値[指定座標を中心とした周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * 2);
        case Operation::ACTION::WALK:      // 指定方向移動
            // 指定座標
            team_pos[static_cast<int>(method.team)] += method.GetRoteVector();
            // アイテム取得
            this->PickItem(method);
            // 戻り値[周辺情報]
            return FieldAccessAround(method,team_pos[static_cast<int>(method.team)]);
        case Operation::ACTION::SEARCH:    // 指定方向直線確認
            // 周辺情報
            AroundData around;
            // 接続状態=継続
            around.connect = AroundData::CONNECTING_STATUS::CONTINUE;
            // 確認座標数分
            for(int i=1;i<10;i++){
                // 物体情報取得
                around.data[i-1] = FieldAccess(method,team_pos[static_cast<int>(method.team)] + method.GetRoteVector() * i);
            }
            // 戻り値[指定方向直線上の物体情報]
            return around;
        default:                                    // 不明
            // 戻り値[空の周辺情報]
            return AroundData();
    }
}

/****************************************************************************
*   アイテム取得
*
*   @param method クライアント行動情報
****************************************************************************/
void GameBoard::PickItem(Operation method){
    // 移動元
    QPoint pos = this->team_pos[static_cast<int>(method.team)];
    // 移動先がアイテム
    if(this->FieldAccess(method,pos) == GameMap::OBJECT::ITEM){
        // 移動元のフィールドを初期化
        this->map.field[pos.y()][pos.x()] = GameMap::OBJECT::NOTHING;
        // 移動先のフィールドを壁に設定
        this->map.field[(pos-method.GetRoteVector()).y()][(pos-method.GetRoteVector()).x()] = GameMap::OBJECT::BLOCK;
        // 点数更新
        this->team_score[static_cast<int>(method.team)]++;
        // 残りアイテム数更新
        this->leave_items--;
    }
}

/****************************************************************************
*   マップ設定
*
*   @param mapinfo マップ情報
****************************************************************************/
void GameBoard::setMap(const GameMap& mapinfo){
    // マップ情報取得
    map = mapinfo;
    // 使用テクスチャディレクトリ取得
    this->texture_dir_path = map.texture_dir_path;
    // テクスチャ読込
    LoadTexture(texture_dir_path);
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // チーム位置取得
        team_pos[i] = map.team_first_point[i];
    }
    // マップ高さ取得
    map_height = map.field.size();
    // マップ幅取得
    map_width  = map.field[0].size();
    // 行動効果サイズ設定
    effect.resize(map_height);
    // 行動効果初期化
    for(auto& v : effect)v = QVector<GameMap::EFFECT>(map_width,GameMap::EFFECT::NOTHING);
    // 探索状態サイズ設定
    map.discover.resize(map_height);
    // 探索状態初期化
    for(auto& v : map.discover)v = QVector<GameMap::DISCOVER>(map_width,GameMap::DISCOVER::EXPLORED);
}

/****************************************************************************
*   行動効果初期化
****************************************************************************/
void GameBoard::ResetEffect(){
    // フィールド高さ分
    for(int i=0;i<map.size.y();i++){
        // フィールド幅分
        for(int j=0;j<map.size.x();j++){
            // 行動効果消去
            effect[i][j] = GameMap::EFFECT::NOTHING;
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
int GameBoard::GetMapObjectCount(GameMap::OBJECT mb){
    // オブジェクト数
    int result = 0;
    // フィールド高さ分
    for(int i=0; i<map.size.y();i++){
        // フィールド幅分
        for(int j=0;j<map.size.x();j++){
            // 物体検出
            if(map.field[i][j] == mb){
                // オブジェクト数更新
                result++;
            }
        }
    }

    // 戻り値[オブジェクト数]
    return result;
}

/****************************************************************************
*   テクスチャ読込
*
*   @param tex_dir_path 使用テクスチャディレクトリ
****************************************************************************/
void GameBoard::LoadTexture(QString tex_dir_path){
    // 使用テクスチャディレクトリ取得
    this->texture_dir_path = tex_dir_path;

    // チーム画像読込
    this->team_resource   [static_cast<int>(GameSystem::TEAM::COOL)]     = QPixmap(tex_dir_path + "/Cool.png");
    this->team_resource   [static_cast<int>(GameSystem::TEAM::HOT)]      = QPixmap(tex_dir_path + "/Hot.png");
    // フィールド画像読込
    this->field_resource  [static_cast<int>(GameMap::OBJECT::NOTHING)]   = QPixmap(tex_dir_path + "/Floor.png");
    this->field_resource  [static_cast<int>(GameMap::OBJECT::TARGET)]    = QPixmap();
    this->field_resource  [static_cast<int>(GameMap::OBJECT::ITEM)]      = QPixmap(tex_dir_path + "/Item.png");
    this->field_resource  [static_cast<int>(GameMap::OBJECT::BLOCK)]     = QPixmap(tex_dir_path + "/Block.png");
    // 行動効果画像読込
    this->overray_resource[static_cast<int>(GameMap::EFFECT::NOTHING)]  = QPixmap();
    this->overray_resource[static_cast<int>(GameMap::EFFECT::LOOK)]     = QPixmap(tex_dir_path + "/Look.png");
    this->overray_resource[static_cast<int>(GameMap::EFFECT::GETREADY)] = QPixmap(tex_dir_path + "/Getready.png");
    this->overray_resource[static_cast<int>(GameMap::EFFECT::SEARCH)]   = QPixmap(tex_dir_path + "/Search.png");
    this->overray_resource[static_cast<int>(GameMap::EFFECT::BLIND)]    = QPixmap(tex_dir_path + "/Blind.png");

    // チーム画像サイズ変更
    for(QPixmap& img:team_resource )img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    // フィールド画像サイズ変更
    for(QPixmap& img:field_resource){
        if(!img.isNull())img = img.scaled(image_part.width(),image_part.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    }
    // 行動効果画像サイズ変更
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
