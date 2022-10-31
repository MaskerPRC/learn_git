// THIS_SOURCES_HAS_BEEN_TRANSLATED 
/*
 *除非明确说明，否则所有文档(备注)：**版权所有2018，AnalytixBar LLC，Jacob Stopak**所有代码和明确标记的文档(注释)：**版权所有2005年，Linus Torvalds**这个程序是自由软件；您可以重新分发它和/或*根据GNU通用公共许可证的条款进行修改*自由软件基金会发布的版本2。**这个程序的分发是希望它能有用，*但没有任何保证；甚至没有*适销性或对某一特定目的的适用性。请参阅*GNU通用公共许可证，了解更多详细信息。**您应已收到GNU通用公共许可证的副本*与这项计划一起；如果不是，请参阅&lt;http://www.gnu.org/licenses/&gt;.****************************************************************************此文件的目的是编译成可执行文件*名为`Commit-tree`。该命令的语法为：**提交树&lt;sha1&gt;[-p&lt;sha1&gt;]**当从命令行运行`Commit-tree`时*至少1个命令行参数，它是*要提交的树。如果这是唯一提供的参数，则为git*将假定您在*存储库，即没有父级的提交。如果`-p&lt;sha1&gt;`*被指定为第二个参数，则该SHA1散列应该*对应即将创建的新提交的父提交*已创建。从技术上讲，在多提交合并中，多个*可以传入父提交SHA1哈希。**`Commit-tree`命令创建一个新的提交对象*在对象存储中，如果没有，则作为初始提交*指定了父母，或作为与父对象的新提交*由第二个参数指定。**此文件中Main函数中的所有内容都将运行*当./Commit-tree可执行文件从命令行运行时。**注意：如果`Commit-tree`命令似乎挂起*在命令行按Enter键后，它不是。它是*等待您键入提交消息，然后按*CTRL-D。这会将提交消息添加到新的*创建提交并将其写入对象存储。
*/ 

#include "cache.h"
/*
 上面的‘Include’允许使用以下函数和“cache.h”头文件中的变量，按第一次使用的顺序排序在这份文件中。大多数是标准C库中的函数/宏它们是“cache.h”中的`#includded`。函数名后面跟有圆括号，而变量/结构名称不是：-Malloc(Size)：为大小为字节数的对象分配未使用的空间由`size`指定，其值未指定。来源：来自&lt;stdlib.h&gt;。-Memset(void*s，int c，ize_t n)：拷贝`c`(转换为无符号的Char)转换为前几个`N‘字节由`s`指向的对象的。-va_start(va_list ap，last)：在带有可变参数列表的函数中，此宏将变量`ap`初始化为指向后的第一个未命名参数`last`命名参数。-vSnprint tf(char*s，Size_t n，const char*Format，va_list arg)：使用变量参数列表`arg`构造`Format`，并写入将格式化后的字符串转换为`s‘，其中`N‘指定最大数量应写入`s`的字符的数量，包括空值终止字符。-va_end(Va_List Ap)：对va_list对象执行清理的Marco通过调用va_start()进行了初始化。-realloc(指针，大小)：更新所指向的内存对象的大小`pointer`到`size`。来源：&lt;stdlib.h&gt;。-Memcpy(s1，s2，N)：将s2指向的对象中的n个字节复制到由S1指向的对象。-strlen(字符串)：返回`字符串`的长度，单位为字节。-get_sha1_hex()：转换40个字符的十六进制表示形式将SHA1哈希值转换为等效的20字节表示形式。-Usage()：打印错误消息并退出。-strcMP(s1，S2)：将`s1`指向的字符串与该字符串进行比较由`s2‘指向。-fprint tf(流，消息，...)：在输出`Stream`中写入`Message`。来源：&lt;stdio.h&gt;。-getuid()：返回调用进程的真实用户ID。-getpwuid(Uid)：返回指向包含信息的结构的指针关于用户ID为`uid`的用户。-gethostname(name，namelen)：获取在其上运行函数
*/ 

#ifndef BGIT_WINDOWS
/*
 *包括包含passwd结构模板的头文件*用于存储用户的登录帐户信息。
*/ 
#include <pwd.h>
#endif
/*
 *包括头文件，该文件包含的tm结构的模板*存储数据和时间信息。
*/ 
#include <time.h>

/*
 32768字节，分配给缓冲区的初始内存大小。
*/ 
#define BLOCKING (1ul << 15) 
/*
 *定义缓冲区开始处的字节数*为对象标签和对象数据大小分配。
*/ 
#define ORIG_OFFSET (40) 

