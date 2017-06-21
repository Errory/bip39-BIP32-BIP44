#include "includeAll.h"

unsigned char TxRecivecompleteflag=1;               //���ݽ�����ɱ�־λ 0��δ���գ�1 �������
unsigned char SignResaltState=0;                    //ǩ����� 0 û��ǩ��������1 ǩ���ɹ���2ǩ��ʧ��
unsigned char Saveaddresscoinokflag=0;              //����ַ�洢��ɱ�־λ 0 û��ɣ�1 ���

unsigned char         Txsignedcount=0;          //����ǩ������������¼�Ѿ�ǩ������������
TransactiondataStruct ParseData;                //����������������ݻ���
unsigned short        TxOut_startaddress=0;     //����������ݵ���ʼ��ַ

unsigned char         UtxoHashCount=0;          //��hash utxo����������

unsigned char SignResaultSavebuf[73];           //�洢ǩ�����
unsigned char SignResaultSavelength[1];         //�洢ǩ���������
unsigned long TxPathData[20]={0x8000002C,0x80000000,0x80000000,0,0};
/*******************************************
�������ƣ�ǩ��
��Σ�datatransaction ����Ľ��������׵�ַ
**************************************************/

void Tx_signed(unsigned char* datatransaction,unsigned short length_tx)
{
	unsigned short i; 
	unsigned short hash_size;
    HashState hs;
	unsigned char* p=datatransaction;
	unsigned char lenscrip;
	unsigned short        Data_p=0;                 //����ָ��

	unsigned char out[64];//�洢��˽Կ ǰ32�ֽ�Ϊ��˽Կ
	unsigned char HASHbuf[32];//�洢hash���

	sha256Begin(&hs); //sha256��ʼ��

	for(i=0;i<(TransaVersionsize+TransaInOutlengthsize);i++)  //��ǰ5�ֽڽ���hash��4�ֽڰ汾��1�ֽ����볤��
	{
		sha256WriteByte(&hs,datatransaction[i]);
	}
	Data_p=Data_p+TransaVersionsize+TransaInOutlengthsize; //����ָ��+5
	p=&p[Data_p];

	while(UtxoHashCount<ParseData.Transa_txin_num)//�ж���hash utxo����������<������������
	{
		if(Txsignedcount!=UtxoHashCount)//�жϵ�ǰ��utxo�Ƿ���Ҫ�仯��hash
		{//��Ҫ�仯��hash,�ű�������
			hash_size=TransapreviousHashsize+Transaindexsize;//��hash 32+4
			for(i=0;i<hash_size;i++)  
			{
				sha256WriteByte(&hs,p[i]);
			}
           
            sha256WriteByte(&hs,0);
			Data_p=hash_size;
            lenscrip=p[Data_p];//�洢���ĳ���
            p=&p[hash_size+TransaInOutlengthsize+lenscrip];//����ָ��ָ����һ�����׵�ַ
 			for(i=0;i<TransaNull;i++)  
			{
				sha256WriteByte(&hs,p[i]);
			} 
			p=p+TransaNull;

		}
		else
		{//����仯ֱ��hash
			hash_size=TransapreviousHashsize+Transaindexsize+TransaInOutlengthsize+TransaNull+p[TransapreviousHashsize+Transaindexsize];//��С=32+4+1+p[32+4]+4
			for(i=0;i<hash_size;i++)  
			{
				sha256WriteByte(&hs,p[i]);
			}

			Data_p=hash_size; //����ָ��=����ָ��+��С
			p=&p[Data_p];//����ָ��ָ����һ������
		}

		UtxoHashCount++;
	}

	UtxoHashCount=0; //����hash utxo����������

	hash_size=length_tx-TxOut_startaddress;//�������������hash
	p=&datatransaction[TxOut_startaddress];
	for(i=0;i<hash_size;i++)  
	{
		sha256WriteByte(&hs,p[i]);
	}

sha256WriteByte(&hs,0x01);
sha256WriteByte(&hs,0);
sha256WriteByte(&hs,0);
sha256WriteByte(&hs,0);

	sha256Finish(&hs);//������һ��hash
	memcpy(sha256_h, hs.h, 32);
	for(i=0;i<8;i++)                        //�Խ�����д�С��->���ת��
	{
      BigendChang((unsigned char*)(&sha256_h[i]),4); 
	}

	sha256((unsigned char*)sha256_h,32);    //�ٴ���hash sha256
	for(i=0;i<8;i++)                        //�Խ�����д�С��->���ת��
	{
      BigendChang((unsigned char*)(&sha256_h[i]),4); 
	}


	//����ǩ�����洢ǩ�����
//	//��ȡTxPathData
//	for(i=0;i<pathlengh;i++)
//	{
//       TxPathData=ParseData.Transa_path[i];
//	}
    //��ȡTxPathData end


    bip32DerivePrivate(out, master_node,TxPathData,5);//ͨ��master node��path����child private key//����˽Կ

	memcpy(HASHbuf,(unsigned char*)sha256_h,32);
	BigendChang(HASHbuf,32); //HASH���big-end->little-end


	signTransaction(SignResaultSavebuf,SignResaultSavelength,HASHbuf,out);//ǩ��

}

