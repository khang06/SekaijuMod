/******************************************************************************/
/*                                                                            */
/*  MIDIInstrumentライブラリ「はじめにお読みください」     (C)2002-2014 くず  */
/*                                                                            */
/******************************************************************************/

　このたびは、MIDIInstrumentライブラリをダウンロードしていただき、又はお受け取り
になっていただき、誠にありがとうございます。

　MIDIInstrumentライブラリは、フリーでオープンソースの、MIDIインストゥルメント定
義の製作・編集用ライブラリです。このライブラリは、インストゥルメント定義の生成・
ファイル入力(Cakewalk用インストゥルメント定義ファイル(*.ins))、音色名の追加・変更・
参照・削除、コントローラー名の追加・変更・参照・削除など、MIDIインストゥルメント
定義のあらゆる要素に迅速にアクセスする機能を提供します。

【添付ファイル】

MIDIInstrument
├readme.txt                はじめにお読みください(このファイル)
├license.txt               ライセンス(原文・英語)
├MIDIInstrument.c          Cソースファイル
├MIDIInstrument.h          C/C++用ヘッダーファイル
├MIDIInstrument.def        C/C++用モジュール定義ファイル(dllを作るときに必要)
├MIDIInstrument.mak        C/C++用メイクファイル
├MIDIInstrument.sln        Visual C++ 2008 Service Pack 1用ソリューションファイル
├MIDIInstrument.vcproj     Visual C++ 2008 Service Pack 1用プロジェクトファイル
├MIDIInstrument.bas        Visual Basic 4.0/5.0/6.0用インポートモジュール
├Debug
│├MIDIInstrumentd.lib     オブジェクトライブラリモジュール(デバッグ用)
│└MIDIInstrumentd.dll     ダイナミックリンクライブラリ(デバッグ用)
├Release
│├MIDIInstrument.lib      オブジェクトライブラリモジュール(リリース用)
│└MIDIInstrument.dll      ダイナミックリンクライブラリ(リリース用)
└docs
　├MIDIInstrument.html     公式ガイドブック
　└MIDIDR02.gif            公式ガイドブックで使われている図

【使用方法】

・すべてのファイルを解凍してください。
・MIDIInstrument.hをVisualC++をインストールしたフォルダ内のincludeフォルダ内にコピーしてください。
・MIDIInstrument.libをVisualC++をインストールしたフォルダ内のlibフォルダ内にコピーしてください。
・MIDIInstrument.dllファイルをc:\windows\System32\内(32bitOSの場合)、
　c:\windows\SysWOW64\内(64bitOSの場合)にコピーしてください。
・MIDIInstrumentライブラリを使用するソースでは、#include "MIDIInstrument.h"を行ってください。
・MIDIInstrumentライブラリを使用するプロジェクトでは、VisualC++をご使用の場合、
・「ビルド(B)」-「(プロジェクト名)のプロパティ」から「(プロジェクト名)のプロパティページ」という
　ダイアログを表示させ、その中の「構成とプロパティ」の「リンカ」の「入力」の中に、
　「追加の依存ファイル」という項目があるので、そこにMIDIInstrument.libを追加してください。
・アプリケーションを配布する際には、MIDIInstrument.dllをexeファイルと同じフォルダに添付してください。
・詳しくは、おーぷんMIDIぷろじぇくとの資料又はFAQのページを参照してください。

【ライセンス】

　このライブラリは、GNU 劣等一般公衆利用許諾契約書(LGPL)に基づいて配布されます。
・あなたはこのライブラリを、LGPLに基づき、複製・転載・配布することができます。
・あなたはこのライブラリを改変することができ、それをLGPLに基づき配布することができます。
・あなたはこのライブラリのDLLを利用して(ダイナミックリンクに限る)、あなた独自の
　ライセンスのアプリケーションを製作・配布することができます。
・いずれの場合も作者に許可を得る必要はありません。
・このライブラリは全くの無保証です。
　このライブラリを使用した結果生じた損害につきまして作者は一切責任を負いません。
　あらかじめご了承ください。
※詳しくは、LGPLの日本語訳(http://www.opensource.gr.jp/lesser/lgpl.ja.html)をご参照ください。

【連絡先】

・メールアドレス(仮) ee65051@yahoo.co.jp
・プロジェクトホームページ http://openmidiproject.sourceforge.jp/index.html

