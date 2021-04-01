#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>
#include<linux/limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>

#define PARAM_NONE 0   //无参数
#define PARAM_A 1      //-a：现实所有文件
#define PARAM_L 2      //-l：一行只显示一个文件的详细信息
#define PARAM_R 4      //-R:
#define MAXROWLEN 80   //一行显示的最多的字符数
#define MAXFILES 51200

int g_leave_len = MAXROWLEN;    //一行剩余长度，用于输出对齐
int g_maxlen;                   //存放某目录下最长文件名的长度


//错误处理函数，打印出错误所在行的行数和错误信息
void my_err(const char * err_string, int line)
{
    fprintf(stderr, "line:%d ", line);
    perror(err_string);
    exit(1);
}

//对文件名进行快排
void qsort_string(char **num, int order[], int start, int finish)
{
    int left = start;
    int right = finish;
    int duff;
    if(start < finish)
    {
        duff = order[start];
        while(start < finish && strcmp(num[order[finish]], num[duff]) >= 0)
        {
            finish--;
        }
        order[start] = order[finish];
        while(start < finish && strcmp(num[order[start]], num[duff]) <= 0)
        {
            start++;
        }
        order[finish] = order[start];
    }
    if(start == finish)
    {
        order[finish] = duff;
    }
    qsort_string(num, order, finish+1, right);
    qsort_string(num, order, left, finish-1);
}

void display_R(struct stat buf, int flag_param, char *path)
{
    if(S_ISDIR(buf.st_mode))    //如果该目录中有一个目录的话 递归回display_dir
    {
        display_dir(flag_param, path);
    }
    display(flag_param, path);
}


//获取文件属性并打印
void display_attribute(struct stat buf, char * name)
{
    char buf_time[32];
    struct passwd *psd;         //从该结构体中获取文件所有者的用户名
    struct group *grp;          //从该结构体中获取文件所有者所属组的组名

    //获取并打印文件类型
    if(S_ISLNK(buf.st_mode)){
        printf("l");
    }
    else if(S_ISREG(buf.st_mode)){
        printf("-");
    }
    else if(S_ISDIR(buf.st_mode)){
        printf("d");
    }
    else if(S_ISCHR(buf.st_mode)){
        printf("c");
    }
    else if(S_ISBLK(buf.st_mode)){
        printf("b");
    }
    else if(S_ISFIFO(buf.st_mode)){
        printf("f");
    }
    else if(S_ISSOCK(buf.st_mode)){
        printf("s");
    }
    
    //获取并打印文件所有者的权限
    if(buf.st_mode & S_IRUSR){
        printf("r");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IWUSR){
        printf("w");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IXUSR){
        printf("x");
    }
    else{
        printf("-");
    }

    //获取并打印与文件所有者同组的用户对该文件的操作权限
    if(buf.st_mode & S_IRGRP){
        printf("r");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IWGRP){
        printf("w");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IXGRP){
        printf("x");
    }
    else{
        printf("-");
    }

    //获取并打印其他用户对该文件的操作权限
    if(buf.st_mode & S_IROTH){
        printf("r");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IWOTH){
        printf("w");
    }
    else{
        printf("-");
    }

    if(buf.st_mode & S_IXOTH){
        printf("x");
    }
    else{
        printf("-");
    }

    printf("    ");


    //根据uid与gid获取文件所有者的用户名与组名
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    printf("%-4d", buf.st_nlink);    //打印文件的链接数
    printf("%-8s", psd->pw_name);
    printf("%-8s", grp->gr_name);

    printf("%6d", buf.st_size);     //打印文件的大小
    strcpy(buf_time, ctime(&buf.st_mtime));
    buf_time[strlen(buf_time) - 1] = '\0';  //去掉换行符
    printf(" %s", buf_time);                //打印文件的时间信息
}

//每调用一次打印一个文件名
void display_single(char * name)
{
    int i, len;

    //如果本行不足以打印一个文件名则换行
    if(g_leave_len < g_maxlen)
    {
        printf("\n");
        g_leave_len = MAXROWLEN;
    }

    len = strlen(name);
    len = g_maxlen - len;   //不好理解
    printf("%-s", name);    //打印文件名

    for(i=0; i<len; i++)
    {
        printf(" ");
    }

    printf("  ");
    //下面的2指示空两格
    g_leave_len -= (g_maxlen + 2);
}


//根据命令行参数和完整路径名显示目标文件
//参数 flag：命令行参数     如：-a、-l
//参数 pathname：包含了文件名的路径名
//pathname:只是一个文件的文件名与路径名

void display(int flag, char * pathname)
{
    int i,j;    //j用来存储文件名的字符数
    struct stat buf;
    char name[NAME_MAX + 1];    //name用来存储一个文件名

    //从路径中解析出文件名
    for(i=0, j=0; i<strlen(pathname); i++)
    {
        if(pathname[i] == '/')
        {
            j = 0;
            continue;
        }
        name[j++] = pathname[i];
    }
    name[j] = '\0';     //手动加上结尾0

    //用lstat而不是stat以方便解析链接文件
    //lstat返回的是符号链接文件本身的状态信息
    if(lstat(pathname, &buf) == -1)
    {
        my_err("stat", __LINE__);
    }

    switch (flag)
    {
    case PARAM_NONE:        //没有-l和-a选项
        if(name[0] != '.')  //如果name[0]的值为'.'的话就结束这次的display
        {
            display_single(name);
        }
        break;
    
    case PARAM_A:           //-a：显示包括隐藏文件在内的所有文件
                            //显示隐藏文件的方式是去掉if判断
        display_single(name);
        break;

    case PARAM_L:           //-l：每个文件单独占一行， 显示文件的详细属性信息
        if(name[0] != '.')
        {
            display_attribute(buf, name);
            printf(" %-s\n", name);
        }
        break;

    case PARAM_A + PARAM_L: //同时有-a和-l选项的情况
        display_attribute(buf, name);
        printf(" %-s\n", name);
        break;



    case PARAM_R:
        display_R(buf, flag, pathname);
        break;


    default:
        break;
    }
}

