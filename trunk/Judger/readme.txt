
基于Windows平台的HUSTOJ评判内核

当前版本：1.0

作者：游蓝海
e-mail: you_lan_hai@foxmail.com
个人主页：http://blog.csdn.net/you_lan_hai
创建日期: 2013年1月


原理说明：
	数据库管理模块（DBManager）从数据库查询出当前结果为待定和等待重判的提交信息，
并组装成内部可识别任务数据（Task），然后将Task转交给评判单元（JudgeCell），等待评判。
	评判单元（JudgeCell）管理着一组评判内核（JudgeCore），每个评判内核（JudgeCore）都运行在独立的线程中。
评判内核不断的从评判单元中获取任务，如果发现任务就进行一次评判，然后通过回调接口，将评判结果反馈给上层，
最终，评判的结果会反馈给DBManager。
	DBManager收到结果后，将结果写回数据库。
	一个评判内核（JudgeCore）由三个执行部件（Excuter）组成，分别是编译器（Compiler）、
执行器(Runner)、匹配器(Matcher)组成，分别负责编译用户代码，执行用户程序并生成输出数据，
匹配用户程序的输出数据是否与测试数据匹配。JudgeCore目前只支持c和c++两种语言。


解决方案说明：
	解决方案共有3个项目，LZData,acm,Judger。
	LZData，是我其他项目中读/写配置文件的工具，目前支持LZD和XML两种格式。对XML格式的支持不是很完善，
目前只支持赋值类语法，不支持注释、帮助等格式的语法。
	acm，简单的封装了一些常用的windows API，如线程、进程、网络通信、文件处理、MySql等。
	Judger，是HUSTOJ的评判内核程序。

TODO:
1.支持评判内核沙箱运行模式。
2.增加对Java的支持。
3.增加对sim的支持。
4.评判单元多进程化。
5.HUSTOJ IDE测试功能的支持。

