/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲームボード管理
****************************************************************************/
#ifndef GRAPHICFIELD_H
#define GRAPHICFIELD_H

#include <QWidget>                          // ユーザーインターフェース
#include <QVector>                          // 動的配列
#include <QPainter>                         // 描画
#include <QResizeEvent>                     // リサイズイベント
#include <QDebug>                           // デバッグ情報
#include <QGraphicsPixmapItem>              // 画像管理
#include <QGraphicsView>                    // ビューポート
#include "GameSystem.h"                     // マップ管理

namespace Ui {
class GameBoard;
}

class GameBoard : public QWidget
{
    Q_OBJECT
private:
    int map_width;                          // マップ幅
    int map_height;                         // マップ高さ

    bool animation = false;                 // アニメーション有効

    QString texture_dir_path;               // 使用テクスチャディレクトリ
    QPixmap team_resource[TEAM_COUNT];      // チーム画像
    QPixmap field_resource[4];              // フィールド画像
    QPixmap overray_resource[5];            // オーバーレイ(行動効果)画像

public:
    GameSystem::Map field;                  // ゲーム盤
    Field<GameSystem::MAP_OVERLAY> overlay; // オーバーレイ(行動効果)

    QSize image_part;                       // 単体画像サイズ
    QPoint team_pos[TEAM_COUNT];            // チーム位置

    int leave_items;                        // 残アイテム数
    int team_score[TEAM_COUNT];             // チーム得点
protected:
    void paintEvent (QPaintEvent *event);   // ペイントイベント
    void resizeEvent(QResizeEvent *event);  // リサイズイベント

public:
    // オブジェクト数算出
    int GetMapObjectCount(GameSystem::MAP_OBJECT mb);

public:
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

    // オーバーレイ(行動効果)消去
    void RefreshOverlay();

    explicit GameBoard(QWidget *parent = 0);

    // アイテム回収
    void PickItem(GameSystem::Method method);

    ~GameBoard();

private:
    Ui::GameBoard *ui;                  // UI

public slots:
    // テクスチャ再読込
    void ReloadTexture(QString texture_dir_path);
};

#endif // GRAPHICFIELD_H
