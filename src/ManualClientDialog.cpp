/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   手動クライアント画面
****************************************************************************/
#include "ManualClientDialog.h"         // 手動クライアント画面
#include "ui_ManualClientDialog.h"      // 手動クライアント画面UI

/****************************************************************************
*   クローズ
*
*   @param ce クローズ情報
****************************************************************************/
void ManualClientDialog::closeEvent(QCloseEvent* ce){
    // ウィンドウクローズ
    emit CloseWindow();
    // クローズ許可
    ce->accept();
}

/****************************************************************************
*   キー押下
*
*   @param event キー押下情報
****************************************************************************/
void ManualClientDialog::keyPressEvent(QKeyEvent * event){
    // [W]キーならば[↑]クリック
    if(event->key()==Qt::Key_W)UPButtonClicked();
    // [S]キーならば[↓]クリック
    if(event->key()==Qt::Key_S)DOWNButtonClicked();
    // [D]キーならば[→]クリック
    if(event->key()==Qt::Key_D)RIGHTButtonClicked();
    // [A]キーならば[←]クリック
    if(event->key()==Qt::Key_A)LEFTButtonClicked();
}

/****************************************************************************
*   行動定数取得
****************************************************************************/
GameSystem::Method::ACTION ManualClientDialog::GetAction(){
    // [WalkAction]指定方向移動
    if     (this->ui->WalkRadio->isChecked())  return GameSystem::Method::ACTION::WALK;
    // [PutAction]指定方向ブロック配置
    else if(this->ui->PutRadio->isChecked())   return GameSystem::Method::ACTION::PUT;
    // [SearchAction]指定方向直線確認
    else if(this->ui->SearchRadio->isChecked())return GameSystem::Method::ACTION::SEARCH;
    // [LookAction]指定方向周囲確認
    else if(this->ui->LookRadio->isChecked())  return GameSystem::Method::ACTION::LOOK;
    // 不明
    else                                       return GameSystem::Method::ACTION::UNKNOWN;
}

/****************************************************************************
*   [↑]ボタン押下
****************************************************************************/
void ManualClientDialog::UPButtonClicked()
{
    // 方向=上
    next_method.rote = GameSystem::Method::ROTE::UP;
    // 行動定数取得
    next_method.action = GetAction();
    // 行動決定
    emit ReadyAction();
}

/****************************************************************************
*   [↓]ボタン押下
****************************************************************************/
void ManualClientDialog::DOWNButtonClicked()
{
    // 方向=下
    next_method.rote = GameSystem::Method::ROTE::DOWN;
    // 行動定数取得
    next_method.action = GetAction();
    // 行動決定
    emit ReadyAction();
}

/****************************************************************************
*   [→]ボタン押下
****************************************************************************/
void ManualClientDialog::RIGHTButtonClicked()
{
    // 方向=右
    next_method.rote = GameSystem::Method::ROTE::RIGHT;
    // 行動定数取得
    next_method.action = GetAction();
    // 行動決定
    emit ReadyAction();
}

/****************************************************************************
*   [←]ボタン押下
****************************************************************************/
void ManualClientDialog::LEFTButtonClicked()
{
    // 方向=左
    next_method.rote = GameSystem::Method::ROTE::LEFT;
    // 行動定数取得
    next_method.action = GetAction();
    // 行動決定
    emit ReadyAction();
}

/****************************************************************************
*   ログ追加
*
*   @param str ログ文字列
****************************************************************************/
void ManualClientDialog::AppendLog(const QString &str)
{
    // ログ追加
    this->ui->LogEdit->appendPlainText(str);
}

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
ManualClientDialog::ManualClientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManualClientDialog)
{
    // UI初期化
    ui->setupUi(this);
    // [↑]ボタンシグナル･スロット設定
    connect(this->ui->UPButton,    &QPushButton::clicked, this, &ManualClientDialog::UPButtonClicked);
    // [↓]ボタンシグナル･スロット設定
    connect(this->ui->DOWNButton,  &QPushButton::clicked, this, &ManualClientDialog::DOWNButtonClicked);
    // [→]ボタンシグナル･スロット設定
    connect(this->ui->RIGHTButton, &QPushButton::clicked, this, &ManualClientDialog::RIGHTButtonClicked);
    // [←]ボタンシグナル･スロット設定
    connect(this->ui->LEFTButton,  &QPushButton::clicked, this, &ManualClientDialog::LEFTButtonClicked);
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
ManualClientDialog::~ManualClientDialog()
{
    // ウィンドウ非表示
    hide();
    // UI破棄
    delete ui;
}
