// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.*****************************************************************************此文件的目的是编译成可执行文件*称为`更新缓存`。此命令相当于`git*add`在当前版本的Git中。当运行`update-cache`时*在命令行中，它将使用名称作为参数*要添加到索引中的文件(或Torvalds先生所称的文件*在自述文件中，存储了*默认情况下在.dircache/index文件中。这是可以想象的*作为准备提交更改的“中转区”*积累起来。**此文件中的`main`函数将在./更新-缓存时运行*可执行文件从命令行运行。
*/ 

#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-MemcMP(str1，str2，n)：将str1的前n个字节与str2进行比较并返回大于、等于或小于0的整数，如果由str1指向的对象大于，等于或小于由指向的对象Str2，分别进行了分析。来源：&lt;string.h&gt;。-active_nr：活动缓存/索引中的条目数。-CACHE_ENTRY：表示单个缓存/暂存文件的结构。-ACTIVE_CACHE：表示活动缓存/索引的数组。-MemMove(str1，str2，N)：从str2指向的对象复制n个字节指向str1指向的对象。-sizeof(数据类型)：给出需要存储的字节数的运算符数据类型或变量。-z_stream：通用压缩/解压缩流。来源：&lt;zlib.h&gt;。-Malloc(Size)：为大小为字节数的对象分配未使用的空间由`size`指定，其值未指定。来源：来自&lt;stdlib.h&gt;。-mmap(addr，len，prot，标志，FILE_DESCRIPTOR，Offset)：建立进程地址空间与文件之间的映射，共享内存对象或类型化内存对象。来源：&lt;sys/mman.h&gt;。-prot_read：mmap()函数的`prot`参数的标志，描述映射所需的内存保护。`PROT_READ`指示可以读取页面。来源：&lt;sys/mman.h&gt;。-MAP_PRIVATE：mmap()函数参数的标志，描述映射所需的内存保护。`MAP_PRIVATE`指示对映射数据的更改是调用过程，并且不应更改基础对象。来源：&lt;sys/mman.h&gt;。-SHA_CTX：SHA上下文结构，用于存储对内容进行散列的过程。来源：&lt;openssl/sha.h&gt;。-Close(Fd)：释放文件描述符`fd`。文件描述符`fd`将可供后续调用Open()或其他分配`fd`的函数调用。删除拥有的所有锁`fd`关联的文件上的进程。来源：&lt;unistd.h&gt;。-Memset(void*s，int c，ize_t n)：拷贝`c`(转换为无符号的Char)转换为前几个`N‘字节由`s`指向的对象的。-deflateInit(z_stream，Level)：初始化内部`Z_Stream`状态对于`Level`的压缩，表示上的速度与压缩的比例范围从0到9。来自&lt;zlib.h&gt;。-Z_BEST_COMPRESSION：Transla
*/ 

#ifndef BGIT_WINDOWS
    #define RENAME( src_file, target_file ) rename( src_file, target_file )
    #define RENAME_FAIL -1 
#else
    #define RENAME( src_file, target_file ) MoveFileEx( src_file, \
                                                target_file, \
                                                MOVEFILE_REPLACE_EXISTING )
    #define RENAME_FAIL 0 
#endif

/*
 *函数：`缓存名称_比较`*参数：*-name1：要比较的第一个文件的名称。*-len1：名称1的长度。*-name2：要比较的第二个文件的名称。*-len2：名称2的长度。*目的：按词典顺序比较两个缓存条目的名称。
*/ 
static int cache_name_compare(const char *name1, int len1, const char *name2, 
                              int len2)
{
    int len = len1 < len2 ? len1 : len2;   /*
 Len是较短的长度。
*/ 
    int cmp;

    cmp = memcmp(name1, name2, len);
    if (cmp)           /*
 第一个镜头的角色是不同的。
*/ 
        return cmp;
    if (len1 < len2)   /*
 第一个镜头字符是相同的。
*/ 
        return -1;
    if (len1 > len2)   /*
 第一个镜头字符是相同的。
*/ 
        return 1;
    return 0;          /*
 完全匹配。
*/ 
}

