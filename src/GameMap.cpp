/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ管理
****************************************************************************/
#include "GameMap.h"                    // マップ管理

#include <QFile>                        // ファイルアクセス
#include <QRandomGenerator>             // 乱数発生
#include <QMessageBox>                  // メッセージボックス

/****************************************************************************
*   コンストラクタ
****************************************************************************/
GameMap::GameMap():
    turn(50),                                   // ターン数
    name("[DEFAULT MAP]"),                      // マップ名
    size(DEFAULT_MAP_WIDTH,DEFAULT_MAP_HEIGHT), // サイズ
    texture_dir_path(":/Image/Jewel")           // テクスチャパス
{
}

/****************************************************************************
*   サイズ設定
*
*   @param size      サイズ情報
*   @param block_num ブロック数
*   @param item_num  アイテム数
*   @param isOldMap  旧マップ
****************************************************************************/
void GameMap::SetSize(QPoint size, int block_num, int item_num, bool isOldMap){
    // サイズ取得
    this->size = size;
    // ランダム生成
    this->CreateRandomMap(block_num, item_num, isOldMap);
}

/****************************************************************************
*   対称位置取得
*
*   @return 対称位置情報
*
*   @param pos 位置情報
****************************************************************************/
QPoint GameMap::MirrorPoint(const QPoint& pos){
    // 中心座標算出
    QPoint center(size.x() / 2.0f, size.y() / 2.0f);
    // 戻り値[対称位置情報]
    return center * 2 - pos;
}

