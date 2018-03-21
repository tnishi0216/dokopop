#!perl -S mktpl.pl dchooku.tpl
------------------------------------------------------------------------
 DCHOOK/Unicode Document Template
------------------------------------------------------------------------
%NEWMAJOR=2
%NEWMINOR=1
%NEWRELEASE=3
%POST=
%OLDMAJOR=2
%OLDMINOR=1
%OLDRELEASE=2
%OLDPOST=

%DATE_Y=18		公開日
%DATE_M=03
%DATE_D=21

%TIME_H=02
%TIME_M=01
%TIME_S=03

%DATE_YY=20%DATE_Y%
%VERSION = %NEWMAJOR%.%NEWMINOR%.%NEWRELEASE%%POST%
%VER=%NEWMAJOR%%NEWMINOR%%NEWRELEASE%
%OLDVERSION=%OLDMAJOR%.%OLDMINOR%.%OLDRELEASE%%OLDPOST%
%OLDVER=%OLDMAJOR%%OLDMINOR%%OLDRELEASE%
%DATE=%DATE_YY%.%DATE_M%.%DATE_D%

%INSTALLER=DKPU%VER%.exe

%HTML=S:\Web\NIFTY\
%SAKURA=S:\web\sakura-pdic\cgi-bin\download\files\

#以下はMKU.BAT用
%FDATE=%DATE_M%/%DATE_D%/%DATE_Y%
%FTIME=%TIME_H%:%TIME_M%:%TIME_S%
%PDNEW=%VER%
%PDOLD=%OLDVER%

%VARFILE=%HTML%pdic-unicode.var
%IND=dchooku.ind
%CHGINI=perl -S chgini.pl

%KEY1=DOKOPOP
%KEY2=PDIC
%KEY3=$WIN32
%KEY4=#APP
%KEY5=#EDU

****************** UPDATE 内容 **************************************
<*UPDATE.TXT
##EOF
******************** DokoPop! の変更履歴 ****************************
##<*HISTORY.TXT
##>UPDATE.TXT

>> Ver.2.1 <<
  ・二語前の単語を含めた連語のヒット対応（要：PDIC/Unicode Ver.5.10.24以上)

>> Ver.2.00 <<
  ・文字認識により、ポップアップ検索ができるところが増えた
  ・マウスの中央ボタンへの対応

>> Ver.1.23 <<
  ・DDEで送信するデータの形式が正しく指定されていなかった(CF_TEXT->CF_UNICODETEXT)

>> Ver.1.22 <<
  ・クリックする対象のウィンドウが非Unicodeである場合動作しなかった

>> Ver.1.21 <<
  ・辞書グループの選択が正しく動作しない問題を修正

>> Ver.1.20 <<
  ・PDIC/Unicode Ver.5.0.xに対応

>> Ver.1.15 <<
  ・PDIC/Unicode Ver.0.83に対応
  ・dialogの出る場所をDesktopCenter→ScreenCenter (マルチモニタ対応)

>> Ver.1.14 <<
  ・DokoPop!に関しては通常のバージョン番号を付けることにした
    - DokoPop!はβ版と正式版の実際的な違いが無いため
    - DokoPop!はオープンソースであり、PDIC本体とは別配布であるため、
      PDIC本体と同期するβ版と言う適当な区切りが無いため
  ・バージョン番号はfor Win32版と共通化（ソースコードが同じため）
  * 文の最後の単語がヒットしなかった（DokoPop!の最新版が必要）
    【原因】
  　取得した文字列の最後にゴミが付属したことにより、それをPDICが通常のテキスト
  　と認識してしまい、文の最後のピリオドが単語中の文字列として処理されたため。

>> Ver.0.10 <<
・最初の公開
##EOF
****************** 自己紹介 ******************************************
##<*PROB.TXT
##EOF

##<*INTRO.TXT
　DokoPop! - どこポップ！ は、Ctrl+マウス右クリックで、
　マウスカーソル位置の英単語・英熟語を検索し、ポップアップ表示します。

　どこポップ！ は PDIC/UnicodeのDDE機能を利用しています。
　別途PDIC/Unicodeを入手して下さい。

　PDIC/Unicode の入手先(必須)：
  http://pdic.la.coocan.jp/

　ソースファイル：
  http://dokopop.sourceforge.jp/
##EOF

##<DKPUu.TXT
========================================================================
【ソ フ ト名】  どこポップ！ Ver.%VERSION%
【登  録  名】　%INSTALLER%
【バイト  数】  <FILESIZE:%INSTALLER%> Bytes
【検索  キー】　1:%KEY1% 2:%KEY2% 3:%KEY3% 4:%KEY4% 5:%KEY5%
【著作権  者】　なし
【掲  載  者】　DokoPop Project
【対応  機種】　MS-Windows Vista/7/8/8.1/10
【動作  確認】　IBM PC/AT互換機,7/8/8.1/10
【掲  載  日】　%DATE%
【作成  方法】　%INSTALLER% を実行する
【ｿﾌﾄｳｪｱ種別】　フリーウェア
【転載  条件】  転載自由
========================================================================
【ソフト紹介】

