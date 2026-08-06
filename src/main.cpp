/****************************************************************************
*   SYSTEM  :   CHaserサーバー
*----------------------------------------------------------------------------
*   NOTE    :   メイン
****************************************************************************/
#include "mainwindow.h"                 // メイン画面
#include <QApplication>                 // GUIアプリケーション
#include <QTranslator>                  // 国際化サポート
#include <QLibraryInfo>                 // Qtライブラリ

/****************************************************************************
*   メイン
*
*   @return 処理結果
*
*   @param argc 引数の数
*   @param argv 引数設定
****************************************************************************/
int main(int argc, char *argv[])
{
    // アプリケーションオブジェクト
    QApplication a(argc, argv);
    // ダークテーマに変わらないように設定
    a.setStyle("windowsvista");

    // 国際化サポート
    QTranslator qtTranslator;

    // 翻訳ファイル読込
    (void) qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    // 翻訳インストール
    a.installTranslator(&qtTranslator);

    // メイン画面表示
    MainWindow w;
    w.show();

    // アプリケーション実行
    return a.exec();
}
