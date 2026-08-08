/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲームボード管理
****************************************************************************/
#ifndef GRAPHICFIELD_H
#define GRAPHICFIELD_H

#include <QWidget>                          // ユーザーインターフェース
#include "GameSystem.h"                     // マップ管理

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT

private:
    int map_height;                         // マップ高さ
    int map_width;                          // マップ幅
    bool animation = false;                 // アニメーション有効
    QString texture_dir_path;               // 使用テクスチャディレクトリ
    QPixmap team_resource[TEAM_COUNT];      // チーム画像
    QPixmap field_resource[4];              // フィールド画像
    QPixmap overray_resource[5];            // オーバーレイ(行動効果)画像

    Ui::GameBoard *ui;                      // UI

    void paintEvent(QPaintEvent *event);        // ペイントイベント
    void resizeEvent(QResizeEvent *event);      // リサイズイベント
    void PickItem(GameSystem::Method method);   // アイテム取得
    void LoadTexture(QString texture_dir_path); // テクスチャ読込

public:
    GameSystem::Map field;                  // ゲーム盤
    Field<GameSystem::MAP_OVERLAY> overlay; // オーバーレイ(行動効果)
    QSize image_part;                       // 単体画像サイズ
    QPoint team_pos[TEAM_COUNT];            // チーム位置
    int leave_items;                        // 残アイテム数
    int team_score[TEAM_COUNT];             // チーム得点

    // 指定位置物体種別取得
    GameSystem::MAP_OBJECT FieldAccess(GameSystem::Method method, const QPoint& pos);
    // 周辺確認
    GameSystem::AroundData FieldAccessAround(GameSystem::TEAM team);
    // 指定位置周辺確認
    GameSystem::AroundData FieldAccessAround(GameSystem::Method method, const QPoint& center);
    // 行動後周辺確認
    GameSystem::AroundData FieldAccessMethod(GameSystem::Method method);
    // 接続終了
    GameSystem::AroundData FinishConnecting(GameSystem::TEAM team);

    // マップ設定
    void setMap(const GameSystem::Map &map);
    // オブジェクト数算出
    int GetMapObjectCount(GameSystem::MAP_OBJECT mb);
    // オーバーレイ(行動効果)初期化
    void ResetOverlay();

    explicit GameBoard(QWidget *parent = 0);
    ~GameBoard();
};

#endif // GRAPHICFIELD_H