/*
 *函数：`init_Buffer`*参数：*-bufp：指向要分配和初始化的缓冲区的指针的指针。*-sizep：指向缓冲区已填充部分大小的指针。*用途：分配空间给将包含*提交数据。
*/ 
static void init_buffer(char **bufp, unsigned int *sizep)
{
    /*
 分配要缓冲的内存。
*/ 
    char *buf = malloc(BLOCKING); 
    /*
 将空字节复制到缓冲区的前40个字节。
*/ 
    memset(buf, 0, ORIG_OFFSET); 
    /*
 缓冲区填充部分的大小。
*/ 
    *sizep = ORIG_OFFSET; 
    /*
 将提交对象缓冲区设置为已分配的空间。
*/ 
    *bufp = buf; 
}

/*
 *函数：`Add_Buffer`*参数：*-bufp：指向提交对象缓冲区的指针。*-sizep：指向缓冲区已填充部分大小的指针。*-fmt：要写入缓冲区的格式化字符串。*用途：将一项提交对象数据添加到缓冲区。
*/ 
static void add_buffer(char **bufp, unsigned int *sizep, const char *fmt, ...)
{
    char one_line[2048];                /*
 要添加到缓冲区的字符串。
*/ 
    va_list args;                       /*
 变量参数列表。
*/ 
    int len;                            /*
 One_line中的字符串长度。
*/ 
    unsigned long alloc, size, newsize; /*
 用于跟踪缓冲区大小的变量。
*/ 
    char *buf;                          /*
 本地缓冲区。
*/ 

    /*
 初始化args以指向fmt后的第一个未命名参数。
*/ 
    va_start(args, fmt); 

    /*
 *使用变量参数列表参数构造FMT字符串并写入*将字符串设置为one_line。
*/ 
    len = vsnprintf(one_line, sizeof(one_line), fmt, args);

    /*
 清理args变量列表对象。
*/ 
    va_end(args); 
    /*
 提交对象缓冲区已填充部分的当前大小。
*/ 
    size = *sizep; 
    /*
 将One_line的长度与已填充的缓冲区部分的大小相加。
*/ 
    newsize = size + len; 
    /*
 *计算当前最大缓冲区大小。应该是的倍数*32768字节。
*/ 
    alloc = (size + 32767) & ~32767; 
    /*
 将本地指针设置为指向提交对象缓冲区。
*/ 
    buf = *bufp; 

    /*
 *如果填充部分的预期大小为*缓冲区大于计算的最大缓冲区大小。
*/ 
    if (newsize > alloc) {
        /*
 *计算新的最大缓冲区大小。应该是32768的倍数*字节。
*/ 
        alloc = (newsize + 32767) & ~32767; 
        /*
 增加缓冲区大小。
*/ 
        buf = realloc(buf, alloc); 
        /*
 将提交对象缓冲区设置为重新分配的缓冲区。
*/ 
        *bufp = buf; 
    }
    /*
 提交对象缓冲区已填充部分的新大小。
*/ 
    *sizep = newsize; 
    /*
 *在提交对象的填充部分后追加one_line*缓冲。
*/ 
    memcpy(buf + size, one_line, len); 
}

/*
 *函数：`Prepend_Integer`*参数：*-Buffer：指向保存提交数据的缓冲区的指针。*-val：提交数据的字节大小。*-i：`Buffer`开头分配的字节数*表示对象标记和对象数据大小。*用途：添加一个字符串，该字符串包含*将数据以字节为单位提交到缓冲区。
*/ 
static int prepend_integer(char *buffer, unsigned val, int i)
{
    /*
 在缓冲区中的提交数据前附加一个空字符。
*/ 
    buffer[--i] = '\0'; 

    /*
 *添加一个包含提交大小的十进制形式的字符串*空字符之前的以字节为单位的数据。
*/ 
    do {
        buffer[--i] = '0' + (val % 10);
        val /= 10;
    } while (val);
    /*
 *`i`的值现在是包含的缓冲区元素的索引*提交数据大小的小数形式的最高有效位。
*/ 
    return i;
}

