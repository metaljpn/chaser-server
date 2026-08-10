/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ管理
****************************************************************************/
#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QVector>                      // 動的配列
#include "GameSystem.h"

// 二次元配列エイリアス
template <class T>
using Field = QVector<QVector<T>>;

struct GameMap
{
private:
    // マップサイズ
    const static int DEFAULT_MAP_WIDTH  = 15;
    const static int DEFAULT_MAP_HEIGHT = 17;

public:
    // マップ上に存在する物体
    enum class OBJECT{
        NOTHING,                        // 無し
        TARGET,                         // 相手
        BLOCK,                          // ブロック
        ITEM,                           // アイテム
    };

    // 探索状態
    enum class DISCOVER{
        UNKNOWN,                        // 未探索
        EXPLORED,                       // 探索済
    };

    // マップ上に描画する非物体(行動効果)
    enum class OVERLAY{
        NOTHING,                        // 無し
        LOOK,                           // 指定方向周囲確認
        SEARCH,                         // 指定方向直線確認
        GETREADY,                       // 周辺情報確認
        BLIND,                          // 暗闇
        ERASE,                          // 消去(未選択)
    };

    Field<OBJECT> field;                    // 物体フィールド
    Field<DISCOVER> discover;               // 探索状態
    int turn;                               // ターン
    QString name;                           // ステージ名
    QPoint size;                            // マップサイズ
    QPoint team_first_point[TEAM_COUNT];    // チーム初期位置
    QString texture_dir_path;               // テクスチャパス

    GameMap();

    // サイズ設定
    void SetSize(QPoint size, int block_num = 20, int item_num = 51);
    // 対称位置取得
    QPoint MirrorPoint(const QPoint& pos);
    // ランダムマップ生成
    void CreateRandomMap(int block_num = 20, int item_num = 51);
    // 設定読込
    bool Import(QString Filename);
    // 設定書込
    bool Export(QString Filename);
    // チーム名先頭文字取得
    QString getTeamName(GameSystem::TEAM team);
    // ブロック配置確認
    bool CheckBlockRole(QPoint pos);
};

// 周辺情報
struct AroundData{
    // 接続状態
    enum class CONNECTING_STATUS{
        FINISHED,                       // 終了
        CONTINUE,                       // 継続
    };

    CONNECTING_STATUS connect;          // 接続状態
    GameMap::OBJECT data[9];            // 周辺情報

    QString toString();                 // 文字列変換
    void Finish();                      // ゲーム終了
};

#endif // GAMEMAP_H
