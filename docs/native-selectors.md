# Native selector mangling

Kernel NativeMethod の安定シンボル名。パターン:

```
ao_<Class>_<selectorMangled>
```

例: `Object>>#==` は `ao_Object_identityEquals`、`SmallInteger>>#+` は `ao_SmallInteger_add`。

イメージとヒープのポインタスロットに関数ポインタは書かない。`.aoimage` に載るのはこの名前だけである。P7 のロードは名前で `NativeRegistry` に再結合する。

## 規則

- 識別子 unary はそのまま（`class` → `class`）。
- キーワードは各キーワード部分の後に `_`（`doesNotUnderstand:` → `doesNotUnderstand_`、`at:put:` → `at_put_`）。
- 二項演算子は下表に列挙する。表に無い演算子を足すときは本ファイルを更新する。

## マングル表

| セレクタ | selectorMangled | 例 |
|---|---|---|
| `==` | `identityEquals` | `ao_Object_identityEquals` |
| `~~` | `identityNotEquals` | `ao_Object_identityNotEquals` |
| `=` | `equals` | `ao_Object_equals` |
| `class` | `class` | `ao_Object_class` |
| `+` | `add` | `ao_SmallInteger_add` |
| `&` | `and` | `ao_True_and` |
| `\|` | `or` | `ao_True_or` |
| `doesNotUnderstand:` | `doesNotUnderstand_` | `ao_Object_doesNotUnderstand_` |
| `at:put:` | `at_put_` | `ao_Array_at_put_` |
| `ifTrue:` | `ifTrue_` | `ao_True_ifTrue_` |
| `ifTrue:ifFalse:` | `ifTrue_ifFalse_` | `ao_True_ifTrue_ifFalse_` |
| `and:` | `and_` | `ao_True_and_` |
| `or:` | `or_` | `ao_True_or_` |
| `eqv:` | `eqv_` | `ao_True_eqv_` |
| `xor:` | `xor_` | `ao_True_xor_` |

C++ の関数名はマングル名と一致させる。
