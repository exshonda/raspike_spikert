# RasPike互換モジュール

このプロジェクトはRasPikeで使用されるLEGO SPIKEで実行するエージェント
プログラムをSPIKE-RTに移植したものである．

https://github.com/ETrobocon/RasPike/blob/master/spike/raspike_etrobo.py

# 接続関係の変更点

- Raspberry PiとSPIKEの接続
	- アームモータ   : A
	- 右モータ       : B
	- 左モータ       : E
	- カラーセンサー : C
	- 超音波センサー : F -> D
	- serial通信     : D -> F

# Raspberry Pi -> SPIKE 送信フォーマット


# SPIKE -> Raspberry Pi 送信フォーマット

10msec毎に送信
