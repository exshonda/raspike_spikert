# RasPike互換モジュール

このプロジェクトはRasPikeで使用されるLEGO SPIKEで実行するエージェント
プログラムをSPIKE-RTに移植したものである．

## オリジナルのエージェントプログラム

https://github.com/ETrobocon/RasPike/blob/master/spike/raspike_etrobo.py

## ビルド方法

カーネルライブラリ等を作成．

```
$ git clone https://github.com/spike-rt/spike-rt.git
$ cd spike-rt
$ git submodule update --init ./external/ 
$ ./scripts/build-test.sh
```
アプリケーションのクローンとビルド

```
$ mkdir app
$ cd app
$ git clonehttps://github.com/exshonda/raspike_spikert.git
$ cd raspike_spikert
$ make
```

## 接続関係の変更点

- Raspberry PiとSPIKEの接続
	- アームモータ   : A
	- 右モータ       : B
	- 左モータ       : E
	- カラーセンサー : C
	- 超音波センサー : F -> D
	- serial通信     : D -> F

## Raspberry Pi -> SPIKE 送信

- データを受信するとパケット解析を開始

- void receiver_task(intptr_t exinf)
	- 受信タスク
	- オリジナル関数 async def receiver():

## SPIKE -> Raspberry Pi 送信

- 10msec毎に送信
- 100msec毎に送信

- void notify_sensor_task(intptr_t exinf)
	- 送信タスク
	- オリジナル関数 sync def notifySensorValues():
