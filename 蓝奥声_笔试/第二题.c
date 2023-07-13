/*第二题*/  

#include <stdio.h>
//如果一张图片为白色，灰度值拉满
//黑色就是灰度值为零
#define WIDTH 1024 // 定义图像  宽度
#define HEIGHT 768 // 定义图像  高度
typedef unsigned char Uint8;// 定义无符号8位整型

// 像素灰度图像的平均灰度值和大于平均灰度值的像素总数量
//double* average表示指向double类型变量的指针，用于存储灰度图像的平均值。
//int* count表示指向int类型变量的指针，用于存储灰度图像的像素点数量。
void calculateGrayscaleMetrics(Uint8 image[HEIGHT][WIDTH], double* average, int* count) {
    int total = 0;
    *count = 0;//被设置为0的变量是由指针变量count指向的。这行代码通常用于将变量初始化为已知值，这里是0。
    // 像素灰度总和
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            total += image[i][j];
        }
    }
    // 平均灰度值
    //使用 *average 可以访问和修改该指针所指向的 double 类型变量的值。
    //不需要使用 (double *)，是因为 average 已经是一个指向 double 类型变量的指针。
    //在这种情况下，我们只需使用 *average 来访问指针所指向的变量，而无需使用 (double *) 进行类型转换。 
    *average = (double)total / (WIDTH * HEIGHT);//将一幅图所有像素值加总除以总像素个数，得到平均灰度值。
    // 大于平均灰度值的像素数量
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            //image[i][j] 表示二维数组 image 中的特定像素值，而 *average 表示指针 average 所指向的平均灰度值。
            if (image[i][j] > *average) {//如果像素值大于平均灰度值，则将 *count 增加 1。
                //(*count)++ 表示递增指针 count 所指向的变量。它是一个指针解引用操作，用于访问和修改指针所指向的内存中的值。
                //因为 count 是一个指向整数类型变量的指针，所以使用 (*count) 可以访问和修改该指针所指向的整数类型变量的值。
                (*count)++;//通过使用 ++ 运算符，我们对变量的值进行递增操作。
            }
        }
    }
}
//Grayscal  灰度；
int main() {
    Uint8 image[HEIGHT][WIDTH];  // 像素灰度图像数组
    double average;              // 平均灰度值
    int count;                   // 大于平均灰度值的像素数量
    // 在此处填充像素灰度图像数据存储到image数组中
    calculateGrayscaleMetrics(image, &average, &count);

    printf("平均灰度值：%.2f\n", average);

    printf("大于平均灰度值的像素数量：%d\n", count);
    return 0;
}
/*
1    定义了图像的宽度和高度，分别为 1024 和 768。同时，
使用 typedef 定义了 Uint8 类型，表示无符号 8 位整数（范围为 0 到 255）。
命名空间污染：#define 定义的标识符没有作用域限制，它们是全局的。
这可能导致命名冲突和意外的替换。如果在代码的其他地方已经定义了同样的标识符，可能会产生意外的结果。

2    定义了函数 calculateGrayscaleMetrics，该函数接受一个二维数组 image，
表示灰度图像，以及两个指针参数 average 和 count，分别用于存储计算结果。

3    在 calculateGrayscaleMetrics 函数中，首先初始化变量 total 为 0，
用于累加像素值的总和，初始化变量 *count 为 0，用于统计高于平均值的像素数量。

使用嵌套循环遍历图像的每个像素，累加像素值到变量 total 中。

计算平均灰度值，将 total 除以图像的像素总数（即 WIDTH * HEIGHT），并将结果存储在变量 *average 中。

再次使用嵌套循环遍历图像的每个像素，如果像素值大于平均灰度值 *average，则将 *count 增加 1。

返回到 main 函数，声明了一个名为 image 的二维数组，用于存储灰度图像的像素值。

定义了变量 average 和 count，用于接收计算结果。

调用函数 calculateGrayscaleMetrics，传递图像数组 image 以及 &average 和 &count 的地址作为参数，
计算平均灰度值和高于平均值的像素数量。

最后，使用 printf 函数打印出计算结果，分别输出平均灰度值和高于平均值的像素数量。

整体思路是通过遍历图像的每个像素，累加像素值并计算平均值，然后再次遍历图像，
统计高于平均值的像素数量。这样可以得到图像的灰度统计信息。

*/
	