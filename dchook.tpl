#!perl -S mktpl.pl dchook.tpl
------------------------------------------------------------------------
 DCHOOK Document Template
------------------------------------------------------------------------
%NEWMAJOR=1
%NEWMINOR=11
%POST=
%OLDMAJOR=1
%OLDMINOR=10
%OLDPOST=

%DATE_Y=02		公開日
%DATE_M=12
%DATE_D=09

%TIME_H=01
%TIME_M=11
%TIME_S=00

%DATE_YY=20%DATE_Y%
%VERSION = %NEWMAJOR%.%NEWMINOR%%POST%
%VER=%NEWMAJOR%%NEWMINOR%
%OLDVERSION=%OLDMAJOR%.%OLDMINOR%%OLDPOST%
%OLDVER=%OLDMAJOR%%OLDMINOR%
%DATE=%DATE_YY%.%DATE_M%.%DATE_D%

%ARC=dchook-%VERSION%.lzh
%DKPPARC=DKPP%VER%.EXE

%HTML=S:\Web\NIFTY\
%MYSOFT=S:\down\MySoft

#以下はMK.BAT用
%FDATE=%DATE_M%/%DATE_D%/%DATE_Y%
%FTIME=%TIME_H%:%TIME_M%:%TIME_S%
%PDNEW=%VER%
%PDOLD=%OLDVER%

%VARFILE=%HTML%pdic-download.var
%IND=dchook.ind
%CHGINI=perl -S chgini.pl

%KEY1=DOKOPOP
%KEY2=PDIC
%KEY3=$WIN32
%KEY4=#APP
%KEY5=#EDU

****************** UPDATE 内容 **************************************
<*UPDATE.TXT
>> Ver.1.12 <<
* 辞書が大きい場合に、PDIC起動後の最初のポップアップができないときがあった
##EOF
******************** DokoPop! の変更履歴 ****************************
##<*HISTORY.TXT
##>UPDATE.TXT

>> Ver.1.11 <<
・Unicode版対応のために内部処理を変更

>> Ver.1.10 <<
○DokoPopでポップアップしたウィンドウ上でさらにポップアップする
  ことが可能に（連続ポップアップ）（PDIC for Win32はVer.4.50以上が必要）
○日本語混在の文章でも正しく動作するようにした。
　（設定ダイアログの”日本語を無視する”で切り替え可能）

>> Ver.1.02 <<
* DKPP.EXEのバージョン設定を間違っていた

>> Ver.1.01 <<
* Windows9xでポップアップしない場合があった(IEなど)
・それに対処したため、Windows95は動作対象外に
* 右クリック検索切り替えでリソースリークが発生していた
* ヒットしない単語があるとメモリリークが発生していた

>> Ver.1.00 <<
* Windows9x/Me系のOSでの動作を安定化した
* 辞書グループ指定を正常に動作するようにした
  【注意】この機能はPDIC for Win32 Ver.4.40からの対応になります
・検索のキーの組み合わせをカスタマイズできるようにした
・ファイル構成を変更
・連続してポップアップ検索ができるようにした
・ほか表示の変更など

>> Ver.0.98 <<
○マウス右クリックのみでポップアップを可能に
　（Ctrl+Alt+右クリックでON/OFFできます）
* ”辞書グループが開けません”と出ることがあった
* 最小化で起動した場合、タスクバーにアイコンが残っていた

>> Ver.0.97 <<
* 辞書グループの変更ができなくなっていた
* IEでヒットしないことがあった

>> Ver.0.96 <<
* PDICを自動起動できなかった(読み込むregistryを間違っていた)
* どこポップを最小化で起動した場合、スプラッシュウィンドウが消えなかった

>> Ver.0.95 <<
??

>> Ver.0.94 <<
* スプラッシュを非表示にした場合の不具合修正

>> Ver.0.94 <<
* 起動時DLL初期化失敗(Win98)
* 日本語混じりの文でテキスト抽出位置がずれていた(Win98)

>> Ver.0.93 <<
・起動スプラッシュ変更
・スプラッシュ表示・非表示
・スプラッシュビットマップのカスタマイズ
・インストーラ追加
* メモ帳で異常終了(NT)
* 日本語混じりの文でテキスト抽出位置がずれていた

>> Ver.0.92 <<
・安定性向上

