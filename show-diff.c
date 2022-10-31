// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************此文件的目的是编译成可执行文件*称为`show-diff`。当从命令行运行`show-diff`时*它不需要任何命令行参数。**`show-diff`命令用于显示*索引中暂存的文件以及这些文件的当前版本*因为它们存在于文件系统中。**此文件中Main函数中的所有内容都将运行*当./show-diff可执行文件从命令行运行时。
*/ 

#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-Snprint tf(char*s，Size_t n，const char*Format，...)：将格式化字符串`Format`写入`s`，其中`N`指定应写入`s`的最大字符数，包括空的终止字符。-Popen(命令，模式)：在调用程序和命令并返回指向流的指针，启用对命令的读或写访问权限取决于模式的值。来源：&lt;stdio.h&gt;。-fWRITE(data，size，items，stream)：最多写入`nitems`，每个文件的大小`size`，从数组`data`到`stream`。来源：&lt;stdio.h&gt;。-pClose(Stream)：关闭Popen()打开的流。来源：&lt;stdio.h&gt;。-READ_CACHE()：将`.dircache/index`文件的内容读入`active_cache`数组。-print tf(Message，...)：将`Message`写入标准输出流stdout。来源：&lt;stdio.h&gt;。-Free(Ptr)：释放`ptr`指向的空间。来源：&lt;stdlib.h&gt;。
*/ 

#define MTIME_CHANGED   0x0001
#define CTIME_CHANGED   0x0002
#define OWNER_CHANGED   0x0004
#define MODE_CHANGED    0x0008
#define INODE_CHANGED   0x0010
#define DATA_CHANGED    0x0020

/*
 *函数：`Match_stat`*参数：*-ce：指向缓存条目结构的指针。*-st：指向包含正在处理的元数据的统计结构的指针*与缓存条目对应的文件。*目的：将存储在缓存条目中的元数据与*对应的工作文件检查是否相同或是否相同*任何事情都改变了。
*/ 
static int match_stat(struct cache_entry *ce, struct stat *st)
{
    /*
 用于指示哪些文件元数据已更改(如果有)的标志。
*/ 
    unsigned int changed = 0;

    /*
 *将存储在缓存条目中的元数据与相应的*工作文件以检查它们是否相同。
*/ 

    /*
 检查上次修改时间。
*/ 
    if (ce->mtime.sec  != (unsigned int)STAT_TIME_SEC( st, st_mtim ) ||
        ce->mtime.nsec != (unsigned int)STAT_TIME_NSEC( st, st_mtim ))
            changed |= MTIME_CHANGED;
    /*
 检查上次状态更改的时间。
*/ 
    if (ce->ctime.sec  != (unsigned int)STAT_TIME_SEC( st, st_ctim ) ||
        ce->ctime.nsec != (unsigned int)STAT_TIME_NSEC( st, st_ctim ))
            changed |= CTIME_CHANGED;

    /*
 检查文件用户ID和组ID。
*/ 
    if (ce->st_uid != (unsigned int)st->st_uid ||
        ce->st_gid != (unsigned int)st->st_gid)
        changed |= OWNER_CHANGED;
    /*
 检查文件模式。
*/ 
    if (ce->st_mode != (unsigned int)st->st_mode)
        changed |= MODE_CHANGED;
    #ifndef BGIT_WINDOWS
    /*
 检查设备ID和文件索引节点号。
*/ 
    if (ce->st_dev != (unsigned int)st->st_dev ||
        ce->st_ino != (unsigned int)st->st_ino)
        changed |= INODE_CHANGED;
    #endif
    /*
 检查文件大小。
*/ 
    if (ce->st_size != (unsigned int)st->st_size)
        changed |= DATA_CHANGED;
    return changed;
}

