/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ管理
****************************************************************************/
#include "GameSystem.h"                 // マップ管理

#include <QFile>                        // ファイルアクセス
#include <QMessageBox>                  // メッセージボックス
#include <QRandomGenerator>             // 乱数発生

/****************************************************************************
*   チーム名文字列変換
*
*   @return 文字列
*
*   @param team チーム種別
****************************************************************************/
QString GameSystem::TEAM_PROPERTY::getTeamName(GameSystem::TEAM team){
    // COOL文字列
    if(team == GameSystem::TEAM::COOL)return "COOL";
    // HOT文字列
    if(team == GameSystem::TEAM::HOT) return "HOT";
    // 不明
    return "UNKNOWN";
}

/****************************************************************************
*   方向別座標取得
*
*   @return 座標
****************************************************************************/
QPoint GameSystem::Method::GetRoteVector(){
    // 方向ベクトル生成
    switch(this->rote){
        case GameSystem::Method::ROTE::UP:    return QPoint( 0,-1); // 上
        case GameSystem::Method::ROTE::RIGHT: return QPoint( 1, 0); // 右
        case GameSystem::Method::ROTE::DOWN:  return QPoint( 0, 1); // 下
        case GameSystem::Method::ROTE::LEFT:  return QPoint(-1, 0); // 左
        default:                              return QPoint( 0, 0); // 不明
    }
}

/****************************************************************************
*   行動･方向変換
*
*   @return 行動･方向情報
*
*   @param str 行動･方向文字列
****************************************************************************/
GameSystem::Method GameSystem::Method::fromString(const QString& str){
    // 文字列から抽出
    GameSystem::Method answer;

    if     (str[0] == 'w')answer.action = GameSystem::Method::ACTION::WALK;     // 指定方向移動
    else if(str[0] == 'l')answer.action = GameSystem::Method::ACTION::LOOK;     // 指定方向周囲確認
    else if(str[0] == 's')answer.action = GameSystem::Method::ACTION::SEARCH;   // 指定方向直線確認
    else if(str[0] == 'p')answer.action = GameSystem::Method::ACTION::PUT;      // 指定方向ブロック配置
    else                  answer.action = GameSystem::Method::ACTION::UNKNOWN;  // 不明
    if     (str[1] == 'u')answer.rote   = GameSystem::Method::ROTE::UP;         // 上
    else if(str[1] == 'd')answer.rote   = GameSystem::Method::ROTE::DOWN;       // 下
    else if(str[1] == 'r')answer.rote   = GameSystem::Method::ROTE::RIGHT;      // 右
    else if(str[1] == 'l')answer.rote   = GameSystem::Method::ROTE::LEFT;       // 左
    else                  answer.rote   = GameSystem::Method::ROTE::UNKNOWN;    // 不明
    // 戻り値[行動･方向情報]
    return answer;
}

/****************************************************************************
*   マップコンストラクタ
****************************************************************************/
GameSystem::Map::Map():
    // ターン数
    turn(100),
    // マップ名
    name("[DEFAULT MAP]"),
    // サイズ
    size(DEFAULT_MAP_WIDTH,DEFAULT_MAP_HEIGHT),
    // テクスチャパス
    texture_dir_path(":/Image/Jewel"){
}

/****************************************************************************
*   サイズ設定
*
*   @param size      サイズ情報
*   @param block_num ブロック数
*   @param item_num  アイテム数
****************************************************************************/
void GameSystem::Map::SetSize(QPoint size, int block_num, int item_num){
    // サイズ取得
    this->size = size;
    // ランダム生成
    this->CreateRandomMap(block_num, item_num);
}

/****************************************************************************
*   対称位置取得
*
*   @return 対称位置情報
*
*   @param pos 位置情報
****************************************************************************/
QPoint GameSystem::Map::MirrorPoint(const QPoint& pos){
    // 中心座標算出
    QPoint center(size.x() / 2.0f, size.y() / 2.0f);
    // 戻り値[対称位置情報]
    return center * 2 - pos;
}

