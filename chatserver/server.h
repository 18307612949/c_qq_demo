/*****************************************************
*		   �������-�����				     		 *
*	       by:Sucoder QQ:330729793	     			 *
*				2013.01.25    				  		 *
*��ͷ�ļ�,��Ҫ�õ���ͷ�ļ���������,�������ݿ���Ϣ..  *
*****************************************************/

#ifndef SERVER_H
#define SERVER_H

#include "../include/datauct.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <netdb.h>
#include <strings.h>
#include <mysql/mysql.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <syslog.h>


#define DBHOST 	   "localhost"  //���ݿ�����IP
#define DBPORT	   0		//���ݿ�˿�
#define DBUSER	   "root"	//�����û���
#define DBPASSWD   "root"	//���ݿ�����
#define DBNAME     "mychat"   //���ݿ���
#define TABLENAME  "usertable" //�û�����
#define USERID	   "userid"	//�û�id
#define USERNAME   "username"   //�û��ǳ�
#define USERPASSWD "passwd"	//�û�����

#endif