>> Ver.0.90 <<
・最初の公開
##EOF
****************** 自己紹介 ******************************************
##<*PROB.TXT
・Acrobat Reader上では動作しない
・Opera上では動作しない
・DOS窓上では動作しない
・16bitアプリ上では動作しない
・複数行にまたがる熟語はヒットしない
##EOF

##<*INTRO.TXT
　DokoPop! - どこポップ！ は、Ctrl+マウス右クリックで、
　マウスカーソル位置の英単語・英熟語を検索し、ポップアップ表示します。

　どこポップ！ は PDIC for Win32のDDE機能を利用しています。
　別途PDIC for Win32を入手して下さい。

　PDIC for Win32の入手先(必須)：
  http://homepage3.nifty.com/TaN/pdic-download.html

　ソースファイル：
  http://homepage3.nifty.com/TaN/prog-tips.html  %ARC%
##EOF

##<DKPPu.TXT
========================================================================
【ソ フ ト名】  どこポップ！ Ver.%VERSION%
【登  録  名】　%DKPPARC%
【バイト  数】  <FILESIZE:%DKPPARC%> Bytes
【検索  キー】　1:%KEY1% 2:%KEY2% 3:%KEY3% 4:%KEY4% 5:%KEY5%
【著作権  者】　なし
【掲  載  者】　TaN(SGM00353)
【対応  機種】　MS-Windows 98/Me/NT4.0/2000/XP
【動作  確認】　IBM PC/AT互換機,NT4,0,Win98,2000
【掲  載  日】　%DATE%
【作成  方法】　%DKPPARC% を実行する
【ｿﾌﾄｳｪｱ種別】　フリーウェア
【転載  条件】  転載自由
========================================================================
【ソフト紹介】

##>INTRO.TXT

##>UPDATE.TXT

現在の問題点

##>PROB.TXT

========================================================================
ダウンロードファイル名は【%DKPPARC%】です。
##EOF

<README.TXT
==========================================================================
                        DCHook sample program
==========================================================================
Introduction -------------------------------------------------------------

DCHookは、Windowsにおいて任意のWindowの任意の座標位置から
そこに描画されているテキストを拾ってくるプログラムです。
ちょっとやってみたら簡単に動いたので、公開することにしました。

このプログラムの応用としては、
・任意のウィンドウのテキストを拾う
・全ウィンドウのテキストをgrepする
・カーソル位置のテキストを拾う
といった一風変わったことができます。
このプログラムには参考として、DokoPopというサンプルプログラムを
添付してあります。

install ------------------------------------------------------------------

圧縮ファイルを適当なところに解凍してください。
ディレクトリ付きで解凍する必要があります。

添付ファイル -------------------------------------------------------------

targetになるもの：

dchook.dll (NT/2K版 or 9x版)
hk95d.vxd  (9x専用)
dchooktest.exe (NT/2Kと9x8共用)

必要な開発環境 -----------------------------------------------------------
C++ Builder 5
Visual C++ 6.0
Windows Device Driver Kit (DDK) for Windows 98

作成方法 -----------------------------------------------------------------

--------------------------------
dchook.dll
--------------------------------
必要な環境：
Visual C++

作成方法：
nmake -fdchook.mak

--------------------------------
hk95d.vxd
--------------------------------
必要な環境
Visual C++, DDK for Win98

変更箇所：
makefile
INC32をDDKのroot pathに(e:\98ddkなど)

作成方法：
nmake
※この実行前にVisual-C++およびDDKの環境が設定されている必要があります。
　バッチファイルで環境設定するようになっている場合は、
　作成前にバッチファイルを実行してください。
　（vcvars32.bat、setenv.bat e:\98ddk といった感じで
　→詳しくはVisual-C++、DDKに付属の説明書を参照）

--------------------------------
dchooktest.exe
--------------------------------
必要な環境：
C++ Builder 5

変更箇所：
特になし
※DEBUGをdefineすると、DEBUG用の動作になります。

作成方法：
dchooktest.bprをC++Builderで開いてください。
C++Builder4でもできるかも、と思って C++Builder4以前用に
dchooktest.makも添付してあります。多少の変更で使えるかもしれません。

あとは、メニューから Project | 再構築 で作成します。

実行：
dchooktest.exeを実行します。

★注意点：
DCHookTest.exeがLoadするDLLは、
DKPPHK.DLL -> DCHOOK.DLL
の順番で先に見つかった方をloadする仕様になっています。
DEBUGをdefineしたときは、DCHOOK.DLLのみをloadしようとします。

