/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ゲーム管理
****************************************************************************/
#include "GameSystem.h"                 // ゲーム管理

/****************************************************************************
*   クライアント行動-方向別座標取得
*
*   @return 座標
****************************************************************************/
QPoint GameSystem::Method::GetRoteVector(){
    // 方向ベクトル生成
    switch(this->rote){
        case GameSystem::Method::ROTE::UP:    return QPoint( 0,-1); // 上
        case GameSystem::Method::ROTE::RIGHT: return QPoint( 1, 0); // 右
        case GameSystem::Method::ROTE::DOWN:  return QPoint( 0, 1); // 下
        case GameSystem::Method::ROTE::LEFT:  return QPoint(-1, 0); // 左
        default:                              return QPoint( 0, 0); // 不明
    }
}

/****************************************************************************
*   クライアント行動-行動･方向変換
*
*   @return 行動･方向情報
*
*   @param str 行動･方向文字列
****************************************************************************/
GameSystem::Method GameSystem::Method::fromString(const QString& str){
    // クライアント行動情報
    GameSystem::Method answer;

    // 文字列から行動と方向を取得
    if     (str[0] == 'w')answer.action = GameSystem::Method::ACTION::WALK;     // 指定方向移動
    else if(str[0] == 'l')answer.action = GameSystem::Method::ACTION::LOOK;     // 指定方向周囲確認
    else if(str[0] == 's')answer.action = GameSystem::Method::ACTION::SEARCH;   // 指定方向直線確認
    else if(str[0] == 'p')answer.action = GameSystem::Method::ACTION::PUT;      // 指定方向ブロック配置
    else                  answer.action = GameSystem::Method::ACTION::UNKNOWN;  // 不明
    if     (str[1] == 'u')answer.rote   = GameSystem::Method::ROTE::UP;         // 上
    else if(str[1] == 'd')answer.rote   = GameSystem::Method::ROTE::DOWN;       // 下
    else if(str[1] == 'r')answer.rote   = GameSystem::Method::ROTE::RIGHT;      // 右
    else if(str[1] == 'l')answer.rote   = GameSystem::Method::ROTE::LEFT;       // 左
    else                  answer.rote   = GameSystem::Method::ROTE::UNKNOWN;    // 不明
    // 戻り値[行動･方向情報]
    return answer;
}

/****************************************************************************
*   周辺情報-文字列変換
*
*   @return 文字列
****************************************************************************/
QString GameSystem::AroundData::toString(){
    // 文字列
    QString str;

    // 接続状態
    str.append(static_cast<QChar>('0' + static_cast<int>(this->connect)));
    // 周辺情報数分
    for(int i=0;i<9;i++){
        if     (this->data[i] == GameSystem::MAP_OBJECT::NOTHING)str.append('0');   // 無し
        else if(this->data[i] == GameSystem::MAP_OBJECT::TARGET) str.append('1');   // 相手
        else if(this->data[i] == GameSystem::MAP_OBJECT::BLOCK)  str.append('2');   // ブロック
        else if(this->data[i] == GameSystem::MAP_OBJECT::ITEM)   str.append('3');   // アイテム
    }
    // 戻り値[文字列]
    return str;
}

/****************************************************************************
*   周辺情報-ゲーム終了
****************************************************************************/
void GameSystem::AroundData::Finish(){
    // 周辺情報先頭を接続終了
    this->data[0] = static_cast<GameSystem::MAP_OBJECT>(GameSystem::CONNECTING_STATUS::FINISHED);
    // 接続終了
    this->connect = GameSystem::CONNECTING_STATUS::FINISHED;
}
