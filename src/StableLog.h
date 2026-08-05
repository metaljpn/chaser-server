/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ログ管理
****************************************************************************/
#ifndef STABLELOG_H
#define STABLELOG_H
#include <QString>                      // 文字列
#include <QFile>                        // ファイルアクセス
#include <QDir>                         // ディレクトリ操作
#include <QIODevice>                    // I/Oデバイス
#include <QTextStream>                  // テキストアクセス
#include <QMutex>                       // 排他制御

class StableLog
{
public:
    // ファイル名取得
    QString filename() { return m_filename; }
    // 書込
    void Write(const QString& str)const;


    // 書込オペレータ
    const StableLog& operator<<(const QString& str)const;

    StableLog();
    StableLog(QString filename);
    StableLog(const StableLog& other);
    StableLog& operator=(const StableLog& other);
    ~StableLog();

private:
    QString m_filename;                 // ファイル名

    mutable QMutex* m_mutex;            // 排他用オブジェクト
};

#endif // STABLELOG_H