プログラム説明 -----------------------------------------------------------

一番厄介なものは、dchook.cppでしょう。
試行錯誤しながらも短時間で一気に書き上げたものですので、
かなり汚くなっています。
#defineによるswitchには、色んな組み合わせがありますが、
現状以外の設定にすると正常に動作しなかったり、
compileさえもできないはずです。だったら、整理しなさい！
と言われそうですが、今後必要となるところがあるかもしれないので、
そのままにしてあります。

動作原理としては、テキストを取得したいwindowに対して
再描画命令を送り、再描画中のテキスト描画関数の内容を
横取りすることで拾ってきます。

最初作り始めた頃は、WindowsからDCを渡している関数をhookし、
DCをmetafileで再生すれば簡単じゃないか？と思ってやってみました。
そうしたらあっけなく動いたのですが、別のapplicationでやってみると
うまく動かないことがありました。
原因は、CreateCompatibleBitmapでbitmapを作成し、そこへ一旦描画
してから、BitBltでwindowに貼り付けるapplicationがあったからです。
そうなると、DCだけではテキスト描画情報が得られませんので、
必要なtext描画関数をhookしてやる必要があります。
(ほかにもBeginPaint()で渡されるDCではなく、GetDC()で
得たDCに対して描画しているapplicatonに対してもうまくいきませんでした)

この前にこのhookの方法ですが、NTと9xでは異なります。
NTは各applicationが独立のmemory空間であり、user32.dll,gdi32.dllも
application毎のmemory空間に存在しているのですが、9xでは、
いきなり共有memoryへ行ってしまっているので、簡単にはhookできません。

APIのhook方法は、アスキーから出版されている「Advanced Windows」
を参考にしました。
http://www.amazon.co.jp/exec/obidos/ASIN/4756138055/pdichomepage-22
ここにAPIフックの説明が詳細に記述されています。
しかし、この方法を知るまでは Hook先のアドレスを直接書き換えるという
危険な方法をやっていました。つまり Advanced Windowsに記述されている
１番目の方法ですね。NT系のOSならほぼ問題なく動作するのですが、
9x系は例外がどうしても発生してしまいます。
ということで、この本の２番目の方法で安定して動作するように
なりました。しかし、この本のままではDokoPop!で効率よく
動作してくれないので、かなり書き換えてあります。
（ということで著作権表示も省略したりして・・・）
書き換えた、というより、今まで自分が書いたAPIHOOK構造体を
変更した、というほうが正しいかもしれません。
APIフックの解説はこの本に譲ります。

