// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************此文件的目的是编译成可执行文件*称为`Read-tree`。当从命令行运行`read-tree`时*以树对象的哈希为参数，例如：*`./读取树c21baf88bb59f4376b242edcd915d82e9fef4913`**注意树对象必须存在于对象存储中，这样才能正常工作。**然后它将打印出该树对象的内容，其中包括：*1)内容的文件模式(文件类型/权限/属性)*被树引用。即，被*承诺。*2)树对象引用的文件的名称。*3)树对象引用的BLOB的哈希。**整个文件(即Main函数中的所有内容)都将运行*当./Read-tree可执行文件从命令行运行时。
*/ 

#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-getenv(Name)：获取环境变量`name`的值。来源：&lt;stdlib.h&gt;。-DB_Environment：常量字符串(通过“cache.h”中的宏定义)，用于指定要设置其路径/名称的环境变量对象存储。-DEFAULT_DB_ENVIRONMENT：常量字符串(通过cache.h中的宏定义)默认路径为。对象存储。-strcMP(s1，S2)：将`s1`指向的字符串与该字符串进行比较由`s2‘指向。-strlen(字符串)：返回`字符串`的长度，单位为字节。-strchr(字符串，c)：返回指向首次出现字符`c`的指针在`字符串‘中。-sscanf(字符串，格式，参数)：使用以下命令将输入从`字符串`读取到`args`输入格式`Format`。来源：&lt;stdio.h&gt;。-print tf(Message，...)：将`Message`写入标准输出流stdout。来源：&lt;stdio.h&gt;。-Usage()：打印错误消息并退出。-sha1_to_hex()：将sha1散列值的20字节表示形式转换为等效的40个字符的十六进制表示形式。-get_sha1_hex()：转换40个字符的十六进制表示形式将SHA1哈希值转换为等效的20字节表示形式。-READ_SHA1_FILE()：在对象数据库中定位对象，读取并给它充气，然后返回膨胀的对象数据(没有预置的元数据)。****************************************************************此源文件中定义了以下变量和函数。-main()：main函数。每次运行./READ-TREE命令时运行。-unpack()：调用READ_SHA1_FILE()函数读取和膨胀树对象存储中的对象，然后将树数据输出到屏幕。
*/ 

/*
 *函数：`unpack`*参数：*-sha1：对象存储中树对象的sha1散列。*用途：调用READ_SHA1_FILE()函数读取和膨胀树*对象存储，然后将树数据输出到*屏幕。
*/ 
static int unpack(unsigned char *sha1)
{
    void *buffer;         /*
 树数据缓冲区。
*/ 
    unsigned long size;   /*
 树对象数据的大小，以字节为单位。
*/ 
    char type[20];        /*
 对象类型。
*/ 

    /*
 *从对象存储中读取散列值为`sha1`的对象，膨胀*它，并返回指向对象数据的指针(不带前缀*元数据)。将对象类型和对象数据大小存储在`type`和*`size`。
*/ 
    buffer = read_sha1_file(sha1, type, &size);

    /*
 如果`Buffer`为空或NULL，则打印用法消息，然后退出。
*/ 
    if (!buffer)
        usage("unable to read sha1 file");

    /*
 *如果散列`sha1`对应的对象为
*/ 
    if (strcmp(type, "tree"))
        usage("expected a 'tree' node");

    /*
 *从树对象数据缓冲区读取有关每个BLOB对象的元数据。
*/ 
    while (size) {
        /*
 计算当前Blob对象的SHA1哈希的偏移量。
*/ 
        int len = strlen(buffer) + 1;           
        /*
 指向当前Blob对象的SHA1哈希。
*/ 
        unsigned char *sha1 = buffer + len;  
        /*
 *指向当前Blob对应的文件路径*反对。
*/ 
        char *path = strchr(buffer, ' ') + 1;   
        unsigned int mode; 
        
        /*
 *验证当前缓冲区大小，获取文件模式*对应于当前的BLOB对象。如果任一项失败，则显示*错误消息，然后退出。
*/ 
        if (size < len + 20 || sscanf(buffer, "%o", &mode) != 1)
            usage("corrupt 'tree' file");

        /*
 *调整缓冲区以指向下一个BLOB对象的*元数据。
*/ 
        buffer = sha1 + 20; 
        /*
 *按读取的元数据的长度减小`size`*当前BLOB对象。
*/ 
        size -= len + 20; 

        /*
 *显示当前对应的文件的模式和路径*BLOB对象，以及当前*Blob对象的SHA1哈希。
*/ 
        printf("%o %s (%s)\n", mode, path, sha1_to_hex(sha1));
    }
    return 0;
}

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`read-tree`从命令行运行。确保正确指挥*语法并调用上面的`unpack()`函数以打印树的*内容到屏幕。
*/ 
int main(int argc, char **argv)
{
    /*
 文件描述符。
*/ 
    int fd; 
    /*
 用于保存哈希值的20字节表示形式的字符串。
*/ 
    unsigned char sha1[20]; 

    /*
 *验证命令行参数个数，应等于*2，因为命令本身也被计算在内。如果没有，请打印用法*消息和退出。
*/ 
    if (argc != 2)
        usage("read-tree <key>");

    /*
 *转换SHA1的给定40个字符的十六进制表示*将散列值转换为等效的20字节表示形式。IF转换*失败(例如，如果十六进制表示法包含字符*超出有效的十六进制范围0-9、a-f或A-F)，打印用法*消息和退出。
*/ 
    if (get_sha1_hex(argv[1], sha1) < 0)
        usage("read-tree <key>");

    /*
 *将`sha1_file_directory`(即对象存储的路径)设置为*`DB_ENVIRONMENT`环境变量的值，默认为*`SHA1_FILE_DIRECTORY`，定义在cache.h中。如果环境*未设置变量(很可能也不会设置)，则getenv()将返回*空指针。
*/ 
    sha1_file_directory = getenv(DB_ENVIRONMENT);

    /*
 *如果没有从上面的环境变量设置对象存储路径，*将其设置为默认值`.dircache/Objects`，即*cache.h中的标记`DEFAULT_DB_ENVIRONMENT`。
*/ 
    if (!sha1_file_directory)
        sha1_file_directory = DEFAULT_DB_ENVIRONMENT;

    /*
 *使用树对象的二进制SHA1散列调用`unpack()`函数*作为函数参数。
*/ 
    if (unpack(sha1) < 0)
        usage("unpack failed");

    return 0;
}