##>INTRO.TXT

##>UPDATE.TXT

現在の問題点

##>PROB.TXT

========================================================================
ダウンロードファイル名は【%INSTALLER%】です。
##EOF

<DKPU.TXT
=============================================
      どこポップ！/Unicode    Ver.%VERSION%
=============================================
紹介 ---------------------------------------------------------

　「どこポップ！/Unicode」は、Personal Dictionary/Unicode版(以下PDIC)
　と連携して、マウスクリックによってマウス位置の単語を検索、日本語訳な
　どをポップアップウィンドウによって表示するソフトウェアです。
　Unicodeに対応しています。

動作確認OS ---------------------------------------------------

　Windows7/8/8.1/10

 ※Windows2000/XPで使用することも可能ですが、文字認識を利用したい場合、設定が必要です。（→文字認識の場所)

準備 ---------------------------------------------------------

　%INSTALLER%をそのまま実行して下さい。
　自動的にインストールされます。
　スタートアップに登録した方が便利です。(Windows起動時にDokoPop!も起動)

　どこポップ！にはPDIC/Unicode Ver.5.7.21以降 が必要です。
  http://pdic.la.coocan.jp/
  からダウンロードして下さい。

使用方法 -----------------------------------------------------

　DKPU.EXEを起動するとタスクトレイに常駐します。(DPと書いてあるアイコン)
　Ctrlキーを押しながらマウスの右ボタンをクリックすると、クリックした位置の
　単語または熟語の意味を表示します。

  【注意】PDIC本体は必ず辞書グループをオープンした状態にしておいてください。
          つまり、通常の検索(WORD欄入力）ができる状態にしておいてください。
          (メニューの File | 辞書グループオープン)
          辞書変換中など、通常の検索ができない状態ではポップアップ検索は
          できません。

  【注意】MS-Wordでは Ctrl+右クリック がカット＆ペーストに割り当てられている
          ため、うまく動作しないときがあります。
          （「はじめに選択してください」などと出てくる）
          もしこの問題を回避したい場合は、次の「ちょっと便利な使用方法」
          にあるように、「右クリックだけでポップアップ」にするか、
          次のように設定を行ってください。
          DokoPop!のタスクトレイのメニューから、[設定...]を選び、
          「ポップアップ検索」から、ポップアップをさせるときの組み合わせを
          選んでください。
          Alt + 右クリック であれば問題ないようです。

ちょっと便利な使用方法 ---------------------------------------

　「Ctrl + 右クリック」はわずらわしい！！
　という人には、

　「右クリックだけでポップアップ」

　という方法もあります。
　Ctrl + Alt + 右クリック でON/OFFの切り替えができます。
　（切り替え時にメッセージが出ます)

　ヒント：「右クリックだけ」にした場合、「Ctrl + 右クリック」をすると、
　　　　　通常の右クリックメニューが出てきます。
　　　　　ちょうど「Ctrl」が逆転する感じになります。

メニュー -----------------------------------------------------

　タスクトレイのDokoPop!アイコンを右クリックするとメニューが現れます。

　辞書グループ         - PDICの辞書グループの変更を行います
　右クリック検索       - マウスの右クリックだけでポップアップ検索を行います
　ポップアップ検索有効 - 検索を有効／無効にします
　設定                 - 各種設定を行います
　説明書を表示         - このファイルを表示します
　バージョン           - version表示
　終了                 - どこポップ！の終了