ですが、せっかく１番目の危険な方法でいくつか得た情報があるので、
旧バージョンで使用していたフック方法の情報を書いておきます。
---------- 古い話 ----------
ちなみに、NTではUSER32.DLL,GDI32.DLLのGetProcAddress()は
applicationによって異なります。しかし、9xでは同じです。
先ほどもちらっと書いたように、NTのapplicationは個別のmemory空間で
USER32.DLL,GDI32.DLLも異なる空間に存在しているからです。
ところが9xは共有していて、しかもjumpしたらすぐにkernel modeに
入っているようです。(NTはある程度user modeでやっている感じ）

さらにその先はどうなっているかというと・・・という話は
このプログラムのscope外なのでまたの機会に譲るとして、
ここで9xに対してちょっと嫌な予感がよぎります。
NTの場合、entry pointを書き換えてもそのapplication固有の
entry pointであるため、その間にtask switchingが発生しても
問題ないのですが、9xでは、entry pointを書き換えた直後に
task switchingが発生したら、(来て欲しくない)ほかのapplicationが
そのhookを伝ってやってくる・・・最悪entry point書き換え中に
switchingが発生すると・・・ちょっと怖いです。
でも、実際はそのようなケースは起こりにくく、実際そういう
問題は発生していません。ですが、一応プログラムでは
そのための対策をしてあります。
(このへんの理屈も書いた方がいいのですが・・・省略）

entry pointの書き換えですが、NTの場合は
通常のapplicationと同じですので、WriteProcessMemory関数で
code領域を書き込みができます。しかし、9xは
共有memoryであり、そこはwrite protectされています。
このprotectionを解除するには通常のWindows APIには
無いようなので（あったら教えて欲しい）、わざわざ
memoryを書き換えるだけのためのVxDを作りました。
VxDならkernel modeなのでなんでもできてしまいます。
(このへんが先ほどのtask switchingの話と絡んでいますが)

VxDはdynamic loadingで作ってあります。
このVxDで注意しなければならないのは、CreateFile()関数で
VxDをloadしていますが、二度CreateFile()はできないところです。
従って、一番最初の初期化のときだけ、CreateFile()を行い、
あとはずっとVxDを常駐させたままにしておく必要があります。

一応API Hookの話はこのくらいにしておきます。
API Hookをするapplicationはほかにもありますが、
そういうapplicationはこれと同じ方法を使用しているのでしょうか？
MSDNにもapimon(だったかな？)というtoolが付属していますが、
どうなんでしょう？
---------- 以上、古い話 ----------

話はずっと前に戻って、BitBlt対策の話です。
このBitBltで描画する方法を調べてみると、window全体ではなく、
bandingによって、windowを複数のbitmapに分割してBitBltしている
ようです。従って、BitBlt APIもhookしてやり、bitmapへtextを
描画したときの座標と、BitBltしたときのdestination pointの
offsetを考慮してtext座標を計算してやる必要があります。

以上、textをgetできたら、指定座標(sampleではmouseのcursor位置)に
textがあるかどうか、という判定を行います。
hitした場合は、そのtext全体と、text上のbyte offsetを
WM_COPYDATA messageで初期化の時に登録してある windowへSendMessage()
します。(WM_COPYDATAはSendMessageでないといけない)

そして、DCHookTest.exeがそのmessageを受け取り、DCHookTest.exeが
textをほかのapplication(PDIC)へDDEで渡しています。

DCHookTest.exeがやっていることはそれがほとんどで、
dchook.dllにくらべれば遙かに楽な仕事なのです。

DLLでは常識ですが、dataは基本的にprocess固有の値を持ちます。
ここで注意しなければならないのが、WindowsHookのhook dllは、
WindowsHookでセットした後、foreground applicationに
切り替わったときに初めてattachされます。
(つまり、WindowsHookした時点ではHookしたprocessにしか
attachされていない)
しかも、attachしたときには、別のapplicationのprocess空間に
居ますので、WM_COPYDATAの送り先の情報がどこにもありません。
ファイル/registryに保存して読み込む、なんていう方法もあり
ますが、dchook.defでsharedによって、dll間で共有する
data segmentを作成する方法があります。
ここにWM_COPYDATA送り先のhandleを保存しておけば、このような
問題は解決できます。

それともう一つやってみてわかったのですが、9xでは
Unhookをすると、すぐにすべてdetachされるのですが、
NTでは、これまたforegroundに切り替わったときにしか
detachしてくれません。そこで、dchookでは強制的に
detachさせるためにmessageをbroadcastさせています。
実際はそこまでやる必要性はないのですが、開発時に
memoryを解放させるために色んなapplicationに切り替えるのが
嫌だったので・・・BroadcastMessage以外に良い方法があるのかな？

プログラム解説 -----------------------------------------------------------

DBW()

デバッグ用の関数です。拙作のdbgmsg.exeへ文字列を出力しますが、
ほかのdebug stringを補足するdebug string monitorで使えるように
DBW()を書き換えたほうが良いでしょう。(OutputDebugString()など)

popenable.h, popdisable.h
これらは「右クリック検索を有効にしました」というメッセージウィンドウの
リージョン情報です。（丸みのあるウィンドウ）
このリージョン情報は、bmp2rgnというツールでpopenable.bmp, popdisable.bmp
を変換したものです。bmp2rgnはフリーウェアです。

サンプルプログラム -------------------------------------------------------

sample programとして、mouse cursor位置のtextを抽出し、
拙者作の英和辞書検索プログラム Personal Dictionary for Win32(PDIC)へ
DDEを経由して、検索結果をpopupさせる 「どこポップ！」というものがあります。

http://homepage3.nifty.com/TaN/pdic-download.html
にDokoPop!という名前で登録されています。

今後の課題 ---------------------------------------------------------------

・複数行検出
　このプログラムでは１行分しか取得できませんが、
　描画開始位置を追跡することにより、前後の行も抽出可能になると
　思います。

著作権 ------------------------------------------------------------------

再配布、転載、複製、改変、販売等はすべて自由です。一切の制限を設けません。
ただし、以下のことはご留意ください。
・このソフトウェアによって生じる一切の損害責任を負いません。
・このソフトウェアに関して筆者はいかなる義務も負いません。
・再配布されたものに関しては一切関知しません。

履歴 --------------------------------------------------------------------

##>HISTORY.TXT

##EOF

<DKPP.TXT
==========================================================================
                          どこポップ！    Ver.%VERSION%
==========================================================================
紹介 ---------------------------------------------------------------------

　「どこポップ！」は、TaN作のPersonal Dictionary for Win32(以下PDIC)と
　連携して、マウスのクリックによってマウス位置の単語を検索、日本語訳な
　どをポップアップウィンドウによって表示するソフトウェアです。

動作確認OS --------------------------------------------------------------

　Windows98/Me
　WindowsNT4.0/2000/XP

問題点 --------------------------------------------------------------------

##>PROB.TXT

準備 --------------------------------------------------------------------

　%DKPPARC%をそのまま実行して下さい。
　自動的にインストールされます。
　スタートアップに登録した方が便利です。(Windows起動時にDokoPop!も起動)

　どこポップ！にはPDIC for Win32(できればVer.4.50以降)が必要です。
　http://homepage3.nifty.com/TaN/pdic-download.html
  からダウンロードして下さい。

  NT4.0では、PSAPI.DLL が必要です。
  http://homepage3.nifty.com/TaN/psapi.exe
  からダウンロード後、解凍、システムディレクトリ(C:\WINNT\System32など)
  へコピーしてください。

使用方法 -----------------------------------------------------------------

　DKPP.EXEを起動するとタスクトレイに常駐します。(DPと書いてあるアイコン)
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

ちょっと便利な使用方法 ---------------------------------------------------

　「Ctrl + 右クリック」はわずらわしい！！
　という人には、

　「右クリックだけでポップアップ」

　という方法もあります。
　Ctrl + Alt + 右クリック でON/OFFの切り替えができます。
　（切り替え時にメッセージが出ます)

　ヒント：「右クリックだけ」にした場合、「Ctrl + 右クリック」をすると、
　　　　　通常の右クリックメニューが出てきます。
　　　　　ちょうど「Ctrl」が逆転する感じになります。

メニュー -----------------------------------------------------------------

　タスクトレイのDokoPop!アイコンを右クリックするとメニューが現れます。

　辞書グループ         - PDICの辞書グループの変更を行います
　右クリック検索       - マウスの右クリックだけでポップアップ検索を行います
　ポップアップ検索有効 - 検索を有効／無効にします
　設定                 - 各種設定を行います
　説明書を表示         - このファイルを表示します
　バージョン           - version表示
　終了                 - どこポップ！の終了

設定 ----------------------------------------------------------------------

  タスクトレイの右クリックで出てきたメニューの「設定」です。

  ■起動時にスプラッシュを表示する
    起動時の画面中央に出てくるウィンドウの
    表示を有効／無効にします。

    ★起動時のスプラッシュビットマップをカスタマイズ★
    DKPP.EXEがあるフォルダーに DKPP.BMP というファイルがあると、
    そのビットマップを起動時とバージョン...のときに表示します。

  ■辞書グループを指定する
    ここをチェックすると、ポップアップ検索を行うときに
    ここで指定した辞書グループを使用します。
    PDIC本体での辞書グループの選択が変更されます。
    【注意】この機能はPDIC for Win32 Ver.4.40からの対応になります

  ■ポップアップ検索 - ポップアップ検索を行うマウスとキーの組み合わせを指定
    します。

  ■右クリック検索ON/OFF
    右クリック検索の有効・無効を切り替えるマウスとキーの組み合わせです。

  ■Ctrlキーを離したらポップアップを閉じる
    Ctrl+マウスクリックでポップアップ検索を行うようにした場合、
    ポップアップ表示された後、Ctrlキーを離したらポップアップした
    ウィンドウを閉じます。
    （将来的にこのオプションは無くします - 困る！という方はお知らせください）

  ■英数字のみ検索する
    日本語・英数字混在の文章があった場合、日本語を無視して検索を行います。
    混在文章では ON にしたほうがヒットしやすくなります。
    また、フランス語、ドイツ語のようなウムラウト・アクサンなどを
    含む文章ではここを OFF にしないとうまく動作しない場合があります。

その他 --------------------------------------------------------------------

　再利用・再配布・改変などは自由です。
　このソフトウェアによる損害は一切その責任を負いません。

　このprogramは、open sourceであるDCHookのsample programです。
　ソースファイルは、
　http://homepage3.nifty.com/TaN/prog-tips.html
　にあります。(dchook-x.xx.lzh)

　このソフトウェアのサポートは行っていません。ただし、
　http://www4.plala.or.jp/cgi-bin/wforum/wforum.cgi/grape/dkpp
　にて、情報交換を行っています。

　PDIC for Win32は
　http://homepage3.nifty.com/TaN/pdic-download.html

　インストーラにちくわさん(RXQ01346@nifty.ne.jp)のMakeSetを使用しています。

履歴 ----------------------------------------------------------------------

##>HISTORY.TXT

---------------------------------------------------------------------------
##EOF

****************** パッケージ作成用.BATファイル *******************************
<MK.BAT
@echo off
echo/
echo 完全版パッケージを作成します
echo/
echo ./dchook/dchook.dllを準備
echo ./dchooktest/dchooktest.exeを準備
echo ./vxd/hk95d.vxdを準備
pause
set TZ=JST-9

copy .\dchook\dchook.dll .\DKPPHK.DLL /y
copy .\VxD\hk95d.vxd .\HK95D.VXD /y
copy .\dchooktest\dchooktest.exe .\DKPP.EXE /y

touch -d%FDATE% -t%FTIME% -c DKPP.EXE DKPPHK.DLL HK95D.VXD README.TXT

del %ARC% > nul

lha32 a %ARC% /x README.TXT
lha32 a %ARC% /x ./dchook/dchook.mak ./dchook/dchook.cpp ./dchook/dchook.h ./dchook/dchook.rc ./dchook/dchook.def
lha32 a %ARC% /x ./dchooktest/dchooktest.bpr ./dchooktest/dchooktest.mak
lha32 a %ARC% /x ./dchooktest/*.h ./dchooktest/*.cpp ./dchooktest/dchooktest.rc ./dchooktest/*.dfm ./dchooktest/dkpp.bmp ./dchooktest/popenable.bmp ./dchooktest/popdisable.bmp ./dchooktest/popenable.h ./dchooktest/popdisable.h ./dchooktest/icon.rc
lha32 a %ARC% /x ./vxd/makefile ./vxd/hk95d.def ./vxd/*.h ./vxd/*.asm ./vxd/*.c

%CHGINI% %IND% Exe Name %DKPPARC%
%CHGINI% %IND% Date Year %DATE_YY%
%CHGINI% %IND% Date Month %DATE_M%
%CHGINI% %IND% Date Day %DATE_D%
%CHGINI% %IND% Date Hour %TIME_H%
%CHGINI% %IND% Date Minute %TIME_M%
%CHGINI% %IND% Date Second %TIME_S%
%CHGINI% %IND% Back 8 "|Doko Pop! Ver.%VERSION%"
%CHGINI% %IND% Back 12 "|Doko Pop! Ver.%VERSION%"

rem INDの実行
k:\wbin\makeset\makeset .\%IND%

rem lha32 a %DKPPARC% DKPP.TXT DKPP.EXE DKPPHK.DLL HK95D.VXD
ren %DKPPARC% %DKPPARC%
touch -d%FDATE% -t%FTIME% -c %DKPPARC%

echo/
echo 保存用圧縮ファイルを作成します
echo/
rem pause

dir %ARC%

echo/
echo 公開用ファイルを更新します
echo/
rem pause

perl -S -i.bak filestamp.pl DKPPu.TXT
del DKPPu.TXT.bak

echo/
echo Web用のファイルを更新します
echo/
pause

copy %ARC% %HTML%
copy %ARC% %MYSOFT%
del %HTML%dchook-%OLDVERSION%.lzh
copy %DKPPARC% %HTML%
copy %DKPPARC% %MYSOFT%
del %HTML%DKPP%OLDVER%.EXE

copy %VARFILE% vars.dat
perl -S repvars.pl %HTML%pdic-download0.html > %HTML%pdic-download.html
copy vars.dat %VARFILE%

rem echo [del] >> %HTML%ftp.ctl
rem echo dchook-%OLDVERSION%.lzh >> %HTML%ftp.ctl
rem echo DKPP%OLDVER%.EXE >> %HTML%ftp.ctl
rem echo [include] >> %HTML%ftp.ctl
rem echo %DKPPARC% >> %HTML%ftp.ctl
rem echo %ARC% >> %HTML%ftp.ctl


##EOF

<<@%VARFILE%,DP_