/*************************************************
�������ƣ����������ʼ��ַ
��Σ�datatransaction ����Ľ��������׵�ַ
**************************************************/
void TxOut_startaddress_f(unsigned char *datatransaction)
{
	unsigned char j;
    
	TxOut_startaddress=5;
	for(j=0;j<ParseData.Transa_txin_num;j++)
	{
      TxOut_startaddress=TxOut_startaddress+TransapreviousHashsize+Transaindexsize+TransaInOutlengthsize;
      TxOut_startaddress=TxOut_startaddress+datatransaction[TxOut_startaddress-1]+4;
	}

	ParseData.Transa_txout_num=datatransaction[TxOut_startaddress]; //�洢�������������

}

/**************************************************
�������ƣ�׼����ַ��������ʾ
��Σ�datatransaction ����Ľ��������׵�ַ
**************************************************/
void TxAddressCoinSave(unsigned char *datatransaction)
{
	unsigned short num=0;
	unsigned char* p;

	unsigned char  TxoutAdd=0;//�Դ洢����ۼ���
	unsigned char  scriplength=0;//���ĳ��ȼ�¼
	unsigned char  buf[40];
	unsigned char  buf2[40];
	unsigned short i,wn[1];

	if(Saveaddresscoinokflag==0)
	{
       p=&datatransaction[TxOut_startaddress+1];//������ָ��ָ���һ���������
	   while(TxoutAdd<ParseData.Transa_txout_num)//�жϽ��״洢�Ƿ�ȫ�����
	   {
		memcpy(ParseData.Transa_coincountdata[TxoutAdd].coin,p,Transacoinsize);//�洢���
        p=p+8;//����ָ��+8��ָ����һλ��
		
        scriplength=p[0];//��¼���ĳ���
		p++;
		num=0;
        while(p[0]>0x4b)
		{
			p++;
			num++;
		}
        ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0]=p[0];//��¼��ַ����
        p++;
		num++;
        memcpy(ParseData.Transa_coincountdata[TxoutAdd].address,p,ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0]);//�洢��ַ
		p=p+ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0];
		num=num+ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0];
		while(num<scriplength)
		{
			num++;
			p++;
		}
  

		for(i=0;i<ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0];i++)
		{
           buf[i]=ParseData.Transa_coincountdata[TxoutAdd].address[i];
		}
		HextoBase58check(buf,ParseData.Transa_coincountdata[TxoutAdd].address,(unsigned short*)&(ParseData.Transa_coincountdata[TxoutAdd].addresslenght[0]));//base58checkת��//base58ת��

	    TxoutAdd++;//�Դ洢����ۼ���+1
	  }

       Saveaddresscoinokflag=1;//��ַ�洢�����λ
	}

}

/**********************************
�������ƣ�����ǩ�����
**********************************/
void Send_sigresault(unsigned char* buf,unsigned char length)
{
	//debug
	unsigned char i;

		printf("TX_sign:\r\n");
	  for(i=0;i<length;i++)
	  {
        printf("%02x",buf[i]);
	  }
      printf("\r\n");
      printf("\r\n");
}

unsigned char Buttonkey=0;
//
void AccordingKey(unsigned char *datatransaction,unsigned short length_tx)
{
	switch(Buttonkey)
	{
	case 0://ȷ�ϼ�
		   Tx_signed(datatransaction,length_tx);     //�Ե�ǰ��Ҫǩ���Ľ��׽���ǩ��
		   Txsignedcount++;//ǩ���������ۼ�
		   Send_sigresault(SignResaultSavebuf,SignResaultSavelength[0]);//����ǩ�����



		   if(Txsignedcount<ParseData.Transa_txin_num) {}//�ж����������Ƿ�ǩ�����
		   else
		   {   //�ɹ� ǩ�����
			   SignResaltState=signSuccess;
			   Txsignedcount=0;//��ǩ��������
               TxRecivecompleteflag=0;//�彻�����ݽ�����ɱ�־
			   Saveaddresscoinokflag=0;  ///����ַ�洢��ɱ�־λ����
		   }
	break;
	case 1://ȡ����
			SignResaltState=signFail; //ʧ�ܱ�־λ��ֵ
			TxRecivecompleteflag=0;   //�������ݽ��ձ�־λ����
			Saveaddresscoinokflag=0;  ///����ַ�洢��ɱ�־λ����
		break;


	}

}


/*************************************************
�������ƣ�������������
��Σ�datatransaction ����Ľ��������׵�ַ,length_tx ���׳���
���Σ������������ֵ 1 ʧ�ܣ�0 �ɹ�
**************************************************/

unsigned char parse_transaction_Byte(unsigned char *datatransaction,unsigned short length_tx)
{

	if(TxRecivecompleteflag==1)//�������ݽ������
	{
		memcpy(ParseData.Transa_verion_data,datatransaction,4);         //�汾����
		ParseData.Transa_txin_num=datatransaction[4];                      //�������������洢����
		TxOut_startaddress_f(datatransaction);                             //���ҳ������λ��
		if((ParseData.Transa_txin_num!=0)&&(ParseData.Transa_txout_num!=0))//�жϽ����Ƿ�Ϸ�
		{
		     TxAddressCoinSave(datatransaction);       //�Խ���ַ���д洢
		     AccordingKey(datatransaction,length_tx);  //���ݼ�ֵ���в���
		   
		}
		else
		{
		//�����޷�ǩ��
			SignResaltState=signFail; //ʧ�ܱ�־λ��ֵ
			TxRecivecompleteflag=0;   //�������ݽ��ձ�־λ����
			Saveaddresscoinokflag=0;  ///����ַ�洢��ɱ�־λ����
		}
	}

    

	return 0;
}