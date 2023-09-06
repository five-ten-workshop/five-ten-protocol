# five-ten-protocol
DLC/645 like protocol

main2()を呼び出しすることで、1005500本体との通信を行い、測定結果を出力します。
測定結果は、
1.地絡接地抵抗値(Ω)
2.地絡電圧(V)
の2点です。
結果は、main2()の引数に参照渡しされます。
main2()の戻り値が0以外の場合はエラーです。再度main2()を呼び出してください。

使用するマイコンにより、プラットフォームを変更して使用します。
プラットフォームを変更する場合、ヘッダファイルにある大文字で定義されたマクロ文を修正してください。
修正する必要のあるのは、
1.シリアル送信、受信部への1バイトの送信、受信命令
2.シリアルポートの受信バッファがemptyであることのわかる命令
3.シリアルポートの送信バッファがemptyになるまで待たせる命令(emptyになると関数からリターンする)
4.I/Oポートの属性(出力、入力)切り替え
5.wait命令(引数の単位はmS(ミリ秒)です)
6.I/Oポートへの1bitのread, writeを行う命令
となります。以上の命令はプラットフォームのライブラリに準じて書きかえて使用してください。

外部仕様書に記載の通り、通信時のパラメータは以下の通りとなります。
1.通信速度 : 9600bps
2.パリティ : even(あまり使わないパラメータとなります。注意。)
3.ストップビット : 1bit

RS485の通信は2線式のため、送信するときに受信部をdisableする必要があります。
I/Oポートを一つ割り当てて、送受信の切り替えに使用します。
RS485チップによっては、受信部をdisableしたときにハイインピーダンスになることがあります。必要に応じて入力ポートのプルアップを実施してください。そうでないと、受信後にゴミが受信され、本ライブラリの動作が不定になることがあります(具体的には、受信関数が呼び出されっぱなしになることがあり、呼び出されっぱなしになるった時は、次回のプロトコル受信で動作が継続する)。
もし、ハードウェアが受信部をdisableできない場合、#ECHO_CANCELマクロを有効にしてください。
内部で送信データのエコーキャンセルをします。

通信の流れは、dlt645の規格により、動作1回ごとにハンドシェークが1回必要となります。
ハンドシェークの中身は、
ホストがコマンドを送信し、スレーブが受信。
スレーブがその応答を送信し、ホストが受信。
となります。この流れを繰り返します。
スレーブ応答が500mS以上スタックすると、ホストはエラーと認知します。
命令を出すにしても、状態を得るにしても、必ずハンドシェークが一回行われます。
この制約から、待ち時間のある結果を取得する場合、ステータスを確認しながら
データが更新されるのを待ちます。

物理レイヤーのパリティエラー(evenパリティ)、プロトコルレイヤーのパリティエラー
(フッタ手前の1byte)が発生した場合、受信側は内容をすべて破棄します。
すると、この500mS制約に引っかかり、ホスト側はエラーを察知し、再送する流れになります。
