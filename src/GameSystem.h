/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲーム管理
****************************************************************************/
#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QPoint>                       // 位置
#include <QString>                      // 文字列

// チーム情報マクロ
#define TEAM_COUNT 2                    // チーム数
#define TEAMS COOL,HOT                  // チーム名

class GameSystem
{
public:
    // チーム種別
    enum class TEAM{
        TEAMS,
        UNKNOWN
    };

    // 勝者
    enum class WINNER{
        TEAMS,                          // チーム
        DRAW,                           // 引き分け
        CONTINUE,                       // 継続
    };

    // 接続状態
    enum class CONNECTING_STATUS{
        FINISHED,                       // 終了
        CONTINUE,                       // 継続
    };

    // マップ上に存在する物体
    enum class MAP_OBJECT{
        NOTHING,                        // 無し
        TARGET,                         // 相手
        BLOCK,                          // ブロック
        ITEM,                           // アイテム
    };

    // マップ上に描画する非物体(行動効果)
    enum class MAP_OVERLAY{
        NOTHING,                        // 無し
        LOOK,                           // 指定方向周囲確認
        SEARCH,                         // 指定方向直線確認
        GETREADY,                       // 周辺情報確認
        BLIND,                          // 暗闇
        ERASE,                          // 消去(未選択)
    };

    // クライアント行動
    struct Method{
        // 行動
        enum class ACTION{
            WALK,                       // 指定方向移動
            LOOK,                       // 指定方向周囲確認
            SEARCH,                     // 指定方向直線確認
            PUT,                        // 指定方向ブロック配置
            GETREADY,                   // 周辺情報確認
            UNKNOWN,                    // 不明
        };
        // 方向
        enum class ROTE{
            UP,                         // 上
            DOWN,                       // 下
            RIGHT,                      // 右
            LEFT,                       // 左
            UNKNOWN                     // 不明
        };

        TEAM   team;                    // チーム
        ACTION action;                  // 行動
        ROTE   rote;                    // 方向

        // 方向座標取得
        QPoint  GetRoteVector();
        // 行動･方向変換
        static Method fromString(const QString& str);
    };

    // 周辺情報
    struct AroundData{
        CONNECTING_STATUS connect;      // 接続状態
        MAP_OBJECT data[9];             // 周辺情報

        QString toString();             // 文字列変換
        void Finish();                  // ゲーム終了
    };
};

#endif // GAMESYSTEM_H