/****************************************************************************
*   ランダムマップ生成
*
*   @param block_num ブロック数
*   @param item_num  アイテム数
*   @param isOldMap  旧マップ
****************************************************************************/
void GameMap::CreateRandomMap(int block_num, int item_num, bool isOldMap){
    turn = 100;                         // ターン数

    // 新マップ
    if (!isOldMap) 
    {
        name = "[RANDOM MAP]";              // マップ名

        do {
            // ランダム座標取得
            auto pos = QPoint(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            if (pos.x() < size.x() / 2 ||                               // 盤面の左側 or
                (pos.x() == size.x() / 2 && pos.y() < size.y() / 2)) {  // 盤面中心の縦列 and 盤面の上半分
                // COOL側初期位置取得
                team_first_point[0] = pos;
                // ループ脱出
                break;
            }
        // 座標確定まで
        } while (true);

        // HOT側の初期位置を点対称に配置
        team_first_point[1] = MirrorPoint(team_first_point[0]);
        team_first_point[1] = QPoint(99,99);

        // フィールド初期化
        field.clear();
        // フィールドY軸数分
        for (int i = 0; i < size.y(); i++) {
            // 1行分のフィールドデータを取得
            field.push_back(QVector<GameMap::OBJECT>(size.x()));
        }

        // ブロック配置
        for (int i = 0; i < block_num / 2; i++) {
            // ランダム座標取得
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            // ブロック配置有効 and ブロック以外
            if (CheckBlockRole(pos) &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::BLOCK ) {
                // ブロック配置
                field[pos.y()][pos.x()] = GameMap::OBJECT::BLOCK;
                // 点対称座標取得
                auto mirrorPos = MirrorPoint(pos);
                // 点対称にブロック配置
                field[mirrorPos.y()][mirrorPos.x()] = GameMap::OBJECT::BLOCK;
            }
            else {
                // 座標再取得
                i--;
            }
        }

        // アイテム配置
        for (int i = 0; i < item_num / 2; i++) {
            // ランダム座標取得
            QPoint pos(QRandomGenerator::global()->generate() % size.x(), QRandomGenerator::global()->generate() % size.y());

            // チームの初期座標以外 and アイテム以外 and ブロック以外
            if (pos != team_first_point[0] && pos != team_first_point[1] &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::ITEM &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::BLOCK ) {
                // アイテム配置
                field[pos.y()][pos.x()] = GameMap::OBJECT::ITEM;
                // 点対称座標取得
                auto mirrorPos = MirrorPoint(pos);
                // 点対称にアイテム配置
                field[mirrorPos.y()][mirrorPos.x()] = GameMap::OBJECT::ITEM;
            }
            else {
                // 座標再取得
                i--;
            }
        }
    }
    else
    {
        name = "[OLD RANDOM MAP]";

        // 一様ノルム(L∞ノルム)
        auto UniformNorm = [](QPoint pos){
            // 戻り値[絶対値最大]
            return std::abs(pos.x()) > std::abs(pos.y()) ? std::abs(pos.x()) : std::abs(pos.y());
        };

        do {
            // ランダム座標取得
            auto pos = QPoint(QRandomGenerator::global()->generate() % size.x(),QRandomGenerator::global()->generate() % size.y());
            // 取得座標が盤面中心にあるアイテムの周囲
            if(UniformNorm(pos - QPoint(size.x()/2, size.y()/2)) <= 1)
                // 座標再取得
                continue;
            if(pos.x() < size.x()/2 ||                              // 盤面の左側 or
                (pos.x() == size.x()/2 && pos.y() < size.y()/2)){   // 盤面中心の縦列 and 盤面の上半分
                // COOL側初期位置取得
                team_first_point[0] = pos;
                // ループ脱出
                break;
            }
        // 座標確定まで
        } while(true);

        // HOT側の初期位置を点対称に配置
        team_first_point[1] = MirrorPoint(team_first_point[0]);
        team_first_point[1] = QPoint(99,99);

        // フィールド初期化
        field.clear();
        // フィールドY軸数分
        for(int i=0;i<size.y();i++){
            // 1行分のフィールドデータを取得
            field.push_back(QVector<GameMap::OBJECT>(size.x()));
        }

        // ブロック配置
        for(int i=0;i<block_num/2;i++){
            // ランダム座標取得
            QPoint pos(QRandomGenerator::global()->generate() % size.x(),QRandomGenerator::global()->generate() % size.y());
            // 点対称座標取得
            auto mirrorPos = MirrorPoint(pos);

            // 旧ブロック配置有効 and ブロック以外 and マップ中心以外
            if(CheckBlockRoleOld(pos) &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::BLOCK &&
                pos != QPoint(size.x()/2, size.y()/2 )){
                // ブロック配置
                field[pos.y()][pos.x()] = GameMap::OBJECT::BLOCK;
                // 点対称にブロック配置
                field[mirrorPos.y()][mirrorPos.x()] = GameMap::OBJECT::BLOCK;
            }else{
                // 座標再取得
                i--;
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
                around_item_flag = false;

            // 配置有効 and チームの初期座標以外 and アイテム以外 and ブロック以外 and マップ中心以外
            if(around_item_flag &&
                pos != team_first_point[0] && pos != team_first_point[1] &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::ITEM &&
                field[pos.y()][pos.x()] != GameMap::OBJECT::BLOCK &&
                pos != QPoint(size.x()/2, size.y()/2) ){
                // アイテム配置
                field[pos.y()][pos.x()] = GameMap::OBJECT::ITEM;
                // 点対称にアイテム配置
                field[mirrorPos.y()][mirrorPos.x()] = GameMap::OBJECT::ITEM;
            }else{
                // 座標再取得
                i--;
            }
        }
        // マップ中心にアイテムを配置
        field[size.y()/2][size.x()/2] = GameMap::OBJECT::ITEM;
    }
}

/****************************************************************************
*   設定読込
*
*   @return 処理結果
*
*   @param Filename マップファイル名
****************************************************************************/
bool GameMap::Import(QString Filename){
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
                QVector<GameMap::OBJECT> vec;
                // 座標数分
                foreach(QString s,list){
                    // 座標取得
                    vec.push_back(static_cast<GameMap::OBJECT>(s.toInt()));
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
                if(str[0]==getTeamName(static_cast<TEAM>(i)).at(0)){
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
*   設定書込
*
*   @return 処理結果
*
*   @param Filename マップファイル名
****************************************************************************/
bool GameMap::Export(QString Filename){
    // マップファイル
    QFile file(Filename);
    // 拡張子取得
    QString fileExt = Filename.split("/").last().split(".").last();

    // 拡張子が"txt"ならば追記書込オープン、それ以外は書込オープン
    QIODevice::OpenMode openMode = (fileExt == "txt") ? 
        (QIODevice::WriteOnly | QIODevice::Append) : QIODevice::WriteOnly;
    // オープン失敗
    if (!file.open(openMode)) {
        // 異常通知
        QMessageBox::information(nullptr, "ファイルを開けません", file.errorString());
        // 戻り値[異常]
        return false;
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
    for(const auto &v1 : std::as_const(field)){
        // マップ
        stream << "D:";
        // 座標数分
        for(auto it = v1.begin();it != v1.end();it++){
            // 座標出力
            stream << QString::number(static_cast<int>(*it));
            // 区切り文字列出力
            if(it != v1.end()-1)stream << ",";
        }
        // 改行
        stream << "\n";
    }
    // チーム数分
    for(int i=0;i<TEAM_COUNT;i++){
        // チーム名及び初期位置を出力
        stream << QString(getTeamName(static_cast<TEAM>(i)).at(0))
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
*   チーム名先頭文字取得
*
*   @return 文字列
*
*   @param team チーム種別
****************************************************************************/
QString GameMap::getTeamName(TEAM team){
    // COOL文字列
    if(team == TEAM::COOL)return "C";
    // HOT文字列
    if(team == TEAM::HOT) return "H";
    // 不明
    return "UNKNOWN";
}

/****************************************************************************
*   ブロック配置確認
*
*   @return 配置可否
*
*   @param pos 位置情報
****************************************************************************/
bool GameMap::CheckBlockRole(QPoint pos){
	// 各チームの初期位置にブロックがあってはいけない
	if (pos == team_first_point[0] || pos == team_first_point[1])
		return false;

    // COOL側 X軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[0] + QPoint(9, 0) || pos == team_first_point[0] + QPoint(-9, 0))
        return false;
    // HOT側 X軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[1] + QPoint(9, 0) || pos == team_first_point[1] + QPoint(-9, 0))
        return false;
    // COOL側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[0] + QPoint(0, 9) || pos == team_first_point[0] + QPoint(0, -9))
        return false;
    // HOT側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if(pos == team_first_point[1] + QPoint(0, 9) || pos == team_first_point[1] + QPoint(0, -9))
        return false;

    // チーム数分
	for (int i = 0; i < TEAM_COUNT; i++) {
		if((pos.x() == 0 || pos.x() == size.x() - 1) &&                                         // 外周(X=0 or X=size.x()-1) and
	       (pos.y() == team_first_point[i].y()) &&                                              // ブロックを置こうとしているY座標がチームのY座標と同じ and
		   (team_first_point[i].x() - 9 <= pos.x() && pos.x() <= team_first_point[i].x() + 9))  // チームのSearch範囲(X軸)はブロックであってはいけない
            return false;

		if((pos.y() == 0 || pos.y() == size.y() - 1) &&                                         // 外周(Y=0 or Y=size.y()-1) and
		   (pos.x() == team_first_point[i].x()) &&                                              // ブロックを置こうとしているX座標が、チームのX座標と同じ and
		   (team_first_point[i].y() - 9 <= pos.y() && pos.y() <= team_first_point[i].y() + 9))  // チームのSearch範囲(Y軸)はブロックであってはいけない
            return false;
	}

    // 戻り値[配置可]
    return true;
}

/****************************************************************************
*   旧ブロック配置確認
*
*   @return 配置可否
*
*   @param pos 位置情報
****************************************************************************/
bool GameMap::CheckBlockRoleOld(QPoint pos) {
    // 各チームの初期位置にブロックがあってはいけない
    if (pos == team_first_point[0] || pos == team_first_point[1])
        return false;

    // 外周にブロックがあってはいけない
    else if (pos.x() == 0 || pos.x() == size.x() - 1 || pos.y() == 0 || pos.y() == size.y() - 1)
        return false;

    // COOL側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if (pos == team_first_point[0] + QPoint(0, 9) || pos == team_first_point[0] + QPoint(0, -9))
        return false;
    // HOT側 Y軸に対するSearchをしたとき、その終点にブロックがあってはいけない
    else if (pos == team_first_point[1] + QPoint(0, 9) || pos == team_first_point[1] + QPoint(0, -9))
        return false;
    else
        return true;
}

/****************************************************************************
*   周辺情報-文字列変換
*
*   @return 文字列
****************************************************************************/
QString AroundData::toString(){
    // 文字列
    QString str;

    // 接続状態
    str.append(static_cast<QChar>('0' + static_cast<int>(this->connect)));
    // 周辺情報数分
    for(int i=0;i<9;i++){
        if     (this->data[i] == GameMap::OBJECT::NOTHING)str.append('0');  // 無し
        else if(this->data[i] == GameMap::OBJECT::TARGET) str.append('1');  // 相手
        else if(this->data[i] == GameMap::OBJECT::BLOCK)  str.append('2');  // ブロック
        else if(this->data[i] == GameMap::OBJECT::ITEM)   str.append('3');  // アイテム
    }
    // 戻り値[文字列]
    return str;
}

/****************************************************************************
*   周辺情報-ゲーム終了
****************************************************************************/
void AroundData::Finish(){
    // 周辺情報先頭を接続終了
    this->data[0] = static_cast<GameMap::OBJECT>(CONNECTING_STATUS::FINISHED);
    // 接続終了
    this->connect = CONNECTING_STATUS::FINISHED;
}
