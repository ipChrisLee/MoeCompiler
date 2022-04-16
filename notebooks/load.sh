#!/bin/bash
mkdir -p docsFromCSC

tmp="docsFromCSC/SysY2022Doc.pdf"
if ! [ -f $tmp ];then
    curl "https://gitlab.eduxiji.net/nscscc/compiler2022/-/raw/master/SysY2022%E8%AF%AD%E8%A8%80%E5%AE%9A%E4%B9%89-V1.pdf?inline=false" --output $tmp
fi

tmp="docsFromCSC/armDoc.pdf"
if ! [ -f $tmp ];then
    curl "https://gitlab.eduxiji.net/nscscc/compiler2021/-/raw/master/ARM%E5%92%8CThumb-2%E6%8C%87%E4%BB%A4%E9%9B%86%E5%BF%AB%E9%80%9F%E5%8F%82%E8%80%83%E5%8D%A1.pdf?inline=false" --output $tmp
fi

tmp="docsFromCSC/techProtocol.html"
if ! [ -f $tmp ];then
    curl "https://mp.weixin.qq.com/s/fhU_4HbHaUk8iuWkHpimFw" --output $tmp
fi

tmp="docsFromCSC/ISO-C99.pdf"
if ! [ -f $tmp ];then
    curl "http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf" --output $tmp
fi

