#include <stdio.h>
 
int  main(){
    int a = v1,b = v2;
    a = a^b;
    b = a^b;
    a = a^b;
    printf("a = %d, b = %d\n",a,b);
    return 0;
    
}


// 输出时 V2  V1
/*

异或 运算  
两个相同的数异或为0  0与任何数异或为任何数本身

*/