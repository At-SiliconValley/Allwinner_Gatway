#目标:依赖文件
#[制表符]指令1
#[制表符]指令2
#目标和最终输出文件名一般一样 如果不一样 每次都会重新执行

#变量定义
#变量赋值       变量:=值       
#变量追加值		变量+=值
#变量使用	$(变量)
dep_file:=test/hello.c
dep_file+=test/main.c
#-g: 开启debug调试
#-O0:不优化
#-Wall: 开启警告
CCFLAG:=-g -O0 -Wall
#$^ : 使用所有依赖
#$@ : 使用目标
#$< : 使用第一个依赖
#@: 不打印指令
#-: 忽略报错继续执行

# hello.i: hello.c
# 	gcc -E hello.c -o hello.i

# main.i: main.c
# 	gcc -E main.c -o main.i

# hello.S: hello.i
# 	gcc -S hello.i -o hello.S

# main.S: main.i
# 	gcc -S main.i -o main.S

# hello.o: hello.S
# 	gcc -c hello.S -o hello.o

# main.o: main.S
# 	gcc -c main.S -o main.o

# main: hello.o main.o
# 	gcc hello.o main.o -o main
#伪目标一般用于没有依赖不需要生成文件的时候,使用伪目标之后每次都会执行
.PHONY: clean


clean: 
	rm -rf *.i *.o *.S main

main: $(dep_file)
	-@gcc $(CCFLAG)  $^ -o $@
	-@./$@
	-@mkdir aa/bb/cc/dd
	-@echo $<
