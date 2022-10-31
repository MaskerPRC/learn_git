// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************此文件的目的是编译成可执行文件*称为`WRITE-TREE`。当从命令行运行`WRITE-Tree`时*它不需要任何命令行参数。**`write-tree`命令接受已在中暂存的更改*索引，并在对象存储记录中创建树对象*这些变化。**此文件中Main函数中的所有内容都将运行*当./WRITE-TREE可执行文件从命令行运行时。
*/ 

#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-sha1_file_name()：在对象库中构建对象的路径使用对象的SHA1哈希值。-Access(Path，Amode)：检查`path`处的文件是否可以访问根据`amode`中指定的权限。-perror(Message)：将`Message`写入标准错误输出流。来源：来自&lt;stdio.h&gt;。-READ_CACHE()：将`.dircache/index`文件的内容读入`active_cache`数组。缓存条目的数量为回来了。-fprint tf(stream，Message，...)：将`Message`写入输出`stream`。来源：&lt;stdio.h&gt;。-Exit(Status)：停止程序执行，退出，代码为`status`。来源：&lt;stdlib.h&gt;。-Malloc(Size)：为大小为字节数的对象分配未使用的空间由`size`指定，其值未指定。来源：来自&lt;stdlib.h&gt;。-alloc_nr(X)：这是“cache.h”中的一个宏，用于计算分配给ACTIVE_CACHE的最大元素数数组。-realloc(指针，大小)：更新所指向的内存对象的大小`pointer`到`size`。来源：&lt;stdlib.h&gt;。-print intf(s，Message，...)：将`Message`字符串常量写入字符串变量`s`，后跟空字符‘\0’。来源：&lt;stdio.h&gt;。-Memcpy(s1，s2，n)：将s2指向的对象中的n个字节复制到由S1指向的对象。-WRITE_SHA1_FILE()：放空对象，计算哈希值，然后打电话给WRITE_SHA1_BUFFER函数用于写入泄气的对象添加到对象数据库。****************************************************************下列变量和函数在中定义。这个源文件。-main()：Main函数在每次运行./WRITE-TREE命令时运行。-CHECK_VALID_SHA1()：检查用户提供的SHA1散列是否与对象数据库中的。-prpend_INTEGER()：添加包含大小的小数形式的字符串以字节为单位的树数据发送到缓冲区。-原创_。偏移量：定义为对象标记分配的缓冲区和对象数据大小。
*/ 

/*
 *函数：`check_valid_sha1`*参数：*-sha1：要检查的sha1哈希。*目的：检查用户提供的SHA1散列是否与*对象数据库，以及进程是否对其具有读取访问权限。
*/ 
static int check_valid_sha1(unsigned char *sha1)
{
    /*
 *使用对象的在对象数据库中构建对象的路径*SHA1哈希值。
*/ 
    char *filename = sha1_file_name(sha1);
    int ret;   /*
 返回代码。
*/ 

    /*
 *检查进程是否对对象中的对象有读访问权限*数据库。
*/ 
    ret = access(filename, R_OK);

    /*
 如果文件不可访问，则出错。
*/ 
    if (ret)
        perror(filename);

    return ret;
}

/*
 *函数：`Prepend_Integer`*参数：*-Buffer：指向保存的缓冲区的指针
*/ 
static int prepend_integer(char *buffer, unsigned val, int i)
{
    /*
 在缓冲区中的树数据前附加一个空字符。
*/ 
    buffer[--i] = '\0';

    /*
 *添加一个包含树大小小数形式的字符串*空字符之前的以字节为单位的数据。
*/ 
    do {
        buffer[--i] = '0' + (val % 10);
        val /= 10;
    } while (val);
    /*
 *`i`的值现在是包含的缓冲区元素的索引*树数据大小的小数形式的最高有效位。
*/ 
    return i;
}

/*
 Linus Torvalds：有足够的空间添加“tree&lt;Size&gt;\0”的标题。
*/ 
#define ORIG_OFFSET (40)

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`WRITE-Tree`从命令行运行。
*/ 
int main(int argc, char **argv)
{
    /*
 要分配给缓冲区的大小。
*/ 
    unsigned long size;
    /*
 接下来要填充的缓冲区元素的索引。
*/ 
    unsigned long offset;
    /*
 没有用过。即使是莱纳斯·托瓦尔兹也会犯错。
*/ 
    unsigned long val;
    /*
 For循环中使用的迭代器。
*/ 
    int i;
    /*
 *将`.dircache/index`文件的内容读入*`active_cache`数组。返回缓存条目的数量，并且*存储在`条目`中。
*/ 
    int entries = read_cache();

    /*
 用于保存树内容的字符串。
*/ 
    char *buffer;

    /*
 *如果没有活动的缓存条目或读取错误*缓存，显示一条错误消息并退出，因为没有什么可以*写信给一棵树。
*/ 
    if (entries <= 0) {
        fprintf(stderr, "No file-cache to create a tree of\n");
        exit(1);
    }

    /*
 莱纳斯·托瓦尔兹：猜测初始尺寸。
*/ 
    size = entries * 40 + 400;
    /*
 为缓存分配`size`字节，用于存储树内容。
*/ 
    buffer = malloc(size);
    /*
 *使用此文件中定义的宏来设置偏移量索引。那棵树*数据将从该偏移量开始写入。树元数据将*写在它之前。
*/ 
    offset = ORIG_OFFSET;

    /*
 *循环遍历每个缓存条目并通过添加*从缓存条目到缓冲区的数据。
*/ 
    for (i = 0; i < entries; i++) {
        /*
 从ACTIVE_CACHE数组中选择第i个缓存条目。
*/ 
        struct cache_entry *ce = active_cache[i];

        /*
 检查缓存条目的SHA1散列是否有效。否则，退出。
*/ 
        if (check_valid_sha1(ce->sha1) < 0)
            exit(1);

        /*
 如果需要，增加缓冲区的大小。
*/ 
        if (offset + ce->namelen + 60 > size) {
            size = alloc_nr(offset + ce->namelen + 60);
            buffer = realloc(buffer, size);
        }

        /*
 *将缓存条目的文件模式和名称写入缓冲区，并*按写入的字符数递增`offset`。
*/ 
        offset += sprintf(buffer + offset, "%o %s", ce->st_mode, ce->name);

        /*
 *将空字符作为分隔符和增量写入缓冲区*`Offset`。
*/ 
        buffer[offset++] = 0;

        /*
 将缓存条目的SHA1散列添加到缓冲区。
*/ 
        memcpy(buffer + offset, ce->sha1, 20);

        /*
 *将偏移量增加20个字节，即SHA1散列的长度。
*/ 
        offset += 20;
    }

    /*
 *添加一个包含树大小小数形式的字符串*以字节为单位的数据存储到缓冲区。
*/ 
    i = prepend_integer(buffer, offset - ORIG_OFFSET, ORIG_OFFSET);
    /*
 *将字符串`tree`添加到缓冲区，以将此对象标识为*对象存储中的树。
*/ 
    i -= 5;
    memcpy(buffer+i, "tree ", 5);

    /*
 *调整缓冲区，从`tree`对象的第一个字符开始*标签。
*/ 
    buffer += i;
    /*
 计算此缓冲区的最终总大小。
*/ 
    offset -= i;

    /*
 压缩缓冲区内容，计算压缩后的SHA1哈希值*输出，并将树对象写入对象存储。
*/ 
    write_sha1_file(buffer, offset);

    /*
 回报成功。
*/ 
    return 0;
}
