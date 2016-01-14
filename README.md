### apple push php 扩展

- 依赖

 - openssl
 
- 知识点
 
 - c语言基础
 - 编写php扩展
 - 二进制数据
 - 大端序小端序

- 如何安装扩展
 - git clone 项目
 - phpize
 - ./configure
 - make
 - make install

- 遇到的问题
 - openssl不存在,安装即可
 - 其他暂未发现问题

- 参考项目 https://github.com/yuduanchen/apns
 - 原有项目最后一次commit是三年前,现在php版本已经5.6.*
 - make的时候报错
 - 扩展的一些老的用法,现在已经不需要
 - apple push的底层二进制结构,已经更改,这地方我已经重新用c coding.apple官网的文档我也是服了https://developer.apple.com/library/ios/documentation/NetworkingInternet/Conceptual/RemoteNotificationsPG/Appendixes/BinaryProviderAPI.html , “Populate with the number 2”, 0和1的结构我已经找不到了,我直接安装2写的,真他妈2
- 使用方法
 
 ```php
<?php 
apple_push::$passphrase ="";
apple_push::$certificate="";
try{
        apple_push::init();
        $token="";
        $arr = array(
                    "aps"=>array(
                            "alert"=>"higo",
                            "badge"=>1,                                                                                                      
                        ),  
               );
        $json = json_encode($arr);
        apple_push::send($token, $json);                                                              
}catch(exception $e){
        //TODO record log
}
?>
 ```
