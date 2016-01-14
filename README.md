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