設定 ---------------------------------------------------------

  タスクトレイの右クリックで出てきたメニューの「設定」です。

  ■起動時にスプラッシュを表示する
    起動時の画面中央に出てくるウィンドウの
    表示を有効／無効にします。

    ★起動時のスプラッシュビットマップをカスタマイズ★
    DKPU.EXEがあるフォルダーに DKPU.BMP というファイルがあると、
    そのビットマップを起動時とバージョン...のときに表示します。

  ■辞書グループを指定する
    ここをチェックすると、ポップアップ検索を行うときに
    ここで指定した辞書グループを使用します。
    PDIC本体での辞書グループの選択が変更されます。

  ■ポップアップ検索 - ポップアップ検索を行うマウスとキーの組み合わせを指定します。

  ■右クリック検索ON/OFF
    右クリック検索の有効・無効を切り替えるマウスとキーの組み合わせです。

  ■英数字のみ検索する
    日本語・英数字混在の文章があった場合、日本語を無視して検索を行います。
    混在文章では ON にしたほうがヒットしやすくなります。
    また、フランス語、ドイツ語のようなウムラウト・アクサンなどを
    含む文章ではここを OFF にしないとうまく動作しない場合があります。

  ■認識モードの設定
    ・文字認識＋文字抽出
    ・文字認識のみ
    ・文字抽出のみ
    通常一番上の設定がいいと思いますが、ポップアップしない、アプリが落ちる、などの場合は
    設定を変更するとうまくいくようになるかもしれません。
    ※DokoPop! Ver.1は「文字抽出のみ」と同じ動作をします

  ■文字認識の場所 - 上級者向け
    MODIをインストールできない場合に、他のPCにあるAMODIを利用することでMODIを利用可能にします。
    場所として、
    %TEMP%\amodi
    を指定します。
    例：
    \\server\temp\amodi
    ※ファイル共有経由で利用します。

  ■64ビットフックを使用する
    64bitWindowsでうまくヒットしない場合にここをチェックすると動作するかもしれません。

言語判定DLL --------------------------------------------------

　DokoPop!/Unicodeでは、検索対象の単語を切り出す際に、対象言語が
　欧米系の言語 - ここでは単語の区切りに半角スペースを使う言語という意味で
　定義 - を前提として扱っています。
　しかし、欧米系以外の言語、例えば日本語のように単語の区切りは日本語を
　理解できる人にしかわかりません。当然、コンピューターにも単語の区切りを
　簡単に理解させることはできません。
　（日本語入力で文節の区切りをときどき間違えるように）
　さらに、Unicodeで扱える言語には日本語のような言語が数多くあり、
　各言語の文法を勉強し、組み込む必要があります。

　このための開発情報をこちらで公開しています。
　http://pdic.la.coocan.jp/unicode/langproc.html

その他 -------------------------------------------------------

　再利用・再配布・改変などは自由です。
　このソフトウェアによる損害は一切その責任を負いません。

　このprogramは、open sourceであるDCHookのsample programです。
　ソースファイルは、
　http://dokopop.sourceforge.jp/
　にあります。

　PDIC/Unicodeは
　http://pdic.la.coocan.jp/unicode/

履歴 ---------------------------------------------------------

##>HISTORY.TXT

--------------------------------------------------------------
##EOF

****************** パッケージ作成用.BATファイル *******************************
<MKU.BAT
@echo off
echo/
echo 完全版パッケージを作成します
echo/
echo ■確認事項
echo ・./dchooktest/dchooktest.exeを準備(USE_UNICODE defined)
echo ・バージョン番号：.bpr、Util.cpp(StrVersion, VersionValue)と.tplが必要
echo ・Release Build?
echo ・VisualStudio2008でbuildしたか？ for W2K
pause
set TZ=JST-9

echo/
echo Copy the binary to the ind directory.
echo/
pause

make copy_bin

rem touch -d%FDATE% -t%FTIME% -c ind\DKPU.EXE ind\DKPUHK.DLL

rem dkpu.issの更新
perl change_iss.pl ind\dkpu.iss Setup AppVerName "DokoPop! %VERSION%" > ind\dkpu.iss.new
del ind\dkpu.iss.old
ren ind\dkpu.iss dkpu.iss.old
ren ind\dkpu.iss.new dkpu.iss

rem Installer の実行
make buildis

echo/
echo Press Enter key after installer made.
echo/
pause

copy ind\Output\setup.EXE %INSTALLER%
dir %INSTALLER%

rem lha32 a %INSTALLER% DKPU.TXT DKPU.EXE DKPUHK.DLL
ren %INSTALLER% %INSTALLER%
rem touch -d%FDATE% -t%FTIME% -c %INSTALLER%

echo/
echo ソースファイルをzipします
echo/
pause

make zip_src
copy src.zip src-%VERSION%.zip
rem copy src-%VERSION%.zip zip

echo/
echo 公開用ファイルを更新します
echo/
rem pause

perl -S -i.bak filestamp.pl DKPUu.TXT
del DKPUu.TXT.bak

echo/
echo Web用のファイルを更新します
echo/
pause

rem copy %INSTALLER% %HTML%
rem del %HTML%DKPU%OLDVER%.EXE
copy %INSTALLER% %SAKURA%
del %SAKURA%DKPU%OLDVER%.EXE

rem echo [del] >> %HTML%ftp.ctl
rem echo DKPU%OLDVER%.EXE >> %HTML%ftp.ctl
rem echo [include] >> %HTML%ftp.ctl
rem echo %INSTALLER% >> %HTML%ftp.ctl

##EOF

<<@%VARFILE%,DU_

