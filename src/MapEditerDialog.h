/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ編集画面
****************************************************************************/
#ifndef MAPEDITERDIALOG_H
#define MAPEDITERDIALOG_H

#include <QDialog>                      // ダイアログ
#include <QListWidgetItem>              // アイテムリスト
#include "GameMap.h"                    // マップ管理

namespace Ui {
class MapEditerDialog;
}

class MapEditerDialog : public QDialog
{
    Q_OBJECT

private:
    static const int ICON_SIZE = 50;    // アイコンサイズ

    bool clicking;                      // クリック状態

    Ui::MapEditerDialog *ui;            // UI

    void ReCount();                             // オブジェクト数更新
    void mousePressEvent(QMouseEvent *event);   // マウスクリック
    void mouseMoveEvent(QMouseEvent *event);    // マウス移動
    void FillItem(const QPoint& pos);           // オブジェクト配置
    void mouseReleaseEvent(QMouseEvent *event); // マウスクリック解放

public:
    QString filepath;                   // マップ保存フルパス
    GameMap GetMap();                   // マップ取得

    explicit MapEditerDialog(GameMap map,QWidget *parent = 0);
    ~MapEditerDialog();

private slots:
    void ComboChanged();                // エリア選択
    void SpinChanged(int value);        // ターン数変更
    void randomGenerateButtonPressed(); // [ランダム生成]ボタン押下
    void Clear();                       // [全消し]ボタン押下
    void Export();                      // [書き出し]ボタン押下
    // オブジェクト選択
    void SelectItem(QListWidgetItem* next);
};

#endif // MAPEDITERDIALOG_H
