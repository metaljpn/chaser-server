/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   マップ編集画面
****************************************************************************/
#ifndef MAPEDITERDIALOG_H
#define MAPEDITERDIALOG_H

#include <QDialog>                      // ダイアログ
#include <QListWidgetItem>              // アイテムリスト
#include "GameSystem.h"                 // マップ管理

namespace Ui {
class MapEditerDialog;
}

class MapEditerDialog : public QDialog
{
    Q_OBJECT

public:
    static const int IMAGE_PART_SIZE = 25;  // イメージサイズ
    static const int ICON_SIZE = 50;        // アイコンサイズ
    QString filepath;                       // マップ保存フルパス

    GameSystem::Map GetMap();               // マップ取得
    explicit MapEditerDialog(GameSystem::Map map,QWidget *parent = 0);
    ~MapEditerDialog();

protected:
    // マウスクリック
    void mousePressEvent(QMouseEvent *event);
    // マウスクリック解放
    void mouseReleaseEvent(QMouseEvent *event);
    // マウス移動
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::MapEditerDialog *ui;            // UI
    bool clicking;                      // クリック状態

private slots:
    void Export();                      // [書き出し]ボタン押下
    void FillItem(const QPoint& pos);   // オブジェクト配置
    void Clear();                       // [全消し]ボタン押下
    void SpinChanged(int value);        // ターン数変更
    void ComboChanged();                // エリア選択
    void ReCount();                     // オブジェクト数更新
    void randomGenerateButtonPressed(); // [ランダム生成]ボタン押下

public slots:
    // オブジェクト選択
    void SelectItem(QListWidgetItem* next);
};

#endif // MAPEDITERDIALOG_H
