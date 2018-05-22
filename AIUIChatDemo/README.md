AIUIChatDemo是一个集成了AIUI SDK的用于演示AIUI功能的产品Demo，同时开放源码供开发者集成时进行参考。
Demo支持通过语音或文本进行交互，并且会对听写和语义的结果进行处理展示。

## 动态实体与所见即可说

### 介绍

AIUI动态实体的概念参见[AIUI动态实体](http://xfyun-doc.ufile.ucloud.com.cn/1519615970164780/%E5%AE%9E%E4%BD%93%E4%BB%8B%E7%BB%8D3.pdf)。
动态实体还有一种特殊的维度，名为所见即可说。该种动态实体上传的内容只会生效使用一次，在上传后生效交互一次后，即会恢复无数据的状态。 该维度适用于一些临时数据，如只在当前屏幕显示的信息，在上传当前屏幕内容数据后，用户根据当前屏幕的内容进行交互，交互过后屏幕内容即会刷新。

### 使用

AIUI动态实体使用参见[AIUI动态实体使用](http://aiui.xfyun.cn/access_docs/aiui-sdk/mobile_doc/%E7%94%A8%E6%88%B7%E4%B8%AA%E6%80%A7%E5%8C%96.html#动态实体)。
所见即可说使用参见[AIUI所见即可说使用](http://aiui.xfyun.cn/access_docs/aiui-sdk/mobile_doc/%E7%94%A8%E6%88%B7%E4%B8%AA%E6%80%A7%E5%8C%96.html#所见即可说)。

### Demo说明

Demo演示了用户级和自定义两种维度的实体以及所见即可说的使用。

用户级实体定义位于src/main/java/com/iflytek/aiui/demo/chat/model/handler/TelephoneHandler.java。上传成功后，通过 ``打电话给xxx``(xxx为手机联系人）就会识别正确的联系人名字并且返回包含联系人信息的电话语义结果。

自定义级实体定义位于src/main/java/com/iflytek/aiui/demo/chat/model/handler/OrderMenuHandler.java。上传成功后，通过 ``我要点xxx``来进行点菜。

所见即可说实体定义位于src/main/java/com/iflytek/aiui/demo/chat/model/handler/DishSkillHandler.java。

## 常见问题
**Q:Demo中AIUI SDK的调用在哪？**<br>
**A:** Demo对于AIUI SDK的调用位于源码src/main/java/com/iflytek/aiui/demo/chat/repository/AIUIRepository.java中。
  同时Demo中也提供了一个完整调用AIUI SDK的简单入口，源码位于src/sample/java/com/iflytek/aiui/demo/chat/NlpDemo.java，
  你可以通过在Manifest中指定Demo入口Activity为NlpDemo进行体验。

**Q:Demo如何启用唤醒功能？**<br>
**A:** 参考[语音唤醒文档](http://aiui.xfyun.cn/access_docs/aiui-sdk/mobile_doc/%E4%BA%A4%E4%BA%92%E6%8E%A7%E5%88%B6.html#语音唤醒)。