/*
 *函数：`Finish_Buffer`*参数：*-tag：要添加到缓冲区的带有对象标签的字符串。*-bufp：指向提交对象缓冲区的指针。*-sizep：指向缓冲区已填充部分大小的指针。*目的：调用Prepend_Integer()函数以预置*将对象数据提交到缓冲区，然后预先提交*对象标记添加到缓冲区。
*/ 
static void finish_buffer(char *tag, char **bufp, unsigned int *sizep)
{
    /*
 包含对象标记的字符串的长度。
*/ 
    int taglen; 
    /*
 接下来要填充的缓冲区元素的索引。
*/ 
    int offset; 
    /*
 将本地指针设置为指向提交对象缓冲区。
*/ 
    char *buf = *bufp; 
    /*
 提交对象缓冲区已填充部分的当前大小。
*/ 
    unsigned int size = *sizep; 

    /*
 将提交对象数据的大小(以字节为单位)附加到缓冲区。
*/ 
    offset = prepend_integer(buf, size - ORIG_OFFSET, ORIG_OFFSET);
    /*
 获取包含对象标记的字符串的长度。
*/ 
    taglen = strlen(tag); 
    /*
 将偏移量索引减去对象标记字符串的长度。
*/ 
    offset -= taglen; 
    /*
 *指向缓冲区中对象的第一个字符所在的字节*将写入标签字符串。
*/ 
    buf += offset; 
    /*
 *计算缓冲区填充部分的最终大小，即*提交对象。
*/ 
    size -= offset; 
    /*
 将包含提交对象标记的字符串附加到缓冲区。
*/ 
    memcpy(buf, tag, taglen); 

    /*
 *调整缓冲区，从`Commi`对象的第一个字符开始*标签。
*/ 
    *bufp = buf; 
    
    /*
 提交对象的最终大小。
*/ 
    *sizep = size; 
}

/*
 *函数：`REMOVE_SPECIAL`*参数：*-p：要从中删除特殊字符的字符串。*用途：从字符串中删除换行符和尖括号。
*/ 
static void remove_special(char *p)
{
    char c; 
    char *dst = p;   /*
 初始化指针。
*/ 

    /*
 连续循环，直到BREAK语句。
*/ 
    for (;;) {
        c = *p;   /*
 将`c`设置为`p`指向的当前字符。
*/ 
        p++;      /*
 递增指向下一个字符的指针。
*/ 

        /*
 如果字符是换行符或尖括号，则忽略它。
*/ 
        switch(c) {
            case '\n': case '<': case '>':
                continue;
        }

        /*
 将当前字符添加到字符串。
*/ 
        *dst++ = c;

        /*
 如果字符为空，则退出for循环。
*/ 
        if (!c)
            break;
    }
}

/*
 *莱纳斯·托瓦尔兹：拥有超过
*/ 
#define MAXPARENT (16)

