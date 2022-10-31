// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************本文件的目的是定义和包含所需的*库、函数签名、。和默认设置为Git`.c`*程序要发挥作用。此文件&lt;cache.h&gt;包含在*同一(根)目录下的`.c`文件，包括：**cat-file.c、Commit-tree.c、init-DB.c、Read-cache.c、Read-tree.c、*show-Diff.c、update-cache.c、WRITE-tree.c。
*/ 

/*
 *仅当`CACHE_H`尚未执行时才运行此文件中的代码*尚未定义。这是为了防止多次编译*代码相同，因为包含在多个`.c`文件中。
*/ 
#ifndef CACHE_H 
#define CACHE_H         /*
 定义令牌`CACHE_H`。
*/ 

#include <stdio.h>      /*
 定义输入/输出工具的标准C库。
*/ 
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>   /*
 定义`stat`工具的标准C库。
*/ 
#include <fcntl.h>      /*
 用于处理文件的标准C库。
*/ 
#include <stddef.h>     /*
 用于类型定义的标准C库。
*/ 
#include <stdlib.h>     /*
 库定义的标准C库。
*/ 
#include <stdarg.h>     /*
 变量参数列表的标准C库。
*/ 
#include <errno.h>      /*
 系统错误号的标准C库。
*/ 

#ifndef BGIT_WINDOWS
    #include <sys/mman.h>   /*
 用于内存管理的标准C库。
*/ 
                            /*
 申报单。
*/ 
#else
    #include <windows.h>
    #include <lmcons.h>
    #include <direct.h>
    #include <io.h>
#endif

#include <openssl/sha.h>   /*
 包括OpenSSL库中SHA散列工具。
*/ 
#include <zlib.h>          /*
 包括来自zlib库压缩工具。
*/ 

/*
 *Linus Torvalds：目录缓存的基本数据结构。**莱纳斯·托瓦尔兹：注意，注意！这都是本机CPU字节格式。*它甚至没有试图成为便携设备。这是在努力提高效率。它是*毕竟只是一个缓存。
*/ 

#ifdef BGIT_UNIX
    #define STAT_TIME_SEC( st, st_xtim ) ( (st)->st_xtim ## e )
    #define STAT_TIME_NSEC( st, st_xtim ) ( (st)->st_xtim.tv_nsec )

#elif defined BGIT_DARWIN
    #define STAT_TIME_SEC( st, st_xtim ) ( (st)->st_xtim ## espec.tv_sec )
    #define STAT_TIME_NSEC( st, st_xtim ) ( (st)->st_xtim ## espec.tv_nsec )

#elif defined BGIT_WINDOWS
    #define STAT_TIME_SEC( st, st_xtim ) ( (st)->st_xtim ## e )
    #define STAT_TIME_NSEC( st, st_xtim ) 0
#endif

#ifndef BGIT_WINDOWS
    #define OPEN_FILE( fname, flags, mode ) open( fname, flags, mode );
#else
    #define OPEN_FILE( fname, flags, mode ) open( fname, flags | O_BINARY, \
                                                      mode );
#endif

/*
 此`CACHE_SIGNAURE`被硬编码为加载到所有缓存头中。
*/ 
#define CACHE_SIGNATURE 0x44495243   /*
 莱纳斯·托瓦尔兹：《DIRC》。
*/ 

/*
 标识一组缓存条目的标头结构模板。
*/ 
struct cache_header {
    /*
 所有标头上的常量，以验证真实性。
*/ 
    unsigned int signature; 
    /*
 存储创建缓存的Git版本。
*/ 
    unsigned int version; 
    /*
 缓存中的缓存条目数。
*/ 
    unsigned int entries; 
    /*
 标识缓存的SHA1哈希。
*/ 
    unsigned char sha1[20]; 
};

/*
 *用于存储所采取行动的时间戳的时间结构模板*与缓存条目对应的文件。例如，文件的时间*已修改。有关文件时间的一些信息，请参阅：*https://www.quora.com/What-is-the-difference-between-mtime-atime-and-ctime。
*/ 
struct cache_time {
    unsigned int sec;
    unsigned int nsec;
};

