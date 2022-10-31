// THIS_SOURCES_HAS_BEEN_TRANSLATED 
 /*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************编译此文件的目的是定义帮助器函数*议决。代码库中的其他文件用于与对象存储交互*和指数。这包括用于读取和写入的助手函数*对象存储和索引以及验证现有的CACHE_ENTRIES。
*/ 
#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-MemcMP(str1，str2，n)：将str1的前n个字节与str2进行比较并返回大于、等于或小于0的整数，如果由str1指向的对象大于，等于或小于由指向的对象Str2，分别进行了分析。来源：&lt;string.h&gt;。-active_nr：活动缓存/索引中的条目数。-CACHE_ENTRY：表示单个缓存/暂存文件的结构。-ACTIVE_CACHE：表示活动缓存/索引的数组。-sizeof(数据类型)：给出需要存储的字节数的运算符数据类型或变量。-z_stream：通用压缩/解压缩流。来源：&lt;zlib.h&gt;。-Malloc(Size)：为大小为字节数的对象分配未使用的空间由`size`指定，其值未指定。来源：来自&lt;stdlib.h&gt;。-mmap(addr，len，prot，标志，FILE_DESCRIPTOR，Offset)：建立进程地址空间与文件之间的映射，共享内存对象或类型化内存对象。来源：&lt;sys/mman.h&gt;。-prot_read：mmap()函数的`prot`参数的标志，描述映射所需的内存保护。`PROT_READ`指示可以读取页面。来源：&lt;sys/mman.h&gt;。-map_Private：mmap()函数的`prot`参数的标志，描述映射所需的内存保护。`MAP_PRIVATE`指示对映射数据的更改是调用过程，并且不应更改基础对象。来源：&lt;sys/mman.h&gt;。-SHA_CTX：SHA上下文结构，用于存储对内容进行散列的过程。来源：&lt;openssl/sha.h&gt;。-Close(Fd)：释放文件描述符`fd`。文件描述符`fd`将可供后续调用Open()或其他分配`fd`的函数调用。删除拥有的所有锁`fd`关联的文件上的进程。来源：&lt;unistd.h&gt;。-Memset(void*s，int c，ize_t n)：拷贝`c`(转换为无符号的Char)转换为前几个`N‘字节由`s`指向的对象的。-deflateInit(z_stream，Level)：初始化内部`Z_Stream`状态对于`Level`的压缩，表示上速度变化率压缩的比例范围从0到9。来源&lt;zlib.h&gt;。-deducateBound(z_stream，SourceLen)：返回通货紧缩后的压缩大小`SourceLen`字节。-inflateInit(Z_Stream)：初始化的内部`Z_Stream`状态解压。来源&lt;zlib.h&gt;。-Z_BEST_COMPRESSION：转换为压缩级别9，作为输入如果设置为`apreateInit()`，则表示优化压缩按操作调整大小
*/ 

/*
 用于存储对象存储区的路径。
*/ 
const char *sha1_file_directory = NULL; 
/*
 *一个指针数组，指向表示当前*将缓存到文件或已从*缓存文件。
*/ 
struct cache_entry **active_cache = NULL; 
/*
 ACTIVE_CACHE数组中的缓存条目数。
*/ 
unsigned int active_nr = 0; 
/*
 ACTIVE_CACHE数组可以容纳的最大元素数。
*/ 
unsigned int active_alloc = 0; 

/*
 *函数：`usage`*参数：*-Err：要显示的错误消息。*用途：显示错误消息并退出。
*/ 
void usage(const char *err)
{
    fprintf(stderr, "read-tree: %s\n", err);
    exit(1);
}

/*
 函数：`66val`*参数：*-c：要转换为十进制的十六进制字符。*用途：将十六进制符号转换为等价的十进制符号。
*/ 
static unsigned hexval(char c)
{
    /*
 如果字符是0-9，则返回转换为0-9的整数。
*/ 
    if (c >= '0' && c <= '9')
        return c - '0';

    /*
 如果字符是a-f，则返回转换为10-15的整数。
*/ 
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    /*
 如果字符是A-F，则返回转换为10-15的整数。
*/ 
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    /*
 *如果`c`不是有效的十六进制符号，则返回一的补码*共0。
*/ 
    return ~0;
}

