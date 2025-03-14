#include "malloc.h"

#if !(__ARMCC_VERSION >= 6010050)   /* ����AC6����������ʹ��AC5������ʱ */

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
/* �ڴ��(64�ֽڶ���) */
static uint8_t mem1base[MEM1_MAX_SIZE] ;                                                           /* �ڲ�SRAM�ڴ�� */

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
/* �ڴ����� */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];           /* �ڲ�SRAM�ڴ��MAP */

#else      /* ʹ��AC6������ʱ */

/* �ڴ��(64�ֽڶ���) */
static __ALIGNED(64) uint8_t mem1base[MEM1_MAX_SIZE];                                                         /* �ڲ�SRAM�ڴ�� */

/* �ڴ����� */
static MT_TYPE mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                            /* �ڲ�SRAM�ڴ��MAP */

#endif

/* �ڴ������� */
const uint32_t memtblsize[SRAMBANK] = { MEM1_ALLOC_TABLE_SIZE};           /* �ڴ���С */
const uint32_t memblksize[SRAMBANK] = { MEM1_BLOCK_SIZE};                 /* �ڴ�ֿ��С */
const uint32_t memsize[SRAMBANK] = { MEM1_MAX_SIZE};                      /* �ڴ��ܴ�С */

/* �ڴ��������� */
struct _m_mallco_dev mallco_dev=
{
    my_mem_init,                                                          /* �ڴ��ʼ�� */
    my_mem_perused,                                                       /* �ڴ�ʹ���� */
    mem1base,                                                             /* �ڴ�� */
    mem1mapbase,                                                          /* �ڴ����״̬�� */
    0,                                                                    /* �ڴ����δ���� */
};

/*********************************************************************************************
* ����:my_mem_copy
* ����:�����ڴ�
* ����:des : Ŀ�ĵ�ַ
*        src : Դ��ַ
*        n    : ��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/    
void my_mem_copy(void *des, void *src, uint32_t n)  
{  
    uint8_t *xdes = des;
    uint8_t *xsrc = src; 
    while (n--)*xdes++ = *xsrc++;  
}  

/*********************************************************************************************
* ����:my_mem_set
* ����:�����ڴ�ֵ
* ����:s    : �ڴ��׵�ַ
*        c     : Ҫ���õ�ֵ
*        count : ��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/   
void my_mem_set(void *s, uint8_t c, uint32_t count)  
{  
    uint8_t *xs = s;  
    while (count--)*xs++ = c;  
}  

/*********************************************************************************************
* ����:my_mem_init
* ����:�ڴ�����ʼ��
* ����:memx : �����ڴ��
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/   
void my_mem_init(uint8_t memx)  
{
    uint8_t mttsize = sizeof(MT_TYPE);                                  /* ��ȡmemmap��������ͳ���(uint16_t /uint32_t)*/
    my_mem_set(mallco_dev.memmap[memx], 0, memtblsize[memx]*mttsize);   /* �ڴ�״̬���������� */
    mallco_dev.memrdy[memx] = 1;                                        /* �ڴ�����ʼ��OK */
}

/*********************************************************************************************
* ����:my_mem_perused
* ����:��ȡ�ڴ�ʹ����
* ����:memx : �����ڴ��
* ����:ʹ����(������10��,0~1000,����0.0%~100.0%)
* �޸�:��
* ע��:��
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
* ����:my_mem_malloc
* ����:�ڴ����(�ڲ�����)
* ����:memx : �����ڴ��
*        size : Ҫ������ڴ��С(�ֽ�)
* ����:�ڴ�ƫ�Ƶ�ַ
* �޸�:��
* ע��: 0 ~ 0xFFFFFFFE : ��Ч���ڴ�ƫ�Ƶ�ַ
*         0xFFFFFFFF     : ��Ч���ڴ�ƫ�Ƶ�ַ
*********************************************************************************************/   
uint32_t my_mem_malloc(uint8_t memx, uint32_t size)  
{  
    signed long offset = 0;  
    uint32_t nmemb;                                             /* ��Ҫ���ڴ���� */
    uint32_t cmemb = 0;                                         /* �������ڴ���� */
    uint32_t i;

    if (!mallco_dev.memrdy[memx])
    {
        mallco_dev.init(memx);                                  /* δ��ʼ��,��ִ�г�ʼ�� */
    }

    if (size == 0)
    {
        return 0XFFFFFFFF;                                      /* ����Ҫ���� */
    }
    nmemb = size / memblksize[memx];                            /* ��ȡ��Ҫ����������ڴ���� */

    if (size % memblksize[memx]) 
    {
        nmemb++;
    }

    for (offset = memtblsize[memx] - 1;offset >= 0; offset--)   /* ���������ڴ������ */
    {
        if (!mallco_dev.memmap[memx][offset])
        {
            cmemb++;                                            /* �������ڴ�������� */
        }
        else 
        {
            cmemb = 0;                                          /* �����ڴ������ */
        }

        if (cmemb == nmemb)                                     /* �ҵ�������nmemb�����ڴ�� */
        {
            for (i = 0; i < nmemb; i++)                         /* ��ע�ڴ��ǿ�  */
            {  
                mallco_dev.memmap[memx][offset + i] = nmemb;  
            }

            return (offset * memblksize[memx]);                 /* ����ƫ�Ƶ�ַ  */
        }
    }

    return 0XFFFFFFFF;                                          /* δ�ҵ����Ϸ����������ڴ�� */
}

