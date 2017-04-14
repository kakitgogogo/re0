# re0
一个简单的正则表达式引擎，通过转换成NFA实现。仅支持() ，| , + , ? , * ，只能用来判断是否匹配。

## 使用
`make nfa`得到nfa，用于将一个正则表达式转换成NFA，然后用dot显示NFA。
`make match`得到match，用于判断字符串匹配于正则表达式。
`make test`得到test，用于单元测试。
