/***********************************************
*		   �������-�����					   *
*	       by:Sucoder QQ:330729793			   *
*				2013.01.25					   *
*�û�������Ϣ�����弰������..     	       *
***********************************************/

#include "server.h"
#include "dbinit.h"

int Dispose_msg(int socket,char *recvbuf,struct Seruserinfo *clientinfo,int clientnum,int index)
{
	struct Message *msg=(struct Message*)recvbuf;
	
	switch(msg->type){
		case USERREG:{	//�û�ע��
			struct Regloginmsg *regmsg=(struct Regloginmsg*)msg->data;
			int mynum = user_reg(regmsg);
			if(mynum<0){
			 send(socket,"sorry,ע��ʧ��������ע��!",strlen("sorry,ע��ʧ��������ע��!"),0);
			}
			else{
				msg->type=REGSERMSG;
				memset(msg->data,0,sizeof(msg->data));
				snprintf(msg->data,sizeof(msg->data),"�����뵽�ĺ���ID: %d \n\t���μ�\n",mynum);
				send(socket,msg,sizeof(struct Message),0);
				}
			}
			break;
		case LOGIN:{  //��½ ����SOCKET��ID,�û�������һ����������û�������
			struct Regloginmsg *loginmsg=(struct Regloginmsg*)msg->data;
			int login=0;
			for(login=0;login<clientnum;login++)//��ֹ�ظ���½
			{
				if(clientinfo[login].id==loginmsg->id){
					struct Message *msgtemp=(struct Message*)malloc(sizeof(struct Message));
					memset(msgtemp,0,sizeof(struct Message));
					msgtemp->type=LOGINSERMSG;
					msgtemp->torf=-1;
					snprintf(msgtemp->data,sizeof(msgtemp->data),"��½ʧ��,��ID�Ѿ���½,��ֹ�ظ���½!\n");
					send(socket,msgtemp,sizeof(struct Message),0);
					free(msgtemp);
					return -1;
					}
				}
			clientinfo[index].id=loginmsg->id;
			login=user_login(loginmsg,clientinfo[index].username);
			memset(msg,0,sizeof(struct Message));
			msg->type=LOGINSERMSG;
			if(login<=0){
				clientinfo[index].id=-1;
				msg->torf=-1;				
				snprintf(msg->data,sizeof(msg->data),"��½ʧ��,�û�ID���������!\n");
				send(socket,msg,sizeof(struct Message),0);
			}
			else{
				msg->torf=1;
				snprintf(msg->data,sizeof(msg->data),"��ϲ��,��½�ɹ�!\n");
				send(socket,msg,sizeof(struct Message),0);
				for(login=0;login<clientnum;login++){//��½�ɹ���ȡ��ǰ���������û�����֪ͨ�����û������µ�½�û�
					memset(msg,0,sizeof(struct Message));
					msg->type=ADDCLIENT; 
					memcpy(msg->data,&clientinfo[login],sizeof(msg->data));
					if(clientinfo[login].id>0){
						send(socket,msg,sizeof(struct Message),0);
						if(clientinfo[login].id!=clientinfo[index].id){//��������Լ�,�����Լ����ߵ�֪ͨ�����������û�
							memset(msg,0,sizeof(struct Message));
							msg->type=ADDCLIENT; 
							memcpy(msg->data,&clientinfo[index],sizeof(msg->data));
							send(clientinfo[login].socket,msg,sizeof(struct Message),0);
						    }
					}
				usleep(100000);
				}//for
				
				}
			}
			break;

		case MSGDATA:{ //�������û�������Ϣ
			struct Usermsg *usermsg=(struct Usermsg*)msg->data;			
			usermsg->srcid=clientinfo[index].id;
			memcpy(usermsg->srcname,clientinfo[index].username,strlen(clientinfo[index].username));
			int i=0,dissocket=-1;
			for(i=0;i<=clientnum;i++)
			{
			 	if(clientinfo[i].id==usermsg->disid)
				{		
					dissocket=clientinfo[i].socket;
					memcpy(usermsg->disname,clientinfo[i].username,strlen(clientinfo[i].username));
					memcpy(msg->data,usermsg,sizeof(struct Usermsg));
					break;//�ҵ��˲��Ұ���Ϣ�ṹ�������,�ȴ����͸����շ�,���շ���socket������disocket
					}
			}
			if(dissocket<0){ //���շ��û�IDû�����߻����������,������Ϣʧ��,֪ͨ���ͷ�.
				msg->type=MSGSERMSG;
				memset(msg->data,0,sizeof(msg->data));
				snprintf(msg->data,sizeof(msg->data),"���û�ID: %d ������Ϣʧ��,���Ժ�����!\n",usermsg->disid);
				send(socket,msg,sizeof(struct Message),0);
				}
			else{//���շ��û����߾�ֱ�ӷ���
				send(dissocket,msg,sizeof(struct Message),0);
			}
		}			
			break;
		default:
			break;

	}
	
	return 0;

}