/****************************************************************************
*   設定読込
*
*   @return 処理結果
*
*   @param Filename マップファイル名
****************************************************************************/
bool GameSystem::Map::Import(QString Filename){
    // マップファイル
    QFile file(Filename);
    // 読取専用ファイルオープン
    if (file.open(QIODevice::ReadOnly)){
        char buf[1024]={};              // ラインバッファ
        int calm=0;                     // 行カウンタ
        // 読込可能の間
        while(file.readLine(buf,1024) != -1){
            // 文字列として取得
            QString str = QString::fromLatin1(buf);
            // 改行削除
            str.replace("\r","");
            str.replace("\n","");

            // マップ名
            if(str[0]=='N')name = str.remove(0,2);
            // ターン数
            if(str[0]=='T')turn = str.remove(0,2).toInt();


            // マップ
            if(str[0]=='D'){
                // 座標リスト取得
                QStringList list = str.remove(0,2).split(",");
                QVector<GameSystem::MAP_OBJECT> vec;
                // 座標数分
                foreach(QString s,list){
                    // 座標取得
                    vec.push_back(static_cast<GameSystem::MAP_OBJECT>(s.toInt()));
                }
                // サイズ設定
                size.setX(vec.size());
                // 座標設定
                field[calm] = vec;
                // 行カウンタ更新
                calm++;
            }

            // チーム数分
            for(int i=0;i<TEAM_COUNT;i++){
                // チーム名検出
                if(str[0]==GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i))[0]){
                    // 座標取得
                    QStringList list = str.remove(0,2).split(",");
                    // 初期位置設定
                    team_first_point[i] = QPoint(list[0].toInt(),list[1].toInt());
                }
            }
        }
        // 戻り値[正常]
        return true;
    }else{
        // 戻り値[異常]
        return false;
    }

}

/****************************************************************************
*   設定保存
*
*   @return 処理結果
*
*   @param Filename マップファイル名
****************************************************************************/
bool GameSystem::Map::Export(QString Filename){
    // マップファイル
    QFile file(Filename);

    // 拡張子取得
    QString fileExt = Filename.split("/").last().split(".").last();

    // 拡張子が"txt"
    if (fileExt == "txt") {
        // 追記書込オープン
        file.open(QIODevice::WriteOnly | QIODevice::Append);
    } else {
        // 書込専用以外
        if (!file.open(QIODevice::WriteOnly)) {
            // オープン異常通知
            QMessageBox::information(nullptr, "ファイルを開けません", file.errorString());
            // 戻り値[異常]
            return false;
        }
    }

    // マップファイル名
    QString outname(Filename.split("/").last().remove(".map"));
    // ファイルストリーム
    QTextStream stream( &file );

    // マップファイル名
    stream << "N:" + outname << "\n";
    // ターン数
    stream << "T:" + QString::number(this->turn) + "\n";
    // サイズ
    stream << "S:" + QString::number(size.x()) + "," + QString::number(size.y()) + "\n";
    // フィールド数分
    for(auto v1 : field){
        stream << "D:";
        for(auto it = v1.begin();it != v1.end();it++){
            // 座標出力
            stream << QString::number(static_cast<int>(*it));
            // 区切り文字列出力
            if(it != v1.end()-1)stream << ",";
        }
        stream << "\n";
    }
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // チーム名及び初期位置を出力
        stream << QString(GameSystem::TEAM_PROPERTY::getTeamName(static_cast<GameSystem::TEAM>(i))[0])
                + ":"
                + QString::number(team_first_point[i] .x())
                + ","
                + QString::number(team_first_point[i] .y());
        // 改行
        stream << "\n";
    }
    // ファイルクローズ
    file.close();
    // 戻り値[正常]
    return true;
}

/****************************************************************************
*   ブロック配置確認
*
*   @return 配置可否
*
*   @param pos 位置情報
****************************************************************************/
bool GameSystem::Map::CheckBlockRole(QPoint pos){
    // 外周にブロックがあってはいけない
    if(pos.x() == 0 || pos.x() == size.x()-1 || pos.y() == 0|| pos.y() == size.y()-1)
        return false;
    // COOL側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[0] + QPoint(0, 9) || pos == team_first_point[0] + QPoint(0, -9))
        return false;
    // HOT側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[1] + QPoint(0, 9) || pos == team_first_point[1] + QPoint(0, -9))
        return false;
    else
        return true;
}

