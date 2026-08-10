/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   動作管理
****************************************************************************/
#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QPoint>                       // 位置
#include <QString>                      // 文字列

// チーム情報
#define TEAM_COUNT 2                    // チーム数

// チーム種別
enum class TEAM{
    COOL,                               // COOL
    HOT,                                // HOT
    UNKNOWN                             // 不明
};

// 動作
struct Operation{
    // 行動
    enum class ACTION{
        WALK,                           // 指定方向移動
        LOOK,                           // 指定方向周囲確認
        SEARCH,                         // 指定方向直線確認
        PUT,                            // 指定方向ブロック配置
        GETREADY,                       // 周辺情報確認
        UNKNOWN,                        // 不明
    };
    // 方向
    enum class ROTE{
        UP,                             // 上
        DOWN,                           // 下
        RIGHT,                          // 右
        LEFT,                           // 左
        UNKNOWN                         // 不明
    };

    TEAM team;                          // チーム
    ACTION action;                      // 行動
    ROTE rote;                          // 方向

    // 方向別座標取得
    QPoint  GetRoteVector();
    // 行動･方向変換
    static Operation fromString(const QString& str);
};

#endif // GAMESYSTEM_H
