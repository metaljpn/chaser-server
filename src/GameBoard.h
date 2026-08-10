/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲームボード管理
****************************************************************************/
#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>                          // ユーザーインターフェース
#include "GameSystem.h"                     // 動作管理
#include "GameMap.h"                        // マップ管理

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT

private:
    int map_height;                         // マップ高さ
    int map_width;                          // マップ幅
    QString texture_dir_path;               // 使用テクスチャディレクトリ
    QPixmap team_resource[TEAM_COUNT];      // チーム画像
    QPixmap field_resource[4];              // フィールド画像
    QPixmap overray_resource[5];            // 行動効果画像

    Ui::GameBoard *ui;                      // UI

    void paintEvent(QPaintEvent *event);        // ペイントイベント
    void resizeEvent(QResizeEvent *event);      // リサイズイベント
    void PickItem(Operation method);            // アイテム取得
    void LoadTexture(QString texture_dir_path); // テクスチャ読込

public:
    GameMap map;                            // マップ
    Field<GameMap::EFFECT> effect;          // 行動効果
    QSize image_part;                       // 単体画像サイズ
    QPoint team_pos[TEAM_COUNT];            // チーム位置
    int leave_items;                        // 残アイテム数
    int team_score[TEAM_COUNT];             // チーム得点

    // 指定位置物体種別取得
    GameMap::OBJECT FieldAccess(Operation method, const QPoint& pos);
    // 周辺確認
    AroundData FieldAccessAround(TEAM team);
    // 指定位置周辺確認
    AroundData FieldAccessAround(Operation method, const QPoint& center);
    // 行動後周辺確認
    AroundData FieldAccessOperation(Operation method);

    // マップ設定
    void setMap(const GameMap &mapinfo);
    // オブジェクト数算出
    int GetMapObjectCount(GameMap::OBJECT mb);
    // 行動効果初期化
    void ResetEffect();

    explicit GameBoard(QWidget *parent = 0);
    ~GameBoard();
};

#endif // GAMEBOARD_H
