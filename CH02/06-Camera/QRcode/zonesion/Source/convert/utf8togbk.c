#include "utf8togbk.h"
#include "unicodetogb2312.h"

//�õ�UTF-8�ĳ���
int enc_get_utf8_size(const unsigned char pInput)
{
  unsigned char c = pInput;
  // 0xxxxxxx ����0
  // 10xxxxxx ������
  // 110xxxxx ����2
  // 1110xxxx ����3
  // 11110xxx ����4
  // 111110xx ����5
  // 1111110x ����6
  if(c< 0x80) return 0;
  if(c>=0x80 && c<0xC0) return -1;
  if(c>=0xC0 && c<0xE0) return 2;
  if(c>=0xE0 && c<0xF0) return 3;
  if(c>=0xF0 && c<0xF8) return 4;
  if(c>=0xF8 && c<0xFC) return 5;
  if(c>=0xFC) return 6;
  return 0;
}
 
/*********************************************************************************************
* ����:enc_utf8_to_unicode_one
* ����:��һ���ַ���UTF8����ת����Unicode(UCS-2��UCS-4)����. 
* ����:pInput ָ�����뻺����, ��UTF-8���� 
*        Unic     ָ�����������, �䱣������ݼ���Unicode����ֵ, 
*                    ����Ϊunsigned long . pInput      
*                    ָ�����뻺����, ��UTF-8����  
* ����:�ɹ��򷵻ظ��ַ���UTF8������ռ�õ��ֽ���; ʧ���򷵻�0. 
* �޸�:��
* ע��:UTF8û���ֽ�������, ����Unicode���ֽ���Ҫ��; 
 *        �ֽ����Ϊ���(Big Endian)��С��(Little Endian)����; 
 *        ��Intel�������в���С�˷���ʾ, �ڴ˲���С�˷���ʾ. (�͵�ַ���λ) 
*********************************************************************************************/
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)  
{  
    // b1 ��ʾUTF-8�����pInput�еĸ��ֽ�, b2 ��ʾ�θ��ֽ�, ...  
    char b1, b2, b3, b4, b5, b6;  
    int utfbytes; 
    unsigned char *pOutput;  
    *Unic = 0x00; // �� *Unic ��ʼ��Ϊȫ��  
    pOutput = (unsigned char *)Unic;
    utfbytes= enc_get_utf8_size(*pInput);  //�ȵõ�UTF-8�ĳ���
    switch ( utfbytes )  
    {  
        case 0:  
            *pOutput     = *pInput;  
            utfbytes    += 1;  
            break;  
        case 2:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            if ( (b2 & 0xE0) != 0x80 )  
                return 0;  
            *pOutput     = (b1 << 6) + (b2 & 0x3F);  
            *(pOutput+1) = (b1 >> 2) & 0x07;  
            break;  
        case 3:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b2 << 6) + (b3 & 0x3F);  
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
            break;  
        case 4:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b3 << 6) + (b4 & 0x3F);  
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
            break;  
        case 5:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b4 << 6) + (b5 & 0x3F);  
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
            *(pOutput+3) = (b1 << 6);  
            break;  
        case 6:  
            b1 = *pInput;  
            b2 = *(pInput + 1);  
            b3 = *(pInput + 2);  
            b4 = *(pInput + 3);  
            b5 = *(pInput + 4);  
            b6 = *(pInput + 5);  
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
                    || ((b6 & 0xC0) != 0x80) )  
                return 0;  
            *pOutput     = (b5 << 6) + (b6 & 0x3F);  
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
            break;  
        default:  
            return 0;  
            break;  
    }  
 
    return utfbytes;  
}  
 
/*********************************************************************************************
* ����:enc_unicode_to_utf8_one
* ����:��һ���ַ���Unicode(UCS-2��UCS-4)����ת����UTF-8����. 
* ����:unic     �ַ���Unicode����ֵ 
*        pOutput  ָ����������ڴ洢UTF8����ֵ�Ļ�������ָ�� 
*        outsize  pOutput����Ĵ�С 
* ����:����ת������ַ���UTF8������ռ���ֽ���, ��������򷵻� 0 . 
* �޸�:��
* ע��:1. UTF8û���ֽ�������, ����Unicode���ֽ���Ҫ��; 
*            �ֽ����Ϊ���(Big Endian)��С��(Little Endian)����; 
*            ��Intel�������в���С�˷���ʾ, �ڴ˲���С�˷���ʾ. (�͵�ַ���λ) 
*        2. �뱣֤ pOutput �������������� 6 �ֽڵĿռ��С! 
*********************************************************************************************/
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput)  
{  
    if ( unic <= 0x0000007F )  
    {  
        // * U-00000000 - U-0000007F:  0xxxxxxx  
        *pOutput     = (unic & 0x7F);  
        return 1;  
    }  
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
    {  
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
        *(pOutput+1) = (unic & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;  
        return 2;  
    }  
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
    {  
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
        *(pOutput+2) = (unic & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;  
        return 3;  
    }  
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
    {  
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+3) = (unic & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;  
        return 4;  
    }  
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
    {  
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+4) = (unic & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;  
        return 5;  
    }  
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
    {  
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+5) = (unic & 0x3F) | 0x80;  
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;  
        return 6;  
    }  
    return 0;  
}  

/*********************************************************************************************
* ����:utf8togbk2312
* ����:UTF8����ת��ΪGBK2312����
* ����:pInput �����UTF8�ַ���
*        pOutput UTF8ת��gbk2312�Ĵ洢���
*        outsize  pOutput����Ĵ�С 
* ����: ת���ɹ�������ֽ�����GB2312���ֽ�����
* �޸�:��
* ע��:��
*********************************************************************************************/
int utf8togbk2312(unsigned char* pInput,unsigned char *pOutput)
{
  unsigned char* p=pOutput;
  while(*pInput!=0)                     // ����δ����
  { 
    /* �ַ� */
    if(*pInput<0x80){
      *pOutput++=*pInput++;             // �ַ�����ֱ�ӿ���
    }
    /* ���� */
    else
    {
      unsigned long unicode;
      int res_len=enc_utf8_to_unicode_one((const unsigned char*)pInput,&unicode); // ʶ�𵥸�utf8�ַ���ת��ΪUnicode
      if(res_len){
        pInput=pInput+res_len;                        //����ƫ��
        u16 gb_code= UnicodeToGB2312(unicode);        //Unicodeת��ΪGB2312
        pOutput[0]=gb_code>>8;
        pOutput[1]=gb_code;                           //������GB2312�ַ�
        pOutput=pOutput+2;
      }
      else{
        printf("enc_utf8_to_unicode_one error\r\n");
        break;
      }
    }
  }
  return pOutput-p;
}
