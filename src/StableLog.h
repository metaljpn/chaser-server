/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   ログ管理
****************************************************************************/
#ifndef STABLELOG_H
#define STABLELOG_H

#include <QMutex>                       // 排他制御
#include <QString>                      // 文字列

class StableLog
{
private:
    QString m_filename;                 // ファイル名

    mutable QMutex* m_mutex;            // 排他用オブジェクト

public:
    // ファイル名取得
    QString filename() { return m_filename; }
    // 追記書込
    void Write(const QString& str)const;
    // 書込オペレータ
    const StableLog& operator<<(const QString& str)const;
    // 更新オペレータ
    StableLog& operator=(const StableLog& other);

    StableLog();
    StableLog(QString filename);
    StableLog(const StableLog& other);
    ~StableLog();
};

#endif // STABLELOG_H