/*
 *函数：`缓存名称_位置`*参数：*-name：需要缓存的文件路径。*-namelen：路径的长度。*目的：确定缓存条目在*ACTIVE_CACHE阵列。
*/ 
static int cache_name_pos(const char *name, int namelen)
{
    /*
 声明并初始化二进制搜索的索引。
*/ 
    int first, last;
    first = 0;
    last = active_nr;

    /*
 *执行二进制搜索以确定*ACTIVE_CACHE数组中的缓存条目。
*/ 
    while (last > first) {
        int next = (last + first) >> 1;   /*
 除以2。
*/ 
        struct cache_entry *ce = active_cache[next];
        int cmp = cache_name_compare(name, namelen, ce->name, ce->namelen);
        if (!cmp)            /*
 找到了完全匹配的。
*/ 
            return -next-1;
        if (cmp < 0) {
            last = next;
            continue;
        }
        first = next+1;
    }
    return first;
}

/*
 *函数：`REMOVE_FILE_FROM_cache`*参数：*-Path：要从活动缓存中删除的文件的路径/文件名。*用途：从ACTIVE_CACHE数组中删除文件的缓存项。
*/ 
static int remove_file_from_cache(char *path)
{
    int pos = cache_name_pos(path, strlen(path));
    if (pos < 0) {   /*
 如果找到完全匹配的话。
*/ 
        pos = -pos-1;
        active_nr--;
        if (pos < active_nr)
            memmove(active_cache + pos, active_cache + pos + 1, 
                    (active_nr - pos - 1) * sizeof(struct cache_entry *));
    }
}

/*
 *功能：`添加缓存进入`*参数：*-ce：要添加到`active_cache`数组中的缓存条目。*用途：在`active_cache`数组中插入缓存条目*按词典顺序排列。
*/ 
static int add_cache_entry(struct cache_entry *ce)
{
    /*
 *获取缓存条目将插入到*ACTIVE_CACHE阵列。
*/ 
    int pos;   
    pos = cache_name_pos(ce->name, ce->namelen);

    /*
 莱纳斯·托瓦尔兹：现有的对手？只要把它换掉就行了。
*/ 
    if (pos < 0) {
        active_cache[-pos-1] = ce;
        return 0;
    }

    /*
 *确保`active_cache`阵列有空间容纳额外的缓存*进入。
*/ 
    if (active_nr == active_alloc) {
        active_alloc = alloc_nr(active_alloc);
        active_cache = realloc(active_cache, 
                               active_alloc * sizeof(struct cache_entry *));
    }

    /*
 将新的缓存条目插入到active_cache数组中。
*/ 
    active_nr++;
    if (active_nr > pos)
        memmove(active_cache + pos + 1, active_cache + pos, 
                (active_nr - pos - 1) * sizeof(ce));
    active_cache[pos] = ce;
    return 0;
}