/*********************************************************************************************
* ����:my_mem_free
* ����:�ͷ��ڴ�(�ڲ�����)
* ����:memx   : �����ڴ��
*        offset : �ڴ��ַƫ��
* ����:0, �ͷųɹ�;
*        1, �ͷ�ʧ��;
*        2, ��������(ʧ��);
* �޸�:��
* ע��: 
*********************************************************************************************/   
uint8_t my_mem_free(uint8_t memx, uint32_t offset)
{
    int i;

    if (!mallco_dev.memrdy[memx])                       /* δ��ʼ��,��ִ�г�ʼ�� */
    {
        mallco_dev.init(memx);
        return 1;                                       /* δ��ʼ�� */
    }

    if (offset < memsize[memx])                         /* ƫ�����ڴ����. */
    {
        int index = offset / memblksize[memx];          /* ƫ�������ڴ����� */
        int nmemb = mallco_dev.memmap[memx][index];     /* �ڴ������ */

        for (i = 0; i < nmemb; i++)                     /* �ڴ������ */
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }

        return 0;
    }
    else
    {
        return 2;                                       /* ƫ�Ƴ����� */
    }
}

/******* �ⲿ���� ********/
/*********************************************************************************************
* ����:myfree
* ����:�ͷ��ڴ�(�ⲿ����)
* ����:memx : �����ڴ��
*        ptr  : �ڴ��׵�ַ
* ����:��
* �޸�:��
* ע��:��
*********************************************************************************************/   
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;

    if (ptr == NULL)
    {
        return;                 /* ��ַΪ0. */
    }

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    my_mem_free(memx, offset);  /* �ͷ��ڴ� */
}

/*********************************************************************************************
* ����:mymalloc
* ����:�����ڴ�(�ⲿ����)
* ����:memx : �����ڴ��
*        size : Ҫ������ڴ��С(�ֽ�)
* ����:���䵽���ڴ��׵�ַ.
* �޸�:��
* ע��:��
*********************************************************************************************/   
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0xFFFFFFFF)   /* ������� */
    {
      #ifdef MY_DEBUG
        printf("mymalloc NULL\r\n");
      #endif
        return NULL;            /* ���ؿ�(0) */
    }
    else                        /* ����û����, �����׵�ַ */
    {
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
    }
}

/*********************************************************************************************
* ����:myrealloc
* ����:���·����ڴ�(�ⲿ����)
* ����:memx : �����ڴ��
*        *ptr : ���ڴ��׵�ַ
*        size : Ҫ������ڴ��С(�ֽ�)
* ����:�·��䵽���ڴ��׵�ַ.
* �޸�:��
* ע��:��
*********************************************************************************************/   
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)
{
  uint32_t offset;
  offset = my_mem_malloc(memx, size);

  if (offset == 0xFFFFFFFF)                                                          /* ������� */
  {
      #ifdef MY_DEBUG
        printf("myrealloc NULL\r\n");
      #endif
      return NULL;                                                                   /* ���ؿ�(0) */
  }
  else                                                                               /* ����û����, �����׵�ַ */
  {
      my_mem_copy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size); /* �������ڴ����ݵ����ڴ� */
      myfree(memx, ptr);                                                             /* �ͷž��ڴ� */
      return (void *)((uint32_t)mallco_dev.membase[memx] + offset);                  /* �������ڴ��׵�ַ */
  }
}

/*********************************************************************************************
* ����:*mycalloc
* ����:����������ڴ�ռ䣬������һ��ָ������ָ��(�ⲿ����)
* ����:memx : �����ڴ��
*        nitems -- Ҫ�������Ԫ�ظ�����
*        size -- Ԫ�صĴ�С��
* ����:���䵽���ڴ��׵�ַ.
* �޸�:��
* ע��:��
*********************************************************************************************/   
void *mycalloc(uint8_t memx, uint32_t nitems, uint32_t size)
{
  uint32_t offset;
  offset = my_mem_malloc(memx, nitems*size);

  if (offset == 0xFFFFFFFF)   /* ������� */
  {
      #ifdef MY_DEBUG
        printf("mycalloc NULL\r\n");
      #endif
      return NULL;            /* ���ؿ�(0) */
  }
  else                        /* ����û����, �����׵�ַ */
  {
      my_mem_set(mallco_dev.membase[memx] + offset, 0, nitems*size);   /* �ڴ�״̬���������� */
      return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
  }
}
