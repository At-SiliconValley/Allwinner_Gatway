#include "fcntl.h"
#include "unistd.h"
#include "log.h"
#include "stdio.h"
int main(void){

    //系统io使用
    // int f1 = open("test.txt",O_RDWR );
    // if(f1 == -1){
    //     perror("文件1打开失败");
    //     return 1;
    // }
    // int f2 = open("hello.txt",O_RDWR | O_CREAT , 0666);
    // if(f2 == -1){
    //     perror("文件2打开失败");
    //     return 1;
    // }

    // char datas[256] = {0};
    // ssize_t size = 0;
    // while( (size = read(f1,datas,256)) > 0 ){
        
    //     write(f2,datas, size);
    // }

    // close(f1);
    // close(f2);

    //标准io使用
    // FILE * f1 = fopen("test.txt","r");
    // if(f1 == NULL){
    //     perror("文件1打开失败");
    //     return 1;
    // }

    // FILE* f2 = fopen("hello.txt","wa");
    // if(f2 == NULL){
    //     perror("文件2打开失败");
    //     return 1;
    // }

    // char datas[256] = {0};
    // size_t size = 0;
    // while( (size = fread(datas,1,256,f1) ) > 0){
    //     fwrite(datas, 1 , size, f2);
    // }

    // fclose(f1);
    // fclose(f2);

    //系统输入输出
    //从控制台读取数据,写入控制台
    // char datas[50] = {0};
    // ssize_t size = 0;
    // while( (size = read(STDIN_FILENO,datas,50)) > 0 ){
    //     write(STDOUT_FILENO,datas,size);
    // }

    //标准输入输出
    // char datas[20] = {0};
    // size_t size = 0;
    // while( (size = fread(datas, 1, 20, stdin)) > 0){

    //     fwrite(datas, 1 , size, stdout);
    // }

    //系统输入输出的重定向
    //输入重定向
    // int f1 = open("test.txt", O_RDWR);
    // // dup2( f1, STDIN_FILENO);
    // //输出重定向
    // int f2 = open("hello.txt",O_RDWR | O_CREAT, 0666);
    // dup2(f2,STDOUT_FILENO);
    // char datas[50] = {0};
    // ssize_t size = 0;
    // while( (size = read(f1,datas, 50)) > 0 ){
    //     write(STDOUT_FILENO,datas, size);
    // }

    // close(f1);
    // close(f2);

    //标准输入输出重定向
    freopen("test.txt", "r",stdin );
    freopen("hello.txt","wa",stdout);
    char datas[20] = {0};
    size_t size = 0;
    while( (size = fread(datas, 1, 20, stdin)) > 0){

        fwrite(datas, 1 , size, stdout);
    }
}