/*
 *函数：`index_fd`*参数：*-Path：要添加到对象存储的文件的路径。*-namelen：要添加的路径/文件名的长度。*-ce：文件对应的缓存条目结构。*-fd：需要添加的文件关联的文件描述符。*-st：包含要添加的文件信息的`stat`对象。*用途：构造一个BLOB对象，压缩，计算的SHA1散列*压缩的BLOB对象，然后将该BLOB对象写入*对象数据库。
*/  
static int index_fd(const char *path, int namelen, struct cache_entry *ce, 
                    int fd, struct stat *st)
{
    /*
 声明zlib z_stream结构。
*/ 
    z_stream stream;
    /*
 分配给下一个压缩输出的字节数。
*/ 
    int max_out_bytes = namelen + st->st_size + 200; 
    /*
 分配`max_out_bytes`空间来存储下一个压缩输出。
*/ 
    void *out = malloc(max_out_bytes);
    /*
 分配空间以存储文件元数据。
*/ 
    void *metadata = malloc(namelen + 200);

    /*
 将要缓存的文件内容映射到内存。
*/ 
    #ifndef BGIT_WINDOWS
    void *in = mmap(NULL, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    #else
    void *fhandle = CreateFileMapping( (HANDLE) _get_osfhandle(fd), NULL, 
                                       PAGE_READONLY, 0, 0, NULL );
    if (!fhandle)
        return -1;

    void *in = MapViewOfFile( fhandle, FILE_MAP_READ, 0, 0, st->st_size );
    CloseHandle( fhandle );
    #endif

    /*
 声明SHA上下文结构。
*/ 
    SHA_CTX c;

    /*
 释放文件描述符`fd`，因为我们不再需要它。
*/ 
    close(fd);

    #ifndef BGIT_WINDOWS
    /*
 `out`或`in`内存分配失败，返回-1。
*/ 
    if (!out || (int)(long)in == -1)
        return -1;
    #else
    if (!out || in == (void *) NULL)
        return -1;
    #endif

    /*
 将zlib流初始化为包含空字符。
*/ 
    memset(&stream, 0, sizeof(stream));

    /*
 *初始化压缩流以优化压缩*(相对于速度)。
*/ 
    deflateInit(&stream, Z_BEST_COMPRESSION);

    /*
 *Linus Torvalds：ASCII大小+NUL字节。
*/     
    stream.next_in = metadata;   /*
 将文件元数据设置为第一个添加。
*/ 
                                 /*
 传输到压缩流输入。
*/ 
    /*
 *将`BLOB`写入`metadata`数组，后跟*正在将文件添加到缓存。将可用字节数设置为*下一次压缩的输入等于字符数*写入+1(用于终止空字符)。
*/ 
    stream.avail_in = 1 + sprintf(metadata, "blob %lu", 
                                  (unsigned long) st->st_size);
    /*
 指定`out`作为写入下一个压缩输出的位置。
*/ 
    stream.next_out = out;
    /*
 可用于存储下一个压缩输出的字节数。
*/ 
    stream.avail_out = max_out_bytes;

    /*
 压缩数据，到目前为止只是文件元数据。
*/ 
    while (deflate(&stream, 0) == Z_OK)
        /*
 莱纳斯·托瓦尔兹：什么都没有。
*/ ;

    /*
 将文件内容添加到压缩流输入。
*/ 
    stream.next_in = in;
    stream.avail_in = st->st_size;

    /*
 压缩文件内容。
*/ 
    while (deflate(&stream, Z_FINISH) == Z_OK)
        /*
 莱纳斯·托瓦尔兹：什么都没有。
*/ ;

    /*
 用于压缩的空闲数据结构。
*/ 
    deflateEnd(&stream);
    /*
 初始化`c`SHA上下文结构。
*/ 
    SHA1_Init(&c);
    /*
 *计算压缩输出的哈希值，总大小为*`Stream.Total_out`。
*/ 
    SHA1_Update(&c, out, stream.total_out); 
    /*
 *将压缩输出的SHA1散列存储在缓存条目的*`sha1`成员。
*/ 
    SHA1_Final(ce->sha1, &c);

    /*
 *将BLOB对象写入对象存储并随返回一起返回*WRITE_SHA1_BUFFER函数的值。
*/ 
    return write_sha1_buffer(ce->sha1, out, stream.total_out); 
}

/*
 *函数：`添加_文件_到_缓存`*参数：*-Path：要添加到对象存储和索引的文件的路径。*用途：获取要添加到缓存的文件的信息，存储该文件*缓存_条目结构中的元数据，然后调用`index_fd()`*构造BLOB对象并将其写入对象的函数*商店、。以及用于插入缓存的`add_cache_entry()`函数*按字典顺序进入`active_cache`数组。
*/ 
static int add_file_to_cache(char *path)
{
    int size, namelen;
    /*
 用于引用缓存条目。
*/ 
    struct cache_entry *ce; 
    /*
 *用于存储通过`fstat()`函数获取的文件信息*呼叫。
*/ 
    struct stat st;
    /*
 要添加到缓存的文件的文件描述符。
*/ 
    int fd;

    /*
 打开要添加到缓存的文件并返回文件描述符。
*/ 
    fd = open(path, O_RDONLY); 

    /*
 *如果`open()`命令失败，则返回-1。删除相应的*如果文件不存在，则缓存active_cache数组中的条目
*/ 
    if (fd < 0) {
        if (errno == ENOENT)
            return remove_file_from_cache(path);
        return -1;
    }

    /*
 
*/ 
    if (fstat(fd, &st) < 0) {
        close(fd);
        return -1;
    }

    /*
 获取文件路径字符串的长度。
*/ 
    namelen = strlen(path); 
    /*
 以字节为单位计算要分配给缓存条目的大小。
*/ 
    size = cache_entry_size(namelen); 
    /*
 为缓存条目分配`size`字节。
*/ 
    ce = malloc(size); 
    /*
 初始化缓存条目以包含空字符。
*/ 
    memset(ce, 0, size); 
    /*
 将`path`复制到缓存条目的`name`成员中。
*/ 
    memcpy(ce->name, path, namelen); 

    /*
 *将通过fstat()调用获取的文件元数据复制到缓存*进入结构成员。
*/ 
    ce->ctime.sec = STAT_TIME_SEC( &st, st_ctim );
    ce->ctime.nsec = STAT_TIME_NSEC( &st, st_ctim );
    ce->mtime.sec = STAT_TIME_SEC( &st, st_mtim );
    ce->mtime.nsec = STAT_TIME_NSEC( &st, st_mtim );
    ce->st_dev = st.st_dev;
    ce->st_ino = st.st_ino;
    ce->st_mode = st.st_mode;
    ce->st_uid = st.st_uid;
    ce->st_gid = st.st_gid;
    ce->st_size = st.st_size;
    ce->namelen = namelen;

    /*
 *调用index_fd()函数构造BLOB对象，压缩，*计算压缩的BLOB对象的SHA1散列，然后将*BLOB对象复制到对象数据库。
*/ 
    if (index_fd(path, namelen, ce, fd, &st) < 0)
        return -1;

    /*
 *按词典顺序将缓存条目插入active_cache数组*然后返回`add_cache_entry`。
*/ 
    return add_cache_entry(ce);
}

/*
 *函数：`WRITE_cache`*参数：*-newfd：与索引锁文件关联的文件描述符。*-CACHE：指向要写入的缓存条目结构的指针数组*索引锁文件。*-条目：`active_cache`数组中的缓存条目数。*用途：构造缓存头，计算缓存的SHA1哈希值*`active_cache`数组中的头部和缓存条目，和*然后将它们写入`.dircache/index.lock`文件。
*/ 
static int write_cache(int newfd, struct cache_entry **cache, int entries)
{
    SHA_CTX c;                 /*
 声明SHA上下文结构。
*/ 
    struct cache_header hdr;   /*
 声明CACHE_HEADER结构。
*/ 
    int i;                     /*
 For循环迭代器。
*/ 

    /*
 将其设置为“cache.h”中定义的签名。
*/ 
    hdr.signature = CACHE_SIGNATURE; 
    /*
 在此版本中，版本始终设置为1。
*/ 
    hdr.version = 1; 
    /*
 *将`active_cache`数组中的缓存条目数存储在*缓存头。
*/ 
    hdr.entries = entries; 

    /*
 初始化`c`SHA上下文结构。
*/ 
    SHA1_Init(&c); 
    /*
 使用缓存头更新正在运行的SHA1哈希计算。
*/ 
    SHA1_Update(&c, &hdr, offsetof(struct cache_header, sha1));
    /*
 使用每个缓存条目更新正在运行的SHA1哈希计算。
*/ 
    for (i = 0; i < entries; i++) {
        struct cache_entry *ce = cache[i];
        int size = ce_size(ce);
        SHA1_Update(&c, ce, size);
    }
    /*
 将最终的SHA1散列存储在标头中。
*/ 
    SHA1_Final(hdr.sha1, &c);

    /*
 将缓存头写入索引锁文件。
*/ 
    if (write(newfd, &hdr, sizeof(hdr)) != sizeof(hdr))
        return -1;

    /*
 将每个缓存条目写入索引锁文件。
*/ 
    for (i = 0; i < entries; i++) {
        struct cache_entry *ce = cache[i];
        int size = ce_size(ce);
        if (write(newfd, ce, size) != size)
            return -1;
    }
    return 0;
}        

/*
 *莱纳斯·托瓦尔兹：我们根本不喜欢一些道路：我们不想*任何地方都是点或点点的，事实上，我们甚至不想要任何其他的*点文件(.dircache或任何其他文件)。看在上帝的份上，它们是隐藏的。**莱纳斯·托瓦尔兹：还有，我们不想要双斜杠或末尾的斜杠*这可能会使路径名模棱两可。
*/ 
static int verify_path(char *path)
{
    char c;

    goto inside;
    for (;;) {
        if (!c)
            return 1;
        if (c == '/') {
inside:
            c = *path++;
            if (c != '/' && c != '.' && c != '\0')
                continue;
            return 0;
        }
        c = *path++;
    }
}

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`update-cache`从命令行运行。
*/ 
int main(int argc, char **argv)
{
    int i;         /*
 下面是`for‘循环的迭代器。
*/ 
    int newfd;     /*
 引用索引锁文件的文件描述符。
*/ 
    int entries;   /*
 缓存中的条目数，由返回。
*/ 
                   /*
 读取缓存()。
*/ 

    /*
 缓存文件的名称。
*/ 
    char cache_file[]      = ".dircache/index";
    /*
 缓存锁定文件的名称。
*/ 
    char cache_lock_file[] = ".dircache/index.lock"; 

    /*
 *将`.dircache/index`文件的内容读入*`active_cache`数组，返回缓存条目数。显示一个*条目数&lt;0的错误信息，表示*读取缓存时出错，然后返回-1。
*/ 
    entries = read_cache();
    if (entries < 0) {
        perror("cache corrupted");
        return -1;
    }

    /*
 *创建并打开一个名为`.dircache/index.lock`的新缓存锁文件，并*返回文件描述符进行引用。如果出现以下情况，则显示错误消息*OPEN()命令返回值&lt;0表示失败，然后返回*-1.。
*/ 
    newfd = OPEN_FILE(cache_lock_file, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (newfd < 0) {
        perror("unable to create new cachefile");
        return -1;
    }

    /*
 *循环要添加到缓存的文件，其路径或文件名为*作为命令行参数传入：**./更新-缓存路径1路径2...。
*/ 
    for (i = 1 ; i < argc; i++) {
        /*
 存储作为命令行参数传递的第i个路径。
*/ 
        char *path = argv[i];

        /*
 *验证路径。如果路径无效，请继续下一步*文件。
*/ 
        if (!verify_path(path)) {
            fprintf(stderr, "Ignoring path %s\n", argv[i]);
            continue;
        }

        /*
 *这将调用`add_file_to_cache()`，它做了几件事：*1)打开`path`处的文件。*2)获取文件信息并存储文件*CACHE_ENTRY结构中的元数据。*3)调用index_fd()函数构造对应的*BLOB对象并将其写入对象数据库。*4)呼叫。函数的作用是：插入缓存*按词典顺序进入ACTIVE_CACHE数组。**如果这些步骤中的任何一个导致非零返回代码(即失败)，*跳转到下面的`out`标签。
*/ 
        if (add_file_to_cache(path)) {
            fprintf(stderr, "Unable to add %s to database\n", path);
            goto out;
        }
    }

    /*
 *这也做了几件事：*1)调用`WRITE_CACHE()`设置缓存头，计算*对标头和缓存条目进行SHA1散列，然后写入*整个缓存到索引锁文件。*2)将`.dircache/index.lock`文件重命名为`.dircache/index`。
*/ 
    if (!write_cache(newfd, active_cache, active_nr)) {
        close(newfd);
        if (RENAME(cache_lock_file, cache_file) != RENAME_FAIL) {
            return 0;
        }
    }

/*
 解除`.dircache/index.lock`文件的链接。
*/ 
out:
    close(newfd);
    #ifndef BGIT_WINDOWS
    unlink(cache_lock_file);
    #else
    _unlink(cache_lock_file);
    #endif
}