/*
 *函数：`Get_sha1_he`*参数：*-hex：包含SHA1散列的十六进制表示形式的字符串价值。*-sha1：用于存储sha1散列的20字节表示形式的数组*价值。*用途：转换SHA1散列的40个字符的十六进制表示*值设置为等效的20字节表示形式。
*/ 
int get_sha1_hex(char *hex, unsigned char *sha1)
{
    int i;
    /*
 *将每个两位十六进制数(从00到ff)转换为*十进制数(范围从0到255)。
*/ 
    for (i = 0; i < 20; i++) {
        /*
 *第一个十六进制数字的十进制等价物将形成*val的4个高位；第二个十六进制数字的高位将形成*4个低位。
*/ 
        unsigned int val = (hexval(hex[0]) << 4) | hexval(hex[1]);
        /*
 如果val大于255，则返回-1。
*/ 
        if (val & ~0xff)
            return -1;
        *sha1++ = val;   /*
 将val存储在SHA1数组中。
*/ 
        hex += 2;        /*
 获取下一个两位十六进制数。
*/ 
    }
    return 0;
}

/*
 *函数：`sha1_to_he`*参数：*-sha1：包含SHA1散列值的20字节表示形式的数组。*目的：将SHA1哈希值的20字节表示形式转换为*相当于40个字符的十六进制表示法。
*/ 
char *sha1_to_hex(unsigned char *sha1)
{
    /*
 用于存储40个字符的十六进制表示形式的字符串。
*/ 
    static char buffer[50];
    /*
 *用于获取数字的十六进制表示形式的查找数组*从0到15。
*/ 
    static const char hex[] = "0123456789abcdef";
    /*
 用于填充缓冲区字符串的指针。
*/ 
    char *buf = buffer;
    int i;

    /*
 *获取的两位十六进制表示法(范围从00到ff)*一个数字(从0到255)。
*/ 
    for (i = 0; i < 20; i++) {
        unsigned int val = *sha1++;   /*
 获取当前号码。
*/ 
        *buf++ = hex[val >> 4];       /*
 将4位高位转换为十六进制。
*/ 
        *buf++ = hex[val & 0xf];      /*
 将4位低位转换为十六进制。
*/ 
    }
    return buffer;   /*
 返回十六进制表示形式。
*/ 
}

/*
 *莱纳斯·托瓦尔兹：注意！这将返回一个静态分配的缓冲区，因此您*使用时必须小心。如果需要保存文件，请执行“strdup()*文件名。
*/ 

/*
 *函数：`sha1_file_name`*参数：*-sha1：用于标识对象中的对象的sha1哈希值*商店。*用途：在对象数据库中使用*对象的SHA1哈希值。
*/ 
char *sha1_file_name(unsigned char *sha1)
{
    int i;
    /*
 `base‘是一个字符数组，用于存储*对象数据库。`name`是指向`base`中的字节的指针，即*对象数据库路径后加`/`。
*/ 
    static char *name, *base;

    /*
 如果尚未设置基数，则返回。
*/ 
    if (!base) {
        /*
 获取对象数据库的路径。
*/ 
        char *sha1_file_directory 
                 = getenv(DB_ENVIRONMENT) ? : DEFAULT_DB_ENVIRONMENT;
        /*
 路径的长度。
*/ 
        int len = strlen(sha1_file_directory);
        /*
 为基本字符串分配空间。
*/ 
        base = malloc(len + 60);
        /*
 将对象数据库路径复制到基本字符串。
*/ 
        memcpy(base, sha1_file_directory, len);
        /*
 将基本字符串的其余部分初始化为包含空字节。
*/ 
        memset(base+len, 0, 60);
        /*
 在SHA1_FILE_DIRECTORY路径后面写一个斜杠。
*/ 
        base[len] = '/';
        /*
 *写一个斜杠将两个字符的对象目录与*对象文件名。
*/ 
        base[len+3] = '/';
        /*
 将名称设置为指向上面第一个斜杠之后的字节。
*/ 
        name = base + len + 1;
    }
    /*
 *填写对象路径的其余部分(对象目录和文件名)*使用对象的SHA1哈希值。**将SHA1数组中的每个数字(范围从0到255)转换为*两位十六进制数(范围从00到ff)。第一*两位十六进制数将构成对象目录的一部分。*其余两位十六进制数将构成对象*文件名。
*/ 
    for (i = 0; i < 20; i++) {
        /*
 *查找数组，用于获取*从0到15的数字。
*/ 
        static char hex[] = "0123456789abcdef";
        /*
 从SHA1获取当前编号。
*/ 
        unsigned int val = sha1[i];
        /*
 *设置基本数组的索引。这将是名称+0，名称+3，*姓名+5，姓名+7，...，姓名+39。
*/ 
        char *pos = name + i*2 + (i > 0);
        *pos++ = hex[val >> 4];   /*
 将4位高位转换为十六进制。
*/ 
        *pos = hex[val & 0xf];    /*
 将4位低位转换为十六进制。
*/ 
    }
    return base;   /*
 返回对象的路径。
*/ 
}

