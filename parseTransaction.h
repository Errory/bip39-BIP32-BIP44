#ifndef _PARSETRANSACTION_H_
#define _PARSETRANSACTION_H_

//BIPs ��׼Э����ض���
#define TransaVersionsize 4        //�汾�ֽڴ�С
#define TransapreviousHashsize  32 //Utxo��hash��С
#define Transaindexsize         4  //�����ѵ�UTXO�������Ŵ�С
#define Transacoinsize          8  //����С
#define TransaInOutlengthsize   1   //����������ȴ�Сռ��1�ֽ�
#define TransaNull              4  //�޶����ֽ�

//BIPs end

#define signSuccess 1     //ǩ���ɹ�
#define signFail    2     //ǩ��ʧ��
#define Nosign      0     //û��ǩ��

#define Transa_address_MAX 256 //֧�������ַ���
#define addresssize_MAX    40 //��ַ���ռ�
#define pathlengh 5//path����

extern unsigned char TxRecivecompleteflag;
extern unsigned char Saveaddresscoinokflag;
extern unsigned char SignResaltState;
extern unsigned long TxPathData[20];


typedef struct
{
    unsigned char coin[Transacoinsize];     //���
    unsigned char address[addresssize_MAX]; //��ַ
    unsigned char addresslenght[TransaInOutlengthsize];            //��ַ����
}
CoinAddressStruct;

typedef struct
{
    unsigned long path[pathlengh];     //����·��path[]={0x8000002C,0x80000000,0x80000000,0,0};
}
TxPath;

typedef struct
{
	unsigned char Transa_txin_num;                             //�������������
    unsigned char Transa_verion_data[TransaVersionsize];                       //���װ汾����
    TxPath Transa_path[256];                                   //����·��                    
    unsigned char Transa_txout_num;                            //�������������
    CoinAddressStruct Transa_coincountdata[Transa_address_MAX];    //����ַ�洢
}
TransactiondataStruct;

extern TransactiondataStruct ParseData;

unsigned char parse_transaction_Byte(unsigned char *datatransaction,unsigned short length_tx);

#endif
