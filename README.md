# NTP-Clock

- `./NTP_Clock`
  - [`NTP_Clock.ino`](./NTP_Clock/NTP_Clock.ino)
  - ソースファイル
- [`README.md`](./README.md)
  - このファイル

## 入れたい機能

- WebUI, Telnet, SSH による設定
- 時報
- 7 セグ信号を吐く外部出力端子
- 照度計による時報制御

## 構成

※予定です

使用コントローラ

- Raspberry Pi Pico

イーサネットコントローラ IC

- ENC28J60
- 3.3V IO 5V トレラント

7 セグメントディスプレイ

- MAX7219

照度計

- 適当な CdS と抵抗での分圧