/*
 *函数：`READ_SHA1_FILE`*参数：*-sha1：对象的sha1哈希值。*-type：读取的对象类型(BLOB、TREE或COMMIT)。*-SIZE：对象数据的字节大小。*用途：在对象数据库中定位对象，读取并膨胀它，然后*返回膨胀后的Object数据(不带前置的元数据)。
*/ 
void *read_sha1_file(unsigned char *sha1, char *type, unsigned long *size)
{
    z_stream stream;     /*
 声明一个zlib z_stream结构。
*/ 
    char buffer[8192];   /*
 Zlib膨胀输出的缓冲区。
*/ 
    struct stat st;      /*
 用于存储文件的`stat`结构
*/ 
    int i;               /*
 
*/ 
    int fd;              /*
 关联的文件描述符。
*/ 
                         /*
 要读取的对象。
*/ 
    int ret;             /*
 充气命令的返回值。
*/ 
    int bytes;           /*
 用于跟踪缓冲区内容的大小。
*/ 
    /*
 *`map`是指向对象映射内容的指针。`buf`是一个指针*到夸大的对象数据。
*/ 
    void *map, *buf;
    /*
 *使用对象的在对象数据库中构建对象的路径*SHA1哈希值。
*/ 
    char *filename = sha1_file_name(sha1); 

    /*
 *在对象存储中打开对象，并关联`fd`。如果*返回值&lt;0，读取文件时出错。
*/ 
    #ifndef BGIT_WINDOWS
    fd = open(filename, O_RDONLY );
    #else
    fd = open(filename, O_RDONLY | O_BINARY );
    #endif
    if (fd < 0) {
        perror(filename);
        return NULL;
    }

    /*
 *获取文件信息，存储在`st`结构中。发布*如果出现错误，则为文件描述符。
*/ 
    if (fstat(fd, &st) < 0) {
        close(fd);
        return NULL;
    }

    /*
 将对象的内容映射到内存。
*/ 
    #ifndef BGIT_WINDOWS
    map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (-1 == (int)(long)map)   /*
 如果mmap失败，则返回NULL。
*/ 
        return NULL;
    #else
    void *fhandle = CreateFileMapping( (HANDLE) _get_osfhandle(fd), NULL, 
                                       PAGE_READONLY, 0, 0, NULL );
    if (!fhandle)
        return NULL;
    map = MapViewOfFile( fhandle, FILE_MAP_READ, 0, 0, st.st_size );
    CloseHandle( fhandle );
    if (map == (void *) NULL)
        return NULL;
    #endif
    close(fd);   /*
 释放文件描述符。
*/ 

    /*
 将zlib流初始化为包含空字符。
*/ 
    memset(&stream, 0, sizeof(stream));
    /*
 将MAP设置为充气流的下一个输入的位置。
*/ 
    stream.next_in = map; 
    /*
 可用作下一次充气输入的字节数。
*/ 
    stream.avail_in = st.st_size; 
    /*
 将`Buffer`设置为写入下一个膨胀输出的位置。
*/ 
    stream.next_out = buffer; 
    /*
 可用于存储下一个膨胀输出的字节数。
*/ 
    stream.avail_out = sizeof(buffer); 

    /*
 初始化流以进行解压缩。
*/ 
    inflateInit(&stream); 
    /*
 解压对象内容，并将返回码存储在`ret`中。
*/ 
    ret = inflate(&stream, 0); 

    /*
 *从缓冲区读取对象数据的对象类型和大小，并*将它们分别存储在类型和大小变量中。如果满足以下条件，则返回NULL*两次转换均未成功。
*/ 
    if (sscanf(buffer, "%10s %lu", type, size) != 2)
        return NULL;

    /*
 *缓冲区大小，直到第一个空字符，即大小前置的元数据加上终止空字符的*。
*/ 
    bytes = strlen(buffer) + 1; 
    /*
 为`buf`分配空间，等于对象数据大小。
*/ 
    buf = malloc(*size); 
    /*
 如果无法分配空间，则出错。
*/ 
    if (!buf)
        return NULL;

    /*
 *将膨胀的对象数据从缓冲区复制到Buf，即不带*前置元数据(对象类型和预期对象数据大小)。
*/ 
    memcpy(buf, buffer + bytes, stream.total_out - bytes);
    /*
 不带前置元数据的膨胀数据的大小。
*/ 
    bytes = stream.total_out - bytes;
    /*
 如果不是所有数据都被夸大了，那就是继续通货膨胀。
*/ 
    if (bytes < *size && ret == Z_OK) {
        stream.next_out = buf + bytes;
        stream.avail_out = *size - bytes;
        while (inflate(&stream, Z_FINISH) == Z_OK)
            /*
 莱纳斯·托瓦尔兹：什么都没有。
*/ ;
    }
    /*
 用于膨胀的空闲内存结构。
*/ 
    inflateEnd(&stream);
    return buf;   /*
 返回膨胀的对象数据。
*/ 
}

