#ifndef _VOICE_H
#define _VOICE_H
#include "stm32f4xx.h" 



/*������Ӧ�ı��*/
#define CHANGE_ADMIN_PASSWORD	   1 //�޸Ĺ���Ա����
#define SET_DOOR_PASSWORD	   		 2 //���ÿ�������
#define REGISTER_FINGER					 3 //�Ǽ�ָ��
#define REGISTER_CARD					   4 //�Ǽǿ�Ƭ

#define CHANGE_TIME					   	 5 //�޸�ʱ��
#define DELETE_ALLFINGER				 6 //ɾ������ָ��
#define DELETE_ASSIGN_FINGER		 7 //ɾ��ָ��ָ��
#define RECOVER_ALL							 8 //�ָ���������
#define CHANGE_VOLUME						 9 //��������
#define DELETE_ALL_CARD				   10 //ɾ�������ſ�
#define DELETE_ASSIGN_CARD			 11//ɾ��ָ���ſ�
#define INPUT_ADMIN_PASSWORD     12//���������Ա����  
#define INPUT_NEW_PASSWORD 			 13//������������       
#define INPUT_NEW_PASSWORD_AGAIN 14//���ٴ����������� 
#define PASSWORD_ERROR     			 15//��֤ʧ��
#define PASSWORD_INCONFORMITY		 16//���벻һ��
#define SETTING_SUCCESS					 17//�����ɹ�
#define PASSWORD_REPETITION			 18//�����ظ�
#define PUTCARD 								 19//����ÿ�Ƭ
#define CARD_REGISTERED 				 20//���ظ�             
#define DELETE_ALL_FIGNER		 		 21//�Ƿ�ɾ��
#define DELETE_ASSIGNFIGNER		 	 22//�����ָ 0X16
#define FINGER_REPETITION 		 	 23//ָ���ظ�
#define DOOROPEN_SUCCESS				 24//��ӭ�ؼ�
#define DOOROPEN_FAIL					 	 25//����ʧ��
#define CONNECT_SUCCESS          26//�������ӳɹ�
#define TIPS                     27//��#��ȷ�ϣ���*���˳�
#define REPETE                   28//����������
#define Di 											 29//�                
#define MenLing 					       30//���� ����       
#define Warm 							       31//������


#define VOICE_DATA_L (GPIO_ResetBits(GPIOC, GPIO_Pin_5))
#define VOICE_DATA_H (GPIO_SetBits(GPIOC, GPIO_Pin_5))

#define VOICE_BUSY   (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4))


void voice_init(void);
void voice(u8 cmd);



#endif