/****************************************************************************
*   ランダムマップ生成
*
*   @param block_num ブロック数
*   @param item_num  アイテム数
****************************************************************************/
void GameSystem::Map::CreateRandomMap(int block_num, int item_num){
    turn = 100;                         // ターン数
    name = "[RANDOM MAP]";              // マップ名

    // 一様ノルム(L∞ノルム)
    auto UniformNorm = [](QPoint pos){
        return std::abs(pos.x()) > std::abs(pos.y()) ? std::abs(pos.x()) : std::abs(pos.y());
    };

    do{
        // ランダム座標取得
        auto pos = QPoint(QRandomGenerator::global()->generate() % size.x(),QRandomGenerator::global()->generate() % size.y());
        // 取得座標が盤面中心にあるアイテムの周囲
        if(UniformNorm(pos - QPoint(size.x()/2, size.y()/2)) <= 1)
            // 座標再取得
            continue;
        if(pos.x() < size.x()/2 ||                          // 盤面の左側
          (pos.x() == size.x()/2 && pos.y() < size.y()/2)){ // 盤面中心の縦列 and 盤面の上半分
            // COOL側初期位置取得
            team_first_point[0] = pos;
            // ループ脱出
            break;
        }

    }while(true);

    // HOT側の初期位置を点対称に配置
    team_first_point[1] = MirrorPoint(team_first_point[0]);

    // フィールド初期化
    field.clear();
    // フィールドY軸数分
    for(int i=0;i<size.y();i++){
        // 1行分のフィールドデータを取得
        field.push_back(QVector<GameSystem::MAP_OBJECT>(size.x()));
    }

    // ブロック配置
    for(int i=0;i<block_num/2;i++){
        // ランダム座標取得
        QPoint pos(QRandomGenerator::global()->generate() % size.x(),QRandomGenerator::global()->generate() % size.y());
        // 点対称座標取得
        auto mirrorPos = MirrorPoint(pos);

        // ブロック配置有効 and プレイヤー初期値位置以外 and ブロック以外 and マップ中心以外
        if(CheckBlockRole(pos) &&
            pos != team_first_point[0] &&
            pos != team_first_point[1] &&
            field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK &&
            pos != QPoint(size.x()/2, size.y()/2 )){
            // ブロック配置
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::BLOCK;

            // 点対称にブロック配置
            field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::BLOCK;
        }else{
            // 座標再取得
            i--;
            continue;
        }
    }

    // アイテム配置
    for(int i=0;i<item_num/2;i++){
        // ランダム座標取得
        QPoint pos(QRandomGenerator::global()->generate() % size.x(),QRandomGenerator::global()->generate() % size.y());
        // 点対称座標取得
        auto mirrorPos = MirrorPoint(pos);
        // 配置有効フラグ
        bool around_item_flag = true;

        // プレイヤー初期座標の周辺
        if(UniformNorm(team_first_point[0]  - pos) <= 1 || UniformNorm(team_first_point[1] - pos) <= 1)
            // 配置無効
            around_item_flag=false;

        // 配置有効 and アイテム以外 and ブロック以外 and マップ中心以外
        if(around_item_flag &&
            field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::ITEM &&
            field[pos.y()][pos.x()] != GameSystem::MAP_OBJECT::BLOCK &&
            pos != QPoint(size.x()/2, size.y()/2) ){
            // アイテム配置
            field[pos.y()][pos.x()] = GameSystem::MAP_OBJECT::ITEM;
            //点対称にアイテム配置
            field[mirrorPos.y()][mirrorPos.x()] = GameSystem::MAP_OBJECT::ITEM;
        }else{
            // 座標再取得
            i--;
            continue;
        }
    }
    // マップ中心にアイテムを配置
    field[size.y()/2][size.x()/2] = GameSystem::MAP_OBJECT::ITEM;
}

/****************************************************************************
*   ゲーム終了
****************************************************************************/
void GameSystem::AroundData::finish(){
    // 周辺情報先頭を接続終了
    this->data[0] = static_cast<GameSystem::MAP_OBJECT>(GameSystem::CONNECTING_STATUS::FINISHED);
    // 接続終了
    this->connect = GameSystem::CONNECTING_STATUS::FINISHED;
}

/****************************************************************************
*   周辺情報文字列変換
*
*   @return 文字列
****************************************************************************/
QString GameSystem::AroundData::toString(){
    // 文字列
    QString str;
    // 接続状態
    str.append(static_cast<QChar>('0' + static_cast<int>(this->connect)));
    // 周辺情報数分
    for(int i=0;i<9;i++){
        if     (this->data[i] == GameSystem::MAP_OBJECT::NOTHING)str.append('0');   // 無し
        else if(this->data[i] == GameSystem::MAP_OBJECT::TARGET) str.append('1');   // 相手
        else if(this->data[i] == GameSystem::MAP_OBJECT::BLOCK)  str.append('2');   // ブロック
        else if(this->data[i] == GameSystem::MAP_OBJECT::ITEM)   str.append('3');   // アイテム
    }
    // 戻り値[文字列]
    return str;
}