/*
 *函数：`WRITE_SHA1_FILE`*参数：*-buf：要放空并写入对象存储的内容。*-len：压缩前内容的长度，单位为字节。*用途：放空对象，计算哈希值，然后调用*WRITE_SHA1_BUFFER函数将放气对象写入*对象数据库。
*/ 
int write_sha1_file(char *buf, unsigned len)
{
    int size;                 /*
 压缩输出的总大小。
*/ 
    char *compressed;         /*
 用于存储压缩输出。
*/ 
    z_stream stream;          /*
 声明zlib z_stream结构。
*/ 
    unsigned char sha1[20];   /*
 用于存储SHA1哈希的数组。
*/ 
    SHA_CTX c;                /*
 声明SHA上下文结构。
*/ 

    /*
 将zlib流初始化为包含空字符。
*/ 
    memset(&stream, 0, sizeof(stream));

    /*
 *初始化压缩流以优化压缩*(相对于速度)。
*/ 
    deflateInit(&stream, Z_BEST_COMPRESSION);
    /*
 确定压缩大小的上限。
*/ 
    size = deflateBound(&stream, len); 
    /*
 分配`size`字节的空间来存储下一个压缩输出。
*/ 
    compressed = malloc(size); 

    /*
 指定buf作为压缩流的下一个输入的位置。
*/ 
    stream.next_in = buf; 
    /*
 可用作下一次压缩输入的字节数。
*/ 
    stream.avail_in = len; 
    /*
 将压缩指定为写入下一个压缩输出的位置。
*/ 
    stream.next_out = compressed; 
    /*
 可用于存储下一个压缩输出的字节数。
*/ 
    stream.avail_out = size; 

    /*
 压缩buf的内容，即压缩对象。
*/ 
    while (deflate(&stream, Z_FINISH) == Z_OK) 
    /*
 莱纳斯·托瓦尔兹：什么都没有。
*/ ;

    /*
 *动态分配的空闲内存结构*压缩。
*/ 
    deflateEnd(&stream); 
    /*
 获取总压缩输出的大小。
*/ 
    size = stream.total_out; 

    /*
 初始化SHA上下文结构。
*/ 
    SHA1_Init(&c); 
    /*
 计算压缩输出的哈希。
*/ 
    SHA1_Update(&c, compressed, size); 
    /*
 将压缩输出的sha1哈希存储在`sha1`中。
*/ 
    SHA1_Final(sha1, &c); 

    /*
 将压缩对象写入对象存储区。
*/ 
    if (write_sha1_buffer(sha1, compressed, size) < 0)
        return -1;
    /*
 *显示对象的40字符十六进制表示形式*SHA1哈希值。
*/ 
    printf("%s\n", sha1_to_hex(sha1));
    return 0;
}

