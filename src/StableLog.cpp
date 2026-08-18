/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ログ管理
****************************************************************************/
#include "StableLog.h"                  // ログ管理

#include <QDebug>                       // デバッグ情報
#include <QFile>                        // ファイルアクセス
#include <QTextStream>                  // テキストアクセス

/****************************************************************************
*   コンストラクタ
*
*   @param parent 親ウィジェット
****************************************************************************/
StableLog::StableLog()
    : m_filename(),
    m_mutex(new QMutex())
{

}

/****************************************************************************
*   コンストラクタ
*
*   @param filename ファイル名
****************************************************************************/
StableLog::StableLog(QString filename)
    : m_filename(filename),
    m_mutex(new QMutex())
{
    // 排他制御(スレッドセーフ)にするためにロックを取得
    QMutexLocker locker(m_mutex);
    // ログファイル
    QFile file(m_filename);
    // ログストリーム
    QTextStream log;

    // ログファイル書込オープン
    if(!file.open(QIODevice::WriteOnly)){
        // 異常通知
        qDebug() << "ログファイルのオープンに失敗しました : ";
        qDebug() << file.errorString();
        // 関数終了
        return;
    }

    // ログデバイス設定
    log.setDevice(&file);
    // ログ見出し出力
    log << "--Stable Log--\r\n";
    // ログファイルクローズ
    file.close();
}

/****************************************************************************
*   コンストラクタ
*
*   @param other ログ情報
****************************************************************************/
StableLog::StableLog(const StableLog& other)
    : m_filename(other.m_filename),
    m_mutex(new QMutex())
{

}

/****************************************************************************
*   追記書込
*
*   @param str ログ文字列
****************************************************************************/
void StableLog::Write(const QString &str) const
{
    // 排他制御(スレッドセーフ)にするためにロックを取得
    QMutexLocker locker(m_mutex);
    // ログファイル
    QFile file(m_filename);

    // 追記書込オープン
    if(!file.open(QIODevice::Append | QIODevice::Unbuffered | QIODevice::Text)){
        // 異常通知
        qDebug() << "ログファイルのオープンに失敗しました : ";
        qDebug() << file.errorString();
        // 関数終了
        return;
    }

    // 書込
    file.write(str.toUtf8());
    // クローズ
    file.close();
}

/****************************************************************************
*   書込オペレータ
*
*   @return 対象オブジェクト
*
*   @param str ログ文字列
****************************************************************************/
const StableLog &StableLog::operator<<(const QString &str) const
{
    // 書込
    Write(str);
    // 戻り値[対象オブジェクト]
    return (*this);
}

/****************************************************************************
*   更新オペレータ
*
*   @return 対象オブジェクト
*
*   @param other ログ情報
****************************************************************************/
StableLog& StableLog::operator=(const StableLog& other)
{
    // 重複書込ならば関数終了
    if (this == &other) return *this;

    // 新しい排他用オブジェクトを確保してから代入・破棄（例外安全を簡易に確保）
    QMutex* newMutex = new QMutex();

    // ファイル名をコピー
    m_filename = other.m_filename;
    // 排他用オブジェクト破棄
    delete m_mutex;
    // 排他用オブジェクト設定
    m_mutex = newMutex;
    // 戻り値[対象オブジェクト]
    return *this;
}

/****************************************************************************
*   デストラクタ
****************************************************************************/
StableLog::~StableLog()
{
    // 排他用オブジェクト破棄
    delete m_mutex;
}
