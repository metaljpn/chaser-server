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
    // 探索状態
    enum class DISCOVER{
        UNKNOWN,                        // 未探索
        EXPLORED,                       // 探索済
    };

    Field<GameSystem::MAP_OBJECT> field;    // 物体フィールド
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

#endif // GAMEMAP_H