/*
 *缓存条目结构的模板，存储有关*工作目录下对应的用户文件。
*/ 
struct cache_entry {
    struct cache_time ctime;   /*
 文件上次状态更改的时间。
*/ 
    struct cache_time mtime;   /*
 上次修改文件的时间。
*/ 
    unsigned int st_dev;       /*
 包含文件的设备的设备ID。
*/ 

    /*
 *文件序列号，区分此文件与所有文件*同一设备上的其他文件。
*/ 
    unsigned int st_ino;

    /*
 *指定文件的模式。这包括有关*文件类型和权限。
*/ 
    unsigned int st_mode;
    unsigned int st_uid;      /*
 文件所有者的用户ID。
*/ 
    unsigned int st_gid;      /*
 文件的组ID。
*/ 
    unsigned int st_size;     /*
 常规文件的大小，以字节为单位。
*/ 
    unsigned char sha1[20];   /*
 放气的Blob对象的SHA1哈希。
*/ 
    unsigned short namelen;   /*
 文件名或路径长度。
*/ 
    unsigned char name[0];    /*
 文件名或路径。
*/ 
};

/*
 *以下是外部变量的声明。它们在中定义*源代码Read-cache.c..。
*/ 

/*
 对象存储区的路径。
*/ 
extern const char *sha1_file_directory;
/*
 指向缓存条目的指针数组。
*/ 
extern struct cache_entry **active_cache;
/*
 `active_cache`数组中的条目数。
*/ 
extern unsigned int active_nr;
/*
 ACTIVE_CACHE数组可以容纳的最大元素数。
*/ 
extern unsigned int active_alloc;

/*
 *如果需要，您可以使用环境变量设置指向*对象存储。
*/ 
#define DB_ENVIRONMENT "SHA1_FILE_DIRECTORY"

/*
 *对象存储的默认路径。
*/ 
#define DEFAULT_DB_ENVIRONMENT ".dircache/objects"

/*
 *这些宏用于计算要分配给缓存的大小*进入。
*/ 
#define cache_entry_size(len) ((offsetof(struct cache_entry, name) \
                                + (len) + 8) & ~7)
#define ce_size(ce) cache_entry_size((ce)->namelen)

/*
 *有关此宏的详细信息，请参阅此链接：*https://stackoverflow.com/questions/22090101/*why-is-define-alloc-nrx-x163-2-macro-used-in-many-cache-h-files。
*/ 
#define alloc_nr(x) (((x)+16)*3/2)

/*
 *以下是函数原型。它们在源文件中定义*Read-cache.c.。
*/ 

/*
 *将`.dircache/index`文件的内容读入`active_cache`*数组。
*/ 
extern int read_cache(void);

/*
 *Linus Torvalds：返回与SHA1匹配的静态分配的文件名*签名。
*/ 
extern char *sha1_file_name(unsigned char *sha1);

/*
 Linus Torvalds：将内存缓冲区写出到SHA1文件。
*/ 
extern int write_sha1_buffer(unsigned char *sha1, void *buf, 
                             unsigned int size);

/*
 *Linus Torvalds：将SHA1文件读取并解压缩到内存中，将内存写入*SHA1文件。
*/ 
extern void *read_sha1_file(unsigned char *sha1, char *type, 
                            unsigned long *size);
extern int write_sha1_file(char *buf, unsigned len);

/*
 Linus Torvalds：转换为十六进制/SHA1表示法/从十六进制/SHA1表示法转换为十六进制/SHA1表示法。
*/ 
extern int get_sha1_hex(char *hex, unsigned char *sha1);
/*
 莱纳斯·托瓦尔兹：静态缓冲！
*/ 
extern char *sha1_to_hex(unsigned char *sha1);

/*
 将使用情况消息打印到标准错误流。
*/ 
extern void usage(const char *err);

#endif /*
 莱纳斯·托瓦尔兹：CACHE_H
*/ 
