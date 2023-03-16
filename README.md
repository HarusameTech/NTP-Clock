# NTP-Clock
## フォルダ階層

- `3D_models/`
  - 筐体の 3D モデル置き場
- `NTP_Clock/`
  - [`NTP_Clock.ino`](./NTP_Clock/NTP_Clock.ino)
    - ソースファイル
- [`README.md`](./README.md)
  - このファイル
- [`chassis_prot_note.md`](./chassis_prot_note.md)
  - 筐体設計メモ
- [`partslist.md`](./partslist.md)
  - 使用する部品のリスト

## 入れたい機能

- WebUI, Telnet, SSH による設定
- 時報
- 7 セグ信号を吐く外部出力端子
- 照度計による時報制御

## ハードウェア

※予定です

使用マイコン

- Raspberry Pi Pico
  - <https://akizukidenshi.com/catalog/g/gM-16132/>

イーサネットコントローラ IC

- W5500 モジュール
  - <https://www.amazon.co.jp/gp/product/B08C7DX4HN/>
  - 3.3V IO 5V トレラント

7 セグメントディスプレイ

- MAX7219
  - <https://www.amazon.co.jp/dp/B09FZ4S5JQ/>
  - 5V IO なので、レベルコンバータが必要

照度計

- 適当な CdS
- 適当な抵抗
- 可変抵抗（しきい値調整用）
- コンパレータ（照度としきい値の比較用）
