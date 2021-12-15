## ライフゲームの課題について
以下、ライフゲームの課題でやった内容を簡潔にまとめる。

## 課題1について
- 重複しないように時間に依存する乱数を生成し、それが0.1を超えない場合のみを抽出して出力した。
- これにより、ランダム初期化の際に生きたセルの割合がおよそ10%になった。

## 課題2について
- 100世代ごとの盤面の情報をLife1.06形式でファイルに出力するよう実装した。
- 10000世代以上は出力しないようにした。