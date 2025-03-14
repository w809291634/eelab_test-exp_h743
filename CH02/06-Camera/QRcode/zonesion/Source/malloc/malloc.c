#include "malloc.h"

#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */

#if defined(__ICCARM__)
#pragma location = "sram_heap"
#pragma data_alignment=64
#elif defined(__CC_ARM)
__attribute__((section(".sram_heap"), zero_init))
__attribute__ ((aligned (64)))
#elif defined(__GNUC__)
__attribute__((section(".sram_heap")))
__attribute__ ((aligned (64)))
#else
#error Unknown compiler
#endif
/* 内存池(64字节对齐) */
static uint8_t mem1base[MEM1_MAX_SIZE] ;                                                           /* 内部SRAM内存池 */

#if defined(__ICCARM__)
#pragma location = "sram_heap"
#pragma data_alignment=4
#elif defined(__CC_ARM)
__attribute__((section(".sram_heap"), zero_init))
__attribute__ ((aligned (4)))
#elif defined(__GNUC__)
__attribute__((section(".sram_heap")))
__attribute__ ((aligned (4)))
#else
#error Unknown compiler
#endif
/* 内存管理表 */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];           /* 内部SRAM内存池MAP */

#else      /* 使用AC6编译器时 */

/* 内存池(64字节对齐) */
static __ALIGNED(64) uint8_t mem1base[MEM1_MAX_SIZE];                                                         /* 内部SRAM内存池 */

/* 内存管理表 */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                            /* 内部SRAM内存池MAP */

#endif

/* 内存管理参数 */
const uint32_t memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE};           /* 内存表大小 */
const uint32_t memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE};                 /* 内存分块大小 */
const uint32_t memsize[SRAMBANK] = { MEM1_MAX_SIZE};                      /* 内存总大小 */

/* 内存管理控制器 */
struct _m_mallco_dev mallco_dev=
{
    my_mem_init,                                                          /* 内存初始化 */
    my_mem_perused,                                                       /* 内存使用率 */
    mem1base,                                                             /* 内存池 */
    mem1mapbase,                                                          /* 内存管理状态表 */
    0,                                                                    /* 内存管理未就绪 */
};

/*********************************************************************************************
* 名称:my_mem_copy
* 功能:复制内存
* 参数:des : 目的地址
*        src : 源地址
*        n    : 需要复制的内存长度(字节为单位)
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/    
void my_mem_copy(void *des, void *src, uint32_t n)  
{  
    uint8_t *xdes = des;
    uint8_t *xsrc = src; 
    while (n--)*xdes++ = *xsrc++;  
}  

/*********************************************************************************************
* 名称:my_mem_set
* 功能:设置内存值
* 参数:s    : 内存首地址
*        c     : 要设置的值
*        count : 需要设置的内存大小(字节为单位)
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/   
void my_mem_set(void *s, uint8_t c, uint32_t count)  
{  
    uint8_t *xs = s;  
    while (count--)*xs++ = c;  
}  

/*********************************************************************************************
* 名称:my_mem_init
* 功能:内存管理初始化
* 参数:memx : 所属内存块
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/   
void my_mem_init(uint8_t memx)  
{
    uint8_t mttsize = sizeof(MT_TYPE);                                  /* 获取memmap数组的类型长度(uint16_t /uint32_t)*/
    my_mem_set(mallco_dev.memmap[memx], 0, memtblsize[memx]*mttsize);   /* 内存状态表数据清零 */
    mallco_dev.memrdy[memx] = 1;                                        /* 内存管理初始化OK */
}

/*********************************************************************************************
* 名称:my_mem_perused
* 功能:获取内存使用率
* 参数:memx : 所属内存块
* 返回:使用率(扩大了10倍,0~1000,代表0.0%~100.0%)
* 修改:无
* 注释:无
*********************************************************************************************/   
uint16_t my_mem_perused(uint8_t memx)  
{  
    uint32_t used = 0;  
    uint32_t i;

    for (i = 0; i < memtblsize[memx]; i++)  
    {  
        if (mallco_dev.memmap[memx][i])
        {
            used++;
        }
    }

    return (used * 1000) / (memtblsize[memx]);  
}