/*
 *函数：`WRITE_SHA1_Buffer`*参数：*-sha1：要写入*对象存储。*-buf：要写入对象存储的内容。*-Size：要写入对象存储的内容的大小。*用途：使用对象的SHA1将对象写入对象数据库*哈希值作为索引。
*/ 
int write_sha1_buffer(unsigned char *sha1, void *buf, unsigned int size)
{
    /*
 *使用对象的在对象数据库中构建对象的路径*SHA1哈希。
*/ 
    char *filename = sha1_file_name(sha1);
    int i;    /*
 未使用的变量。即使是莱纳斯·托瓦尔兹也会犯错。
*/ 
    int fd;   /*
 要写入的文件的文件描述符。
*/ 

    /*
 在对象存储中打开一个新文件，并将其与`fd`关联。
*/ 
    fd = OPEN_FILE(filename, O_WRONLY | O_CREAT | O_EXCL, 0666);

    /*
 如果打开文件时出现故障，则出错。
*/ 
    if (fd < 0)
        return (errno == EEXIST) ? 0 : -1;

    write(fd, buf, size);   /*
 将对象写入对象存储区。
*/ 
    close(fd);              /*
 释放文件描述符。
*/ 
    return 0;
}

/*
 *函数：`error`*参数：*-字符串：要打印的错误消息。*用途：将错误消息打印到标准错误流。
*/ 
static int error(const char * string)
{
    fprintf(stderr, "error: %s\n", string);
    return -1;
}

/*
 *函数：`Verify_Header`*参数：*-hdr：指向要验证的缓存头结构的指针。*-SIZE：缓存文件的字节大小。*用途：验证CACHE_HEADER。
*/ 
static int verify_hdr(struct cache_header *hdr, unsigned long size)
{
    SHA_CTX c;                /*
 声明SHA上下文。
*/ 
    unsigned char sha1[20];   /*
 用于存储SHA1哈希的数组。
*/ 

    /*
 *确保CACHE_HEADER的签名匹配中定义的值*“cache.h”。
*/ 
    if (hdr->signature != CACHE_SIGNATURE)
        return error("bad signature");

    /*
 确保
*/ 
    if (hdr->version != 1)
        return error("bad version");

    /*
 
*/ 
    SHA1_Init(&c); 

    /*
 计算缓存头和缓存条目的哈希值。
*/  
    SHA1_Update(&c, hdr, offsetof(struct cache_header, sha1));
    SHA1_Update(&c, hdr+1, size - sizeof(*hdr));
    SHA1_Final(sha1, &c);

    /*
 *将上面计算的SHA1散列与存储在*缓存头。如果它们匹配，则缓存有效。
*/ 
    if (memcmp(sha1, hdr->sha1, 20))
        return error("bad header sha1");
    return 0;
}

