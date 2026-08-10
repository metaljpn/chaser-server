/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   動作管理
****************************************************************************/
#include "GameSystem.h"                 // 動作管理

/****************************************************************************
*   方向別座標取得
*
*   @return 座標
****************************************************************************/
QPoint Operation::GetRoteVector(){
    // 方向ベクトル生成
    switch(this->rote){
        case Operation::ROTE::UP:    return QPoint( 0,-1); // 上
        case Operation::ROTE::RIGHT: return QPoint( 1, 0); // 右
        case Operation::ROTE::DOWN:  return QPoint( 0, 1); // 下
        case Operation::ROTE::LEFT:  return QPoint(-1, 0); // 左
        default:                     return QPoint( 0, 0); // 不明
    }
}

/****************************************************************************
*   行動･方向変換
*
*   @return 行動･方向情報
*
*   @param str 行動･方向文字列
****************************************************************************/
Operation Operation::fromString(const QString& str){
    // クライアント行動情報
    Operation answer;

    // 文字列から行動と方向を取得
    if     (str[0] == 'w')answer.action = Operation::ACTION::WALK;     // 指定方向移動
    else if(str[0] == 'l')answer.action = Operation::ACTION::LOOK;     // 指定方向周囲確認
    else if(str[0] == 's')answer.action = Operation::ACTION::SEARCH;   // 指定方向直線確認
    else if(str[0] == 'p')answer.action = Operation::ACTION::PUT;      // 指定方向ブロック配置
    else                  answer.action = Operation::ACTION::UNKNOWN;  // 不明
    if     (str[1] == 'u')answer.rote   = Operation::ROTE::UP;         // 上
    else if(str[1] == 'd')answer.rote   = Operation::ROTE::DOWN;       // 下
    else if(str[1] == 'r')answer.rote   = Operation::ROTE::RIGHT;      // 右
    else if(str[1] == 'l')answer.rote   = Operation::ROTE::LEFT;       // 左
    else                  answer.rote   = Operation::ROTE::UNKNOWN;    // 不明
    // 戻り値[行動･方向情報]
    return answer;
}