/*********************************************************************************************
* 名称:my_mem_malloc
* 功能:内存分配(内部调用)
* 参数:memx : 所属内存块
*        size : 要分配的内存大小(字节)
* 返回:内存偏移地址
* 修改:无
* 注释: 0 ~ 0xFFFFFFFE : 有效的内存偏移地址
*         0xFFFFFFFF     : 无效的内存偏移地址
*********************************************************************************************/   
uint32_t my_mem_malloc(uint8_t memx, uint32_t size)  
{  
    signed long offset = 0;  
    uint32_t nmemb;                                             /* 需要的内存块数 */
    uint32_t cmemb = 0;                                         /* 连续空内存块数 */
    uint32_t i;

    if (!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);                                  /* 未初始化,先执行初始化 */
    }

    if (size == 0)
    {
        return 0XFFFFFFFF;                                      /* 不需要分配 */
    }
    nmemb = size / memblksize[memx];                            /* 获取需要分配的连续内存块数 */

    if (size % memblksize[memx]) 
    {
        nmemb++;
    }

    for (offset = memtblsize[memx] - 1;offset >= 0; offset--)   /* 搜索整个内存控制区 */
    {
        if (!mallco_dev.memmap[memx][offset])
        {
            cmemb++;                                            /* 连续空内存块数增加 */
        }
        else 
        {
            cmemb = 0;                                          /* 连续内存块清零 */
        }

        if (cmemb == nmemb)                                     /* 找到了连续nmemb个空内存块 */
        {
            for (i = 0; i < nmemb; i++)                         /* 标注内存块非空  */
            {  
                mallco_dev.memmap[memx][offset + i] = nmemb;  
            }

            return (offset * memblksize[memx]);                 /* 返回偏移地址  */
        }
    }

    return 0XFFFFFFFF;                                          /* 未找到符合分配条件的内存块 */
}

/*********************************************************************************************
* 名称:my_mem_free
* 功能:释放内存(内部调用)
* 参数:memx   : 所属内存块
*        offset : 内存地址偏移
* 返回:0, 释放成功;
*        1, 释放失败;
*        2, 超区域了(失败);
* 修改:无
* 注释: 
*********************************************************************************************/   
uint8_t my_mem_free(uint8_t memx, uint32_t offset)
{
    int i;

    if (!mallco_dev.memrdy[memx])                       /* 未初始化,先执行初始化 */
    {
        mallco_dev.init(memx);
        return 1;                                       /* 未初始化 */
    }

    if (offset < memsize[memx])                         /* 偏移在内存池内. */
    {
        int index = offset / memblksize[memx];          /* 偏移所在内存块号码 */
        int nmemb = mallco_dev.memmap[memx][index];     /* 内存块数量 */

        for (i = 0; i < nmemb; i++)                     /* 内存块清零 */
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }

        return 0;
    }
    else
    {
        return 2;                                       /* 偏移超区了 */
    }
}

/******* 外部调用 ********/
/*********************************************************************************************
* 名称:myfree
* 功能:释放内存(外部调用)
* 参数:memx : 所属内存块
*        ptr  : 内存首地址
* 返回:无
* 修改:无
* 注释:无
*********************************************************************************************/   
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;

    if (ptr == NULL)
    {
        return;                 /* 地址为0. */
    }

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    my_mem_free(memx, offset);  /* 释放内存 */
}

/*********************************************************************************************
* 名称:mymalloc
* 功能:分配内存(外部调用)
* 参数:memx : 所属内存块
*        size : 要分配的内存大小(字节)
* 返回:分配到的内存首地址.
* 修改:无
* 注释:无
*********************************************************************************************/   
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0xFFFFFFFF)   /* 申请出错 */
    {
      #ifdef MY_DEBUG
        printf("mymalloc NULL\r\n");
      #endif
        return NULL;            /* 返回空(0) */
    }
    else                        /* 申请没问题, 返回首地址 */
    {
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
    }
}

/*********************************************************************************************
* 名称:myrealloc
* 功能:重新分配内存(外部调用)
* 参数:memx : 所属内存块
*        *ptr : 旧内存首地址
*        size : 要分配的内存大小(字节)
* 返回:新分配到的内存首地址.
* 修改:无
* 注释:无
*********************************************************************************************/   
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)
{
  uint32_t offset;
  offset = my_mem_malloc(memx, size);

  if (offset == 0xFFFFFFFF)                                                          /* 申请出错 */
  {
      #ifdef MY_DEBUG
        printf("myrealloc NULL\r\n");
      #endif
      return NULL;                                                                   /* 返回空(0) */
  }
  else                                                                               /* 申请没问题, 返回首地址 */
  {
      my_mem_copy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size); /* 拷贝旧内存内容到新内存 */
      myfree(memx, ptr);                                                             /* 释放旧内存 */
      return (void *)((uint32_t)mallco_dev.membase[memx] + offset);                  /* 返回新内存首地址 */
  }
}

/*********************************************************************************************
* 名称:*mycalloc
* 功能:分配所需的内存空间，并返回一个指向它的指针(外部调用)
* 参数:memx : 所属内存块
*        nitems -- 要被分配的元素个数。
*        size -- 元素的大小。
* 返回:分配到的内存首地址.
* 修改:无
* 注释:无
*********************************************************************************************/   
void *mycalloc(uint8_t memx, uint32_t nitems, uint32_t size)
{
  uint32_t offset;
  offset = my_mem_malloc(memx, nitems*size);

  if (offset == 0xFFFFFFFF)   /* 申请出错 */
  {
      #ifdef MY_DEBUG
        printf("mycalloc NULL\r\n");
      #endif
      return NULL;            /* 返回空(0) */
  }
  else                        /* 申请没问题, 返回首地址 */
  {
      my_mem_set(mallco_dev.membase[memx] + offset, 0, nitems*size);   /* 内存状态表数据清零 */
      return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
  }
}
