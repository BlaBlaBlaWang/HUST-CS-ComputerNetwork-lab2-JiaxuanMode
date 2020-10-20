#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE	//������Ԥ�������֮�󣬼������У��������Ϊ�������Ѿ��������ͷ�ļ���������
	#include "Global.h"
	#include "SRRdtReceiver.h"

	SRRdtReceiver::SRRdtReceiver() :base(0)
	{
	}

	int SRRdtReceiver::windowSize = 4;


	void SRRdtReceiver::printSlideWindow()
	{
		cout << "]]]]]]]]]���շ����������ƶ�[[[[[[[[[[" << endl;
		int i;
		for (i = 0; i < this->packetWindow.size(); i++)
		{
			printf("�������ڵ�%d����", i);	//���SR�Ľ��շ�����ӡ�������ڵ�ͬʱ����Ҫ��ӡ����������ÿ�����ĵĽ���״̬����Ϊ���Ƕ��ǻ���ı���
			pUtils->printPacket("���ģ�", this->packetWindow[i]);
		}
		while (i != this->windowSize)
		{
			printf("�������ڵ�%d��Ϊ���ҿ���\n", i);
			i++;
		}
		cout << "]]]]]]]]]���շ��������ڴ�ӡ����[[[[[[[[[" << endl;
		return;
	}

	bool SRRdtReceiver::seqNumNew(int seqnum) {
		bool result = false;
		for (int i = 0; i < this->windowSize; i++)
			if ((seqnum ) % 8 == (this->base + i) % 8) {
			/*ֻ��base��Ҫ����i��ȡģ*/
				result = true;
				break;
			}
		return result;
	}

	int SRRdtReceiver::modOrder(int seqnum) {
		if (seqnum - this->base >=0)
		/*ģ�ļ�����ֻ��С��0ʱ�ż���ģ��������0 ���ӣ�*/
			return seqnum - this->base;
		else return seqnum - this->base + 8;
	}

	void SRRdtReceiver::savePktInOrder(Packet packet) {
		int status = 0;	//��ʾ֮ǰ�Ѿ���Ź��������Ѿ��Ƚ�����ɲ���
		for (int i = 0; i < this->packetWindow.size(); i++)
		{
			if (this->modOrder(this->packetWindow[i].seqnum) == this->modOrder(packet.seqnum))	//�Ѿ���ȷ�յ���������Ҫ�ٻ���
			{
				status = 1;
				break;
			}
			else if (this->modOrder(this->packetWindow[i].seqnum) > this->modOrder(packet.seqnum))	//i�ű괦����seqnum��packet���󣬽�packet���뻺�浽�ű�i��λ��
			{
				this->packetWindow.insert(this->packetWindow.begin() + i, packet);
				this->printSlideWindow();
				status = 1;
				break;
			}
			else continue;
		}

		if (status == 0)	//����˳�ѭ������Ϊvector������Ԫ�ض�С��������packet�ŵ����
		/*Ҫ��ȷ��vector��Ϊ�գ����ܶԿ�vectorʹ��size()����*/
		{
			this->packetWindow.push_back(packet);
			this->printSlideWindow();
		}
		
		return;
	}

	SRRdtReceiver::~SRRdtReceiver()
	{
	}


	void SRRdtReceiver::receive(const Packet& packet) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(packet);

		if (checkSum == packet.checksum ) {		//���У�����ȷ����һ���ᷢ��ȷ�ϱ���
			pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

			Packet AckPacket;
			AckPacket.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			AckPacket.seqnum = packet.seqnum;
			memcpy(AckPacket.payload, packet.payload, sizeof(packet.payload));
			AckPacket.checksum = pUtils->calculateCheckSum(AckPacket);
			pUtils->printPacket("���շ�����ȷ�ϱ���", AckPacket);
			pns->sendToNetworkLayer(SENDER, AckPacket);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

			if (seqNumNew(packet.seqnum))	//���seqnum�ڴ����У����ж��Ƿ���Ҫ������߽���
			{
				this->savePktInOrder(packet);
				if (packet.seqnum == this->base)	//��������base��ͬ����packet�����л����packet���򽻸�Ӧ�ò�
				{
					Message msg;
					
					//˳�򽻸�packetWindow�л�����������
					for (; this->packetWindow.size()!=0 && this->packetWindow[0].seqnum == this->base; )
					{
					/*���ڴ�ͷ��ʼ����������ɾ������˷���vectorʱֻ������ǰ���*/
					/*������size()�����ж��Ƿ��Ѿ�Ϊ�գ�֮ǰʹ��i������size()�ȽϵĴ������ڣ�i�ڱ��size()��С���ύ���������*/
						memcpy(msg.data, this->packetWindow[0].payload, sizeof(this->packetWindow[0].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						this->packetWindow.erase(this->packetWindow.begin());
						//�������ɾ�������ú���ĳ�Ϊvector��ͷԪ��
						this->base = (++this->base) % 8;
						/*base�޸ĺ�һ��Ҫȡģ�����ұ���ǰ++�����Ҳ��ܷ��ں�i��ͬ������λ*/
						this->printSlideWindow();
					}
				}
			}
		}
		else 
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
#endif