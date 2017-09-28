/*************************************************************************
    > File Name: src/main.c
    > Author: 刘少鹏
    > Mail: 2424492167@qq.com
    > Created Time: 2017年09月28日 星期四 16时30分59秒
 ************************************************************************/

#include "commond.h"

typedef struct {
	pid_t pid;
	char**cmd;
}Program;									//保存命令

void printAd();								//打印广告（其实就是一个语句打印）
void show_environment();					//打印环境表
void trim(char**line);						//清除前导空格
char* take_word(char**line);				//获取一个单词
void do_line(Program* prog,char* cmd);		//处理命令
void exec_program(Program*prog);			//执行命令

extern char** environ;		//环境表
jmp_buf env;				//长跳转


void trim(char**line){		//去除前导空格
	while(**line && **line == ' '&&(*line)++);	//先判断是不是结束符，然后判断是不是空格，最后进行自加
}

char*	take_word(char**line){
	trim(line);			//删除前导空格	
	//使两个指针都指向line的开头
	char* start	 = *line;
	char* end	 = start;
	//向后遍历找到空格或者结束符之后跳出循环
	while(*end != '\0' && *end!=' ' && end++);
	//将空格替换为结束符（如果是的话）
	*end = '\0';
	//将line指向end之后的一个位置
	*line = end + 1;
	//如果两个指针相同，则证明传入的为一个空指针，则返回空
	if(start == end)	return NULL;
	return start;		//返回得到的指针
}
void do_line(Program* prog,char* cmd){	
	char * word = take_word(&cmd);						//获取一个单词
	if(!word)	longjmp(env,0);							//如果没有获取到单词，则进行长跳转
	prog->cmd = calloc(5,sizeof(char*));				//开辟内存空间
	int index = 0;								
	while(index < 4){									//命令中最多出现四个单词
		prog->cmd[index] = calloc(1,strlen(word)+1);	//开辟内存空间（为命令的存放）
		strcpy(prog->cmd[index],word);					//拷贝命令
		prog->cmd[index][strlen(word)] = '\0';			//在结尾处加上结束符
		if((word = take_word(&cmd)) == NULL)	break;	//如果没有获取到单词，则跳出循环
		index ++;										//index自加
	}
	prog->cmd[index+1] = NULL;							//将最后一个字符串指向NULL，目的是为了execvp函数的参数解析
}

void exec_program(Program* prog){
	if(!strcmp(prog->cmd[0],"echo")){					//如果输入的是echo，则打印环境表
		show_environment();	
		return;
	}
	pid_t pid = fork();									//创建子进程
	if(pid<0){}											//创建失败
	else if(pid == 0){									//在子进程中
		prog->pid = getpid();							//记录子进程号
		if(execvp(prog->cmd[0],prog->cmd) < 0){			//执行命令  如果出错
			
		}
	}
	else{												//在父进程中		等待子进程结束
		prog->pid = pid;
		wait(0);
	}
}
void handle_err(int ret){								//处理长跳转错误信息打印

}

int main(int argc,char* argv[]){
	printAd();
	int ret = setjmp(env);								//设置跳转点
	if(env < 0){										//判断返回值	出错
		fprintf(stderr,"setjmp error:%s\n",strerror(errno));
		exit(-1);
	}else if(env == 0){									//第一次设置
	
	}else if(env > 0){									//长跳转  进行错误处理
		handle_err(ret);								//错误处理函数
	}
	char buff[1024] = {'\0'};
	ssize_t len;
	char * prompt = getenv("PS1");						//获取命令提示符  
														//如果出现段错误，可能是没有获取到正确的值，可以去检查一下自己的环境表
	while(1){
		write(STDOUT_FILENO,prompt,strlen(prompt));		//打印提示符
		memset(buff,0,sizeof(buff));					//清空数组
		len = read(STDIN_FILENO,buff,sizeof(buff));		//读取数据
		if(len == 1)	continue;						//如果只读到一个字符
		if(len>0)	buff[len - 1] = '\0';
		Program * prog = calloc(1,sizeof(Program));		//定义变量并开辟内存空间
		do_line(prog,buff);								//解析命令
		if(prog->cmd == NULL){							//如果没有命令输入
			free(prog);									//释放内存空间
			continue;									//跳过本次循环
		}
		if(!strcmp(prog->cmd[0],"exit"))	break;		//如果输入的是exit则退出
		exec_program(prog);								//执行命令
		free(prog);										//释放内存空间
	}
	return 0;
}

void printAd(){					//打印（所谓广告）
	int fd = open("doc/ad.txt",O_RDONLY);
	char buf[1024] = {'\0'};
	ssize_t len;
	while((len = read(fd,buf,sizeof(buf)))>0){
		write(STDOUT_FILENO,buf,len);
		memset(buf,0,sizeof(buf));
	}
	write(STDOUT_FILENO,"\n",1);
}

void show_environment(){			//打印环境表
 	char**tmp = environ;
	while(*tmp != NULL){
		write(STDOUT_FILENO,*tmp,strlen(*tmp));
		write(STDOUT_FILENO,"\n",1);
		tmp++;
	}
}
