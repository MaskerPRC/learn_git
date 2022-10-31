// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************此文件的目的是编译成可执行文件*调用`init-db`。当从命令行运行`init-db`时*它将通过创建对象来初始化Git存储库*store(默认目录名为`.dircache/Objects`)*它将存储用户提交的内容，以便*跟踪一段时间内存储库的历史记录。**整个文件(即Main函数中的所有内容)都将运行*当./init-db可执行文件从命令行运行时。
*/ 

#include "cache.h"

#ifndef BGIT_WINDOWS
    #define MKDIR( path ) ( mkdir( path, 0700 ) )
#else
    #define MKDIR( path ) ( _mkdir( path ) )
#endif

/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-getenv(Name)：获取环境变量`name`的值。来源：&lt;stdlib.h&gt;。-DB_Environment：常量字符串(通过“cache.h”中的宏定义)，用于指定要设置其路径/名称的环境变量对象存储。-mkdir(名称，模式)：创建一个派生权限的新目录`name`来自`mode`。来源：&lt;sys/stat.h&gt;。-perror(Message)：将`Message`写入标准错误流。来源：&lt;stdio.h&gt;。-Exit(Status)：停止程序执行，退出，代码为`status`。来源：&lt;stdlib.h&gt;。-stat：结构指针，由stat()函数用来存储信息与文件系统文件相关。来源：&lt;sys/stat.h&gt;。-stat(name，buf)：获取文件`name`的信息，并存储在由`buf`指向的区域，它是指向`stat`结构。-S_ISDIR(MODE)：根据文件的‘mode`。来源：&lt;sys/stat.h&gt;。-fprint tf(stream，Message，...)：将`Message`写入输出`stream`。来源：&lt;stdio.h&gt;。-stderr：标准错误流。来源：&lt;stdio.h&gt;。-DEFAULT_DB_ENVIRONMENT：常量字符串(通过cache.h中的宏定义)使用对象存储区的默认路径。-strlen(字符串)：返回`字符串`的长度，单位为字节。-errno：上次错误的错误号。来源：&lt;errno.h&gt;。-EEXIST：错误宏指示现有文件是在上下文中，只有指定新文件才有意义。来源：来自&lt;errno.h&gt;。-Malloc(Size)：为大小为字节数的对象分配未使用的空间由`size`指定，其值未指定。来源：来自&lt;stdlib.h&gt;。-Memcpy(s1，s2，n)：将s2指向的对象中的n个字节复制到由S1指向的对象。来源：&lt;string.h&gt;。-print intf(s，Message，...)：将`Message`字符串常量写入字符串变量`s`，后跟空字符‘\0’。来源：&lt;stdio.h&gt;。****************************************************************此源文件中定义了以下变量和函数：-Main(ARGC，Argv)：每次运行init-db时运行的主函数命令正在运行。-argc：执行时提供的命令行参数个数初始化数据库。-argv：包含命令行参数字符串的数组。-sha1_dir：对象存储的路径。-Path：用于构建每个路径的动态字符串子目录，即，每个目录对应一个子目录十六进制中介于0和255之间的数字。每个人的名字子目录将表示SHA1散列的前两位存储的对象的
*/ 

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`init-db`从命令行运行。
*/ 
int main(int argc, char **argv)
{
    /*
 *下面变量的`char*`格式允许将它们用作*字符串，而不是只包含一个字符。*只需将这些视为字符串。
*/ 
    char *sha1_dir, *path;

    /*
 声明三个稍后要使用的整数。
*/ 
    int len, i, fd;

    /*
 *尝试在当前目录中创建名为`.dircache`的目录*目录。如果失败，`mkdir()`将返回-1，程序将*打印错误消息并退出。
*/ 
    if (MKDIR(".dircache") < 0) {
        perror("unable to create .dircache");
        exit(1);
    }
    
    /*
 *将`sha1_dir`(即对象存储的路径)设置为*`DB_ENVIRONMENT`环境变量，默认为*`SHA1_FILE_DIRECTORY`，定义在cache.h中。如果环境*未定义变量(很可能不会定义)，则getenv()将*返回空指针。
*/ 
    sha1_dir = getenv(DB_ENVIRONMENT);

    /*
 *仅当`sha1_dir`不为空时才执行此代码块，即*如果定义了上面的环境变量。
*/ 
    if (sha1_dir) {
        struct stat st;
        if (!(stat(sha1_dir, &st) < 0) && S_ISDIR(st.st_mode))
            return 1;
        fprintf(stderr, "DB_ENVIRONMENT set to bad directory %s: ", sha1_dir);
    }

    /*
 *如果未指定自定义对象存储路径或*无效。
*/ 

    /*
 *将`sha1_dir`设置为中定义的默认值`.dircache/Objects`*“cache.h”，然后在屏幕上打印一条消息传达这一点。
*/ 
    sha1_dir = DEFAULT_DB_ENVIRONMENT;
    fprintf(stderr, "defaulting to private storage area\n");

    /*
 *将`len`设置为`sha1_dir`中字符串的长度，即字符串`.dircache/Objects`的*。这将在以后用来构建*对象数据库中的子目录，其中哈希索引的对象将*保存。
*/ 
    len = strlen(sha1_dir);

    /*
 *尝试在`.dircache`内创建一个名为`objects`的目录。如果*失败，`mkdir()`将返回`-1`，程序将打印*消息和退出。
*/ 
    if (MKDIR(sha1_dir) < 0) {
        if (errno != EEXIST) {
            perror(sha1_dir);
            exit(1);
        }
    }

    /*
 *为`path`分配空间，大小为len`(`sha1_dir`的字节大小)++40字节。
*/ 
    path = malloc(len + 40);

    /*
 将`sha1_dir`复制到`路径`。
*/ 
    memcpy(path, sha1_dir, len);

    /*
 *执行此循环256次，以在*`.目录缓存/对象/`目录。子目录将命名为`00`*到`ff`，它们是数字0到*255。每个子目录将用于保存其SHA1散列的对象*十六进制表示法中的值以这两位数字开头。
*/ 
    for (i = 0; i < 256; i++) {
        /*
 *将`i`转换为两位十六进制数，并将其追加到*`.dircache/Objects/`部分后的路径变量。这样一来，每个人*在循环过程中，我们构建以下路径之一：*`.目录缓存/对象/00`，`.目录缓存/对象/01`，...，*`.dircache/对象/fe`、`.dircache/对象/ff`。
*/ 
        sprintf(path+len, "/%02x", i);

        /*
 *尝试创建当前子目录。如果失败，则返回`mkdir()`*将返回-1，程序将打印一条消息并退出。
*/ 
        if (MKDIR(path) < 0) {
            if (errno != EEXIST) {
                perror(path);
                exit(1);
            }
        }
    }
    return 0;
}