/*
 *功能：`main`*参数：*-argc：提供的命令行参数的数量，包括*命令本身。*-argv：命令行参数的数组，包括命令*本身。*用途：标准`main`函数定义。运行时，可执行文件*`Commit-tree`从命令行运行。
*/ 
int main(int argc, char **argv)
{
    int i;             /*
 用于循环计数器。
*/ 
    int len;           /*
 用户登录名的长度。
*/ 
    int parents = 0;   /*
 父提交对象的数量。
*/ 

    /*
 要提交的树的SHA1哈希。
*/ 
    unsigned char tree_sha1[20];
    /*
 父提交对象的SHA1散列数组。
*/ 
    unsigned char parent_sha1[MAXPARENT][20];
    /*
 用于存储用户信息。
*/ 
    char *gecos, *realgecos;
    /*
 用于存储用户的电子邮件地址。
*/ 
    char *email, realemail[1000]; 
    /*
 分配给主机名的最大字符数。
*/ 
    size_t hostname_size;
    /*
 用于存储日期。
*/ 
    char *date, *realdate; 
    /*
 用于存储提交消息。
*/ 
    char comment[1000]; 

    #ifndef BGIT_WINDOWS
    /*
 用于存储用户帐户信息的`passwd`结构。
*/ 
    struct passwd *pw;
    char *username;
    #else
    unsigned long uname_len = UNLEN + 1;
    char username[uname_len];
    #endif

    /*
 *用于存储提交时间，单位为自*世界协调时1970年1月1日00：00：00。
*/ 
    time_t now; 
    char *buffer;        /*
 提交对象缓冲区。
*/ 
    unsigned int size;   /*
 提交对象缓冲区的填充部分的大小。
*/ 

    /*
 *如果命令行参数少于2个或*如果命令行中给出的树哈希不是有效的40个字符*SHA1哈希值的表示形式。然后从出口出来。
*/ 
    if (argc < 2 || get_sha1_hex(argv[1], tree_sha1) < 0)
        usage("commit-tree <sha1> [-p <sha1>]* < changelog");

    /*
 *循环通过命令行中给出的父提交散列*论据。检查每个哈希是否为有效的40个字符表示SHA1哈希值的*。如果不是，或者命令行参数不是*正确给出，显示用法并退出。否则，递增父级*柜台。
*/ 
    for (i = 2; i < argc; i += 2) {
        char *a, *b;
        a = argv[i]; b = argv[i+1];
        if (!b || strcmp(a, "-p") || get_sha1_hex(b, parent_sha1[parents]))
            usage("commit-tree <sha1> [-p <sha1>]* < changelog");
        parents++;
    }

    /*
 *如果未指定父提交SHA1散列，则假定这是*树对象的第一次提交。
*/ 
    if (!parents)
        fprintf(stderr, "Committing initial tree %s\n", argv[1]);

    #ifndef BGIT_WINDOWS
    /*
 *获取调用进程的真实用户ID，并存储有关*`pw`passwd结构中的该用户。
*/ 
    pw = getpwuid(getuid());

    /*
 打印一条错误消息，如果getpwuid()返回空，则退出。
*/ 
    if (!pw)
        usage("You don't exist. Go away!");

    /*
 获取用户的全名。
*/ 
    realgecos = pw->pw_gecos;
    /*
 获取用户的登录名。
*/ 
    username = pw->pw_name;
    #else
    GetUserName(username, &uname_len);
    realgecos = username;
    #endif

    /*
 获取用户名的长度。
*/ 
    len = strlen(username);

    /*
 为用户构建一个电子邮件地址。
*/ 
    memcpy(realemail, username, len);
    realemail[len] = '@';

    /*
 获取主机名并将其附加到电子邮件字符串中。
*/ 
    hostname_size = sizeof(realemail) - len - 1;
    #ifndef BGIT_WINDOWS
    gethostname(realemail+len+1, hostname_size);
    #else
    GetComputerName(realemail+len+1, &hostname_size);
    #endif

    /*
 *获取当前日历时间并将其转换为人类可读的字符串*包含相应的本地日期和时间。
*/ 
    time(&now);
    realdate = ctime(&now);

    /*
 *使用任一环境设置作者姓名、电子邮件和提交时间*变量或上述获得的值。
*/ 
    gecos = getenv("COMMITTER_NAME") ? : realgecos;
    email = getenv("COMMITTER_EMAIL") ? : realemail;
    date = getenv("COMMITTER_DATE") ? : realdate;

    /*
 删除上述字符串中的特殊字符。
*/ 
    remove_special(gecos); remove_special(realgecos);
    remove_special(email); remove_special(realemail);
    remove_special(date); remove_special(realdate);

    /*
 为提交对象缓冲区分配空间并进行初始化。
*/ 
    init_buffer(&buffer, &size);

    /*
 将字符串‘tree’和树SHA1散列添加到缓冲区。
*/ 
    add_buffer(&buffer, &size, "tree %s\n", sha1_to_hex(tree_sha1));

    /*
 *对于每个父提交SHA1散列，添加字符串‘Parent’和*散列到缓冲区。**莱纳斯·托瓦尔兹：注意！这种排序意味着完全相同的树*与*不同顺序的父项合并将是_不同_*更改集，即使其他一切保持不变。
*/ 
    for (i = 0; i < parents; i++)
        add_buffer(&buffer, &size, "parent %s\n", 
                   sha1_to_hex(parent_sha1[i]));

    /*
 *添加作者和提交者的姓名、电子邮件和提交时间*缓冲。
*/ 
    add_buffer(&buffer, &size, "author %s <%s> %s\n", gecos, email, date);
    add_buffer(&buffer, &size, "committer %s <%s> %s\n\n", 
               realgecos, realemail, realdate);
    /*
 *将提交消息添加到缓冲区。这就是要求用户*按CTRL-D完成`Commit-tree`命令。
*/ 
    while (fgets(comment, sizeof(comment), stdin) != NULL)
        add_buffer(&buffer, &size, "%s", comment);

    /*
 *添加一个包含提交大小的十进制形式的字符串*对象数据放到缓冲区中，然后将提交对象标记添加到*缓冲区。
*/ 
    finish_buffer("commit ", &buffer, &size);

    /*
 *清空提交对象，计算哈希值，然后调用*WRITE_SHA1_BUFFER函数，将放气对象写入对象*数据库。
*/ 
    write_sha1_file(buffer, size);

    return 0;   /*
 回报成功。
*/ 
}