/*
 *函数：`show_Differences`*参数：*-ce：指向缓存条目结构的指针。*-cur：指向包含正在处理的元数据的状态结构的指针*与缓存条目对应的文件。*-old_content：缓存条目对应的BLOB数据。*-old_Size：BLOB数据的大小，单位为字节。*用途：使用diff外壳命令显示*与缓存条目对应的BLOB数据和*相应的工作文件。
*/ 
static void show_differences(struct cache_entry *ce, struct stat *cur,
                             void *old_contents, unsigned long long old_size)
{
    static char cmd[1000];   /*
 用于存储diff命令的字符串。
*/ 
    FILE *f;                 /*
 声明一个文件指针。
*/ 

    /*
 *为该缓存条目构造diff命令，该命令将用于*显示对应的BLOB数据差异*缓存条目和相应工作文件的内容。*将命令字符串存储在`cmd`中。
*/ 
    snprintf(cmd, sizeof(cmd), "diff --strip-trailing-cr -u - %s", ce->name);

    /*
 *创建指向diff命令的管道，并返回指向*对应的写入流。
*/ 
    f = popen(cmd, "w");

    /*
 *将当前缓存条目对应的BLOB对象数据写入*完成的命令流
*/ 
    fwrite(old_contents, old_size, 1, f);

    /*
 关闭命令流。
*/ 
    pclose(f);
}

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`show-Diff`从命令行运行。
*/ 
int main(int argc, char **argv)
{
    /*
 *将`.dircache/index`文件的内容读取到*`active_cache`数组，返回缓存条目数。
*/ 
    int entries = read_cache();

    /*
 用于循环计数器。
*/ 
    int i;

    /*
 *如果读取缓存时出错，则显示一条错误消息并*退出。
*/ 
    if (entries < 0) {
        perror("read_cache");
        exit(1);
    }

    /*
 循环访问ACTIVE_CACHE数组中的缓存条目。
*/ 
    for (i = 0; i < entries; i++) {
        /*
 声明一个Stat结构来存储文件元数据。
*/ 
        struct stat st;
        /*
 当前缓存条目。
*/ 
        struct cache_entry *ce = active_cache[i];
        /*
 用于循环计数器。
*/ 
        int n;
        /*
 用于指示哪些文件元数据已更改(如果有)的标志。
*/ 
        int changed;
        /*
 没有用过。
*/ 
        unsigned int mode;
        /*
 Blob对象数据大小。
*/ 
        unsigned long size;
        /*
 用于存储对象类型(本例中为BLOB)。
*/ 
        char type[20];
        /*
 用于存储Blob对象数据。
*/ 
        void *new;

        /*
 *使用stat()函数获取有关工作的信息*与当前缓存条目对应的文件，并将其存储在*`st`统计结构。如果stat()调用失败，则显示错误*消息并继续到下一个缓存条目。
*/ 
        if (stat(ce->name, &st) < 0) {
            printf("%s: %s\n", ce->name, strerror(errno));
            continue;
        }

        /*
 *将存储在缓存条目中的元数据与*对应的工作文件检查是否相同或是否相同*任何事情都改变了。
*/ 
        changed = match_stat(ce, &st);

        /*
 *如果元数据没有更改，则显示OK消息并继续到*ACTIVE_CACHE数组中的下一个缓存条目。
*/ 
        if (!changed) {
            printf("%s: ok\n", ce->name);
            continue;
        }

        /*
 如果有任何元数据更改，请跳过此处。
*/ 

        /*
 *显示当前缓存对应的文件路径*进入。
*/ 
        printf("%.*s:  ", ce->namelen, ce->name);

        /*
 *显示Blob的SHA1散列的十六进制表示形式*当前缓存条目对应的对象。
*/ 
        for (n = 0; n < 20; n++)
            printf("%02x", ce->sha1[n]);

        printf("\n");   /*
 打印换行符。
*/ 

        /*
 *使用BLOB对象的SHA1散列从对象存储读取BLOB对象，*使其膨胀，并返回指向对象数据的指针(不带*预置的元数据)。将对象类型和对象数据大小存储在*`type`和`size`。
*/ 
        new = read_sha1_file(ce->sha1, type, &size);

        /*
 *使用diff外壳命令显示*当前缓存条目和内容对应的BLOB数据*相应的工作文件。
*/ 
        show_differences(ce, &st, new, size);

        /*
 释放`new`指向的空间。
*/ 
        free(new);
    }
    return 0;
}
