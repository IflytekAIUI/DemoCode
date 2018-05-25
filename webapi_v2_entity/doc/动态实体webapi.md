# API说明

* 授权认证，调用接口需要将nameSpace，nonce，curtime和checkSum信息放在HTTP请求头中。
* 所有接口统一为UTF-8编码。
* 所有接口支持http和https。

# 授权认证

在调用所有业务接口时，都需要在Http Request Header中加入以下参数作为授权验证

|参数名|说明|是否必须|
| ------|----------------------|--------|
| X-NameSpace | aiui开放平台个人中心的命名空间 |是|
| X-Nonce |随机数（最大长度128个字符）|是|
| X-CurTime |当前UTC时间戳，从1970年1月1日0点0 分0 秒开始到现在的秒数(String)|是|
| X-CheckSum |MD5(accountKey + Nonce + CurTime),三个参数拼接的字符串，进行MD5哈希计算|是|

注：

* CheckSum有效期：出于安全性考虑，每个CheckSum的有效期为5分钟(用curTime计算)，同时CurTime要与标准时间同步，否则，时间相差太大，服务端会直接认为CurTime无效。

* checkSum生成示例，例如：

		accountKey是abcd1234, Nonce是12, CurTime是1502607694。那么CheckSum为MD5(abcd1234121502607694)
		最终MD5为32位小写 bf5aa1f53bd173cf7413bf370ad4bddc

# IP 白名单

IP 白名单具备打开和关闭两种状态。

* 当 IP 白名单打开时，用户在调用所有业务接口时，在授权认证通过后，检查调用方ip是否在aiui开放平台配置的ip白名单中。若在，则向用户提供服务，否则拒绝提供服务。

* 当 IP 白名单关闭时，任意终端均可访问 AIUI 服务器，开发者需要自行保证 nameSpace 和 key 值安全。

注：拒绝提供服务返回值：{"code":"20004","desc":"ip非法","data":null,"sid":"rwabb52e660@dx6c9b0e56f81d3ef000"}

# 通用请求地址

	base_url：openapi.xfyun.cn

# AIUI接口

 通用返回参数

|参数名|说明|是否必须|
| ------|----------------------|--------|
|code	|结果码	|是|
|data	|返回结果	|是|
|desc	|描述	|是|
|sid	|本次webapi服务唯一标识	|是|

## 动态实体
### 上传资源

* 接口描述

	本接口提供动态实体上传资源功能，用于动态更新实体资源。

* 接口地址

	POST /v2/aiui/entity/upload-resource HTTP/1.1

	Content-Type:application/x-www-form-urlencoded; charset=utf-8

* 参数说明

|参数|类型|必须|说明|<div style="width:40%;">示例</div>|
| ------|-------|-------|--------|--------|
|appid|string|是|应用id|5adde3cf|
|res_name|String|是|资源名,XXX为用户的命名空间|XXX.music|
|pers_param|String|是|个性化参数（json）|{"appid":"xxxxxx"}|
|data|String|是|Base64编码的资源|示例1|

其中，pers_param为个性化参数。示例如下：

|维度|示例|说明|
| ------|-------|-------|
|应用级|{"appid":"xxxxxx"}||
|用户级|{"auth_id":"d3b6d50a9f8194b623b5e2d4e298c9d6"}|auth_id为用户唯一ID（32位字符串，包括英文小写字母与数字，开发者需保证该值与终端用户一一对应）|
|自定义级|{"xxxxxx":"xxxxxx"}||

data为web页面定义的主子段、从字段给的json格式对应的base64。例如，主子段为song、从字段singer，上传资源的格式为：

	{"song":"给我一首歌的时间","singer":"周杰伦"}
	{"song":"忘情水","singer":"刘德华"}
	{"song":"暗香","singer":"刘德华"}
	{"song":"逆光","singer":"梁静茹"}

注：每条数据之间用换行符隔开。

Base64编码为

示例1：

	eyJzb25nIjoi57uZ5oiR5LiA6aaW5q2M55qE5pe26Ze0Iiwic2luZ2VyIjoi5ZGo5p2w5LymIn0NCnsic29uZyI6IuW/mOaDheawtCIsInNpbmdlciI6IuWImOW+t+WNjiJ9DQp7InNvbmciOiLmmpfpppkiLCJzaW5nZXIiOiLliJjlvrfljY4ifQ0KeyJzb25nIjoi6YCG5YWJIiwic2luZ2VyIjoi5qKB6Z2Z6Iy5In0=

* 返回说明

|参数	|类型	|必须	|说明	|示例|
| ------|-------|-------|--------|--------|
|sid	|String	|是	|本次上传sid，可用于查看上传资源是否成功|psn003478f3@ch00070e3a78e06f2601|
|csid	|String	|是	|本次服务唯一标识|rwa84b7a73b@ch372d0e3a78e0116200|

### 查看上传资源是否成功

* 接口描述

	本接口提供检查动态实体上传资源是否成功。

	注：上传资源数据后至少间隔3秒后再进行查看上传资源是否成功

* 接口地址

	POST /v2/aiui/entity/check-resource HTTP/1.1

	Content-Type:application/x-www-form-urlencoded; charset=utf-8

* 参数说明

|参数|类型|必须|说明|示例|
| ------|-------|-------|--------|--------|
|sid|string|是|sid|psn开头的sid|

* 返回说明

|参数	|类型	|必须	|说明	|
| ------|-------|-------|--------|
|sid	|String	|是	|上传sid|
|csid	|String	|是	|上传sid|
|reply	|String	|是	|查看上传资源是否成功描述|
|error	|int	|是	|查看上传资源是否成功错误码|