/*
 *函数：`Read_cache`*参数：无*用途：将`.dircache/index`文件中的缓存项读取到8个`active_cache`数组。
*/ 
int read_cache(void)
{
    int fd;           /*
 文件描述符。
*/ 
    int  i;           /*
 对于循环迭代变量。
*/ 
    struct stat st;   /*
 用于存储文件信息的`stat`结构。
*/ 
    unsigned long size, offset;
    /*
 *用于存储要在其中映射*`.dircache/index`缓存文件。
*/ 
    void *map; 
    /*
 声明一个指向缓存头的指针，如“cache.h”中所定义。
*/ 
    struct cache_header *hdr; 

    /*
 检查ACTIVE_CACHE阵列是否已填充。
*/ 
    errno = EBUSY;
    if (active_cache) 
        return error("more than one cachefile");

    /*
 *通过首先检查是否存在以下内容来获取对象存储的路径*存储在`DB_ENVIRONMENT`环境变量中。如果不是，请使用*`DEFAULT_DB_ENVIRONMENT`中指定的默认路径，即*`.dircache/Objects`。然后检查是否可以访问该目录。
*/ 
    errno = ENOENT;
    sha1_file_directory = getenv(DB_ENVIRONMENT);
    if (!sha1_file_directory)
        sha1_file_directory = DEFAULT_DB_ENVIRONMENT;
    if (access(sha1_file_directory, X_OK) < 0)
        return error("no access to SHA1 file directory");

    /*
 *打开`.dircache/index`缓存文件，关联`fd`*文件描述符(仅为整数)。
*/ 
    #ifndef BGIT_WINDOWS
    fd = open(".dircache/index", O_RDONLY );
    #else
    fd = open(".dircache/index", O_RDONLY | O_BINARY );
    #endif
    /*
 *如果文件不存在或打开时出错，则返回*文件。
*/ 
    if (fd < 0)
        return (errno == ENOENT) ? 0 : error("open failed");

    /*
 *我不得不查一下这个`(void*)-1`是什么意思。显然，这是一种*对始终无效的内存位置的引用，该位置不会*如果操作成功，可以退回。
*/ 
    #ifndef BGIT_WINDOWS
    map = (void *)-1;
    #else
    map = (void *) NULL;
    #endif

    /*
 *获取缓存文件信息，存储在`st`统计结构中。*如果`fstat()`命令返回0，即为，则执行`if`块*成功。
*/ 
    if (!fstat(fd, &st)) {
        map = NULL;

        /*
 *设置`size`等于`st`结构的`st_size`成员。*`.dircache/index`文件的大小，单位为字节。
*/ 
        size = st.st_size;

        /*
 *预置返回无效参数的错误代码*出现错误。
*/ 
        errno = EINVAL; 

        /*
 *检查以确保返回的索引文件大小更大*大于`缓存_头`结构的大小。这一定是真的*用于有效的缓存，因为它必须由缓存头和*至少一个缓存条目。
*/ 
        if (size > sizeof(struct cache_header)) {
            /*
 *将`.dircache/index`缓存文件的内容映射到内存*并返回指向该空间的指针。了解更多有关*`mmap()`函数，参见：**http://pubs.opengroup.org/onlinepubs/009695399/functions/*mmap.html*https://stackoverflow.com/questions/258091/*何时应使用mmap进行文件访问。
*/ 
            #ifndef BGIT_WINDOWS
            map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
            #else
            void *fhandle 
                = CreateFileMapping( (HANDLE) _get_osfhandle(fd), 
                                     NULL, PAGE_READONLY, 0, 0, NULL );
            if (!fhandle)
                return error("CreateFileMapping failed");
            map = MapViewOfFile( fhandle, FILE_MAP_READ, 0, 0, size );
            CloseHandle( fhandle );
            #endif
        }
    }

    /*
 *取消分配文件描述符`fd`，以便将来使用*调用`open()`。例如，如果fd=1，则文件描述符1将*被解除分配，并可以引用文件。
*/ 
    close(fd);

    /*
 *如果-1\f25`mmap()`-1\f6映射失败，则显示错误信息并返回*文件到内存。
*/ 
    #ifndef BGIT_WINDOWS
    if (-1 == (int)(long)map)
        return error("mmap failed");
    #else
    if (map == (void *) NULL)
        return error("MapViewOfFile failed");
    #endif

    /*
 *设置`hdr`缓存头指针指向所在的内存地址*映射了`.dircache/index`文件的内容。然后打电话给*`verify_hdr()`验证缓存头。如果标头不是*有效，代码跳转到此文件末尾的`unmap`标签。
*/ 
    hdr = map;
    if (verify_hdr(hdr, size) < 0)
        goto unmap;

    /*
 缓存中的缓存条目数。
*/ 
    active_nr = hdr->entries; 
    /*
 ACTIVE_CACHE数组可以容纳的最大元素数。
*/ 
    active_alloc = alloc_nr(active_nr); 

    /*
 *为`active_cache`数组分配内存。内存被分配给*一个由`active_alLoc`数量的元素组成的数组，每个元素的大小*为`缓存_条目`结构。
*/ 
    active_cache = calloc(active_alloc, sizeof(struct cache_entry *));

    /*
 *`offset`是要读取的`map`的下一个字节的索引。在这种情况下，*将其设置为报头后第一个缓存条目的开头。
*/ 
    offset = sizeof(*hdr); 

    /*
 *将每个缓存条目添加到`active_cache`数组中，并增加*当前缓存条目大小的`offset`索引..。
*/ 
    for (i = 0; i < hdr->entries; i++) {
        struct cache_entry *ce = map + offset;
        offset = offset + ce_size(ce);
        active_cache[i] = ce;
    }
    
    /*
 返回缓存中的缓存条目数。
*/ 
    return active_nr;

/*
 *仅当缓存设置为*标头无效。在这种情况下，缓存文件和*移除内存以防止内存泄漏。然后显示一条错误消息*并返回-1。
*/ 
unmap:
    #ifndef BGIT_WINDOWS
    munmap(map, size);
    #else
    UnmapViewOfFile( map );
    #endif
    errno = EINVAL;
    return error("verify header failed");
}

