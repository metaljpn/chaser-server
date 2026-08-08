/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ管理
****************************************************************************/
#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H

#include <QPoint>                       // 位置
#include <QVector>                      // 動的配列

// 二次元配列エイリアス
template <class T>
using Field = QVector<QVector<T>>;

// チーム情報マクロ
#define TEAM_COUNT 2                    // チーム数
#define TEAMS COOL,HOT                  // チーム名

class GameSystem
{
private:
    // マップサイズ
    const static int DEFAULT_MAP_WIDTH  = 15;
    const static int DEFAULT_MAP_HEIGHT = 17;

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

    // 探索状態
    enum class DISCOVER{
        UNKNOWN,                        // 未探索
        EXPLORED,                       // 探索済
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

    // ゲーム盤
    struct Map{
        Field<GameSystem::MAP_OBJECT> field;    // 物体フィールド
        Field<GameSystem::DISCOVER> discover;   // 探索状態
        int turn;                               // ターン
        QString name;                           // ステージ名
        QPoint size;                            // マップサイズ
        QPoint team_first_point[TEAM_COUNT];    // チーム初期位置
        QString texture_dir_path;               // テクスチャパス

        Map();

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