//为显示某个目录下的文件做准备
void display_dir(int flag_param, char * path)
{
    DIR *dir;
    struct dirent *ptr;
    int count = 0;      //存放该目录下文件总数
    char filenames[256][PATH_MAX + 1], temp[PATH_MAX + 1];

    //获取该目录下文件总数和最长文件名
    dir = opendir(path);
    if(dir == NULL)
    {
        my_err("opendir", __LINE__);
    }
    while((ptr = readdir(dir)) != NULL)
    {
        if(g_maxlen < strlen(ptr->d_name))
        {
            g_maxlen = strlen(ptr->d_name);
        }
        count++;
    }
    closedir(dir);

    if(count > 256)
    {
        my_err("too many files under this dir", __LINE__);
    }

    int i, j, len = strlen(path);
    //获取目录下所有 路径和文件名
    dir = opendir(path);
    for(i=0; i<count; i++)
    {
        ptr = readdir(dir);
        if(ptr == NULL)
        {
            my_err("readdir", __LINE__);
        }
        strncpy(filenames[i], path, len);
        filenames[i][len] = '\0';
        strcat(filenames[i], ptr->d_name);
        filenames[i][len + strlen(ptr->d_name)] = '\0';
    }

    //使用冒泡法对文件名进行排序，排序后文件名按字母顺序存储于filenames
    for(i=0; i<count-1; i++)
    {
        for(j=0; j<count-1-i; j++)
        {
            if(strcmp(filenames[j], filenames[j+1]) > 0)
            {
                strcpy(temp, filenames[j+1]);
                temp[strlen(filenames[j+1])] = '\0';
                strcpy(filenames[j+1], filenames[j]);
                filenames[j+1][strlen(filenames[j])] = '\0';
                strcpy(filenames[j], temp);
                filenames[j][strlen(temp)] = '\0';
            }
        }
    }

    //快排
    /*int order[count];
    for(int i=0; i<count; i++)
    {
        order[i] = i;
    }
    qsort_string(filenames, order, 0, count-1);      //段错误
    */

    /*char **dirs = (char **)malloc(sizeof(char **)*MAXFILES);
    for(int i=0; i<MAXFILES; i++)
    {
        dirs[i] = (char *)malloc(PATH_MAX + 1);
    }*/

    //struct stat buf;

    //如果有-R参数就先打印出路径
    if((PARAM_R & flag_param) != 0)
    {
        printf("\n%s\n", path);
    }

    //输出文件名
    for(i=0; i<count; i++)
    {
        display(flag_param, filenames[i]);      //循环给display传入每个文件的参数   包括路径、文件名
    }

    closedir(dir);

    //如果命令行中没有-l选项，打印一个换行符
    if((flag_param & PARAM_L) == 0)
    {
        printf("\n");
    }
}



int main(int argc, char * argv[])
{
    int i, j, k, num;
    char path[PATH_MAX + 1];
    char param[32];     //保存命令行参数，目标文件名和目录名不在此列
    int flag_param = PARAM_NONE;    //参数种类，即是否有-l和-a选项
    struct stat buf;

    //命令行参数的解析，分析-l、-a、-al、-la选项
    j = 0;
    num = 0;
    for(i=1; i<argc; i++)
    {
        if(argv[i][0] == '-')
        {
            for(k=1; k<strlen(argv[i]); k++, j++)
            {
                param[j] = argv[i][k];  //获取-后面的参数保存到数组param中
            }
            num++;      //保存'-'的个数
        }
    }

    //把命令行参数转化为整数存储于param[]中
    for(i=0; i<j; i++)
    {
        if(param[i] == 'a')
        {
            flag_param |= PARAM_A;
            continue;
        }
        else if(param[i] == 'l')
        {
            flag_param |= PARAM_L;
            continue;
        }
        else if(param[i] == 'R')
        {
            flag_param |= PARAM_R;
            continue;
        }
        else
        {
            printf("my_ls: invalid option -%c\n", param[i]);
            exit(1);
        }
    }
    param[j] = '\0';

    //如果没有输入文件名或目录，就显示当前目录
    if((num + 1) == argc)
    {
        strcpy(path, "./");
        path[2] ='\0';
        display_dir(flag_param, path);

        return 0;
    }

    i = 1;
    do
    {
        //如果不是目标文件名或目录，就显示当前目录
        if(argv[i][0] == '-')
        {
            i++;
            continue;
        }
        else
        {
            strcpy(path, argv[i]);

            //如果目标文件或目录不存在，报错并退出程序
            if(stat(path, &buf) == -1)
            {
                my_err("stat", __LINE__);
            }

            if(S_ISDIR(buf.st_mode))
            {
                //argv[i]是一个目录
                //如果目录的最后一个字符不是'/'，就加上'/'
                if(path[strlen(argv[i]) - 1] != '/')
                {
                    path[strlen(argv[i])] = '/';
                    path[strlen(argv[i]) + 1] = '\0';
                }
                else
                {
                    path[strlen(argv[i])] = '\0';
                }

                display_dir(flag_param, path);
                i++;
            }
            else
            {
                //argv[i]是一个文件
                display(flag_param, path);
                i++;
            }
        }
    } while(i<argc);

    return 0;
}