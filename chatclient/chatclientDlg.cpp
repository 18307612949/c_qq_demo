// chatclientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chatclient.h"
#include "chatclientDlg.h"
#include "RegDlg.h"
#include "ChatDlg.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")//PlaySound����������ͷ�ļ��Ϳ�

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct mythreaddata{//���ڴ����̲߳����Ľṹ������
	char recvbuf[MSGALLLEN];
	CChatclientDlg *dlg;
};
struct Itemdata{//���ڴ���û��������ݲ��󶨵�listctrl�и���������
	CChatDlg *chatdlg;
	CString msg;
};
UINT init_socket(LPVOID pParam);//���ӷ������߳�
/////////////////////////////////////////////////////////////////////////////
// CChatclientDlg dialog
UINT ztthread(LPVOID pParam){//��½״̬�߳�
	mythreaddata *data=(mythreaddata *)pParam;
	char *buf=(char *)data->recvbuf;
	CChatclientDlg *dlg=(CChatclientDlg *)data->dlg;
	CString str;
	str.Format("%s",buf);
	int i=0;
	while (!dlg->login){
		str+=".";
		dlg->GetDlgItem(IDC_STATIC_ZT)->SetWindowText(str);
		Sleep(800);
		i++;
		if (i%7==0)		{
			str.Format("%s",buf);
		}
	}//while
	dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDC_BUTTON_REG)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
	dlg->GetDlgItem(IDC_EDIT_PASS)->EnableWindow(TRUE);
	free(data);
	return 1;
}
UINT Disposemsgthread(LPVOID pParam){//���ݴ����߳�
	mythreaddata *data=(mythreaddata *)pParam;
	struct Message *msg=(struct Message*)data->recvbuf;	
	CChatclientDlg *dlg=(CChatclientDlg *)data->dlg;
	switch(msg->type){
			case REGSERMSG:{//ע�������������Ϣ
				dlg->login=TRUE;//Ϊ�˵�½״̬��ʾ.
				Sleep(1000);
				dlg->login=FALSE;//��ԭ��½״̬
					AfxMessageBox(msg->data);
					dlg->login=TRUE;
			}
					break;
			case LOGINSERMSG:{//��½������������Ϣ
					dlg->login=TRUE;//Ϊ�˵�½״̬��ʾ.
					Sleep(1000);
					dlg->login=FALSE;//��ԭ��½״̬
					dlg->m_zt="״̬:";
					dlg->m_zt+=msg->data;
					dlg->GetDlgItem(IDC_STATIC_ZT)->SetWindowText(dlg->m_zt);
					dlg->UpdateData(FALSE);
					if (msg->torf==1)
						{
 							dlg->GetDlgItem(IDC_STATIC_ZXNUM)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDC_LIST_USER)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDC_STATIC_MYNAME)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDC_MYNAME)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDC_STATIC_MYID)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDC_MYID)->ShowWindow(SW_SHOW);
 							dlg->GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
							dlg->GetDlgItem(IDC_BUTTON_REG)->ShowWindow(SW_HIDE);
 							dlg->GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_HIDE);
							dlg->GetDlgItem(IDC_STATIC_ZT)->ShowWindow(SW_HIDE);
 							dlg->GetDlgItem(IDC_EDIT_ID)->ShowWindow(SW_HIDE);
 							dlg->GetDlgItem(IDC_EDIT_PASS)->ShowWindow(SW_HIDE);
							dlg->GetDlgItem(IDC_STATIC_ID)->ShowWindow(SW_HIDE);
							dlg->GetDlgItem(IDC_STATIC_PASS)->ShowWindow(SW_HIDE);
							dlg->login=TRUE;
							break;
						}
					else{
					dlg->GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
					dlg->GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_HIDE);
					dlg->login=FALSE;
					}
			}
					break;
			case MSGSERMSG:{//������Ϣʧ�ܷ�����������Ϣ
				AfxMessageBox(msg->data);				
			}			
					break;
			case MSGDATA:{//�յ������û���������Ϣ
				struct Usermsg *usermsg=(struct Usermsg *)msg->data;
				char *getid=(char*)malloc(30);
				for (int i=0;i<dlg->m_onLineList.GetItemCount();i++)
				{
					dlg->m_onLineList.GetItemText(i,1,getid,30);
					if (atoi(getid)==usermsg->srcid)
					{						
						//ȡ���������ݣ���������Ϣ.
						struct Itemdata *data;
						data=(Itemdata *)dlg->m_onLineList.GetItemData(i);
						CString strtem;
						time_t ostime;
						time(&ostime); 
						CTime time(ostime);
						CString timestr;
						timestr.Format("%d:%d:%d",time.GetHour(),time.GetMinute(),time.GetSecond());
						strtem.Format("%s(%d) (%s)����˵:\r\n    %s",usermsg->srcname,usermsg->srcid,timestr,usermsg->data);
						if (data->msg!=""){
							data->msg+="\r\n";
						}
						data->msg+=strtem;
						dlg->m_onLineList.SetItemData(i,(DWORD)data);
						if (data->chatdlg==NULL){
							dlg->m_onLineList.SetItemText(i,2,"������Ϣ");
						}
						break;
					}
				}
				free(getid);
				if (dlg->sound){//��������
					::PlaySound(NULL,NULL,SND_FILENAME|SND_ASYNC);//�����һ����ΪNULL��ֹͣ���ڲ��ŵ�����
					::PlaySound(MAKEINTRESOURCE(IDR_MSG),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
				}
				
			}			
					break;
			case ADDCLIENT:{//���µ���������				
				struct Seruserinfo *userinfo=(struct Seruserinfo *)msg->data;
				CString id;
				id.Format("%d",userinfo->id);
				if (userinfo->id==dlg->loginmsg.id)//������Լ�
				{
					dlg->GetDlgItem(IDC_MYID)->SetWindowText(id);
					dlg->GetDlgItem(IDC_MYNAME)->SetWindowText(userinfo->username);
					dlg->loginmsg.id=userinfo->id;	//���Լ���ID���û�������������
					memcpy(dlg->loginmsg.username,userinfo->username,sizeof(userinfo->username));
				}
				dlg->m_onLineList.InsertItem(0,userinfo->username,0);
				dlg->m_onLineList.SetItemText(0,1,id);
				struct Itemdata *data= new Itemdata;
				data->chatdlg=NULL;
				data->msg="";
				dlg->m_onLineList.SetItemData(0,DWORD(data));//���ø��������еĴ���ָ��Ϊ�վ���û�д򿪴���
				dlg->m_client++;//����������������1
				id.Format("��ǰ��������: %d ��",dlg->m_client),
				dlg->GetDlgItem(IDC_STATIC_ZXNUM)->SetWindowText(id);
				if (dlg->sound){//��������
					::PlaySound(NULL,NULL,SND_FILENAME|SND_ASYNC);//�����һ����ΪNULL��ֹͣ���ڲ��ŵ�����
					::PlaySound(MAKEINTRESOURCE(IDR_LOGIN),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
				}
			}			
					break;
			case CUTCLIENT:{//����������
				struct Seruserinfo *userinfo=(struct Seruserinfo *)msg->data;
				int id=0;
				char *buf=(char*)malloc(100);
				for (int i=0;i<dlg->m_client;i++)
				{
					dlg->m_onLineList.GetItemText(i,1,buf,100);
					id=atoi(buf);
					if (id==userinfo->id)
					{
						struct Itemdata *data;
						data=(Itemdata *)dlg->m_onLineList.GetItemData(i);
						if (data->chatdlg!=NULL){
							data->chatdlg->GetDlgItem(IDOK)->EnableWindow(FALSE);
						}
						data->chatdlg=NULL;
						free(data);//�ͷ����������󶨵Ķ�̬�ڴ渽������
						data=NULL;
						dlg->m_onLineList.DeleteItem(i);						
						dlg->m_client--;//��������������1
						CString str;
						str.Format("��ǰ��������: %d ��",dlg->m_client);
						dlg->GetDlgItem(IDC_STATIC_ZXNUM)->SetWindowText(str);
						if (dlg->sound){//��������
							::PlaySound(NULL,NULL,SND_FILENAME|SND_ASYNC);//�����һ����ΪNULL��ֹͣ���ڲ��ŵ�����
							::PlaySound(MAKEINTRESOURCE(IDR_LOGOUT),AfxGetResourceHandle(),SND_ASYNC|SND_RESOURCE|SND_NODEFAULT);
						}
						free(buf);
						break;
					}
				}
		 }
			break;
		default:
				break;
	}//switch
	free(data);
	return 1;
}
UINT recvmsgthread(LPVOID pParam){//��Ϣ���ݽ����߳�
	CChatclientDlg *dlg=(CChatclientDlg *)pParam;
	while (dlg->m_socket==NULL){
		Sleep(500);
	}
	fd_set allfd;
	struct timeval tv;
	int index;
	char recvbuf[MSGALLLEN];
	SOCKET maxfd;
	int len;
	FD_ZERO(&allfd);
	while(1){
		tv.tv_sec=0;	
		tv.tv_usec=500000;//��ʱʱ��0.5��
		FD_SET(dlg->m_socket,&allfd);
		maxfd=dlg->m_socket;
		index=select(maxfd+1,&allfd,NULL,NULL,&tv);
		if(SOCKET_ERROR == index){
			AfxMessageBox("select��������\n");
			return 0;
		}
		else if(0==index) continue;//�ȴ���ʱ����û�пɶ���Ϣ�����ؼ���ѯ��
		if(FD_ISSET(dlg->m_socket,&allfd)){
			memset(recvbuf,0,sizeof(recvbuf));
			if((len=recv(dlg->m_socket,recvbuf,sizeof(recvbuf),0))<=0){//�������Ͽ��������ر��˷����
				AfxMessageBox("������������ѶϿ�!�������ӷ�����!");
				closesocket(dlg->m_socket);
				dlg->m_onLineList.DeleteAllItems();
				dlg->GetDlgItem(IDC_STATIC_ZXNUM)->SetWindowText("��ǰ��������:0 ��");
				dlg->GetDlgItem(IDC_STATIC_ZXNUM)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDC_LIST_USER)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDC_STATIC_MYNAME)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDC_MYNAME)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDC_STATIC_MYID)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDC_MYID)->ShowWindow(SW_HIDE);
				dlg->GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_EDIT_ID)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_EDIT_PASS)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_STATIC_ID)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_STATIC_PASS)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_BUTTON_REG)->ShowWindow(SW_SHOW);
				dlg->GetDlgItem(IDC_STATIC_ZT)->ShowWindow(SW_SHOW);
				dlg->login=FALSE;
				dlg->m_socket=NULL;
				dlg->m_client=0;
				AfxBeginThread((AFX_THREADPROC)init_socket,dlg);
				return 0;
			}
			mythreaddata *data=(mythreaddata *)malloc(sizeof(struct mythreaddata));
			memcpy(data->recvbuf,recvbuf,sizeof(recvbuf));
			data->dlg=dlg;
			AfxBeginThread((AFX_THREADPROC)Disposemsgthread,data);
		}
		
	}//while
	return 1;
}

UINT init_socket(LPVOID pParam){//���ӷ������߳�
	CChatclientDlg *dlg=(CChatclientDlg *)pParam;
	WORD	wVersionRequested;			//����socket�汾
	WSADATA	wsaData;					//wsaData�ṹ
	int		nErrCode;					//����ֵ
	
	wVersionRequested = MAKEWORD( 2, 2 );//����windows Sockets 2.2�汾	
	nErrCode = WSAStartup( wVersionRequested, &wsaData );
	if ( 0 != nErrCode ){
		AfxMessageBox("��ʼ��SOCKETʧ��!");
		return 0;
	}
	
	//�����׽���
	dlg->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == dlg->m_socket){
		AfxMessageBox("����SOCKETʧ��!");
		return 0;
	}
	//�������׽��ֵ�ַ
	SOCKADDR_IN server_addr;
	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(SERVERPORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	if (server_addr.sin_addr.s_addr == INADDR_NONE)
	{   
		struct hostent *host= NULL;
		if ((host = gethostbyname(SERVERADDR)) != NULL)
		{
			memcpy(&(server_addr.sin_addr.s_addr), host->h_addr,host->h_length);
			server_addr.sin_family = host->h_addrtype;
		}
		else{
			AfxMessageBox("������IP��ַ����,����������IP��ַ�Ƿ���ȷ!\n");
			return 0;
		}
	}
	//���ӷ�����
	if (connect(dlg->m_socket,(sockaddr*)&server_addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		dlg->login=TRUE;//Ϊ��ȡ�����ڷ��������ӳɹ�ǰ�͵�½�ĵ�½״̬.
		Sleep(1000);
		dlg->login=FALSE;//��ԭ��½״̬
		dlg->GetDlgItem(IDC_STATIC_ZT)->SetWindowText("״̬: ���ӷ�����ʧ��!��������!");
		dlg->GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_HIDE);
		dlg->GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
		dlg->GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDC_EDIT_PASS)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
		dlg->GetDlgItem(IDC_BUTTON_REG)->EnableWindow(TRUE);
		dlg->m_socket=NULL;
		AfxBeginThread((AFX_THREADPROC)init_socket,dlg);
		return 0;
	}
	dlg->GetDlgItem(IDC_STATIC_ZT)->SetWindowText("״̬: ���ӷ������ɹ�!�ȴ���½!");
	AfxBeginThread((AFX_THREADPROC)recvmsgthread,dlg);
	return 1;
}

CChatclientDlg::CChatclientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatclientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChatclientDlg)
	m_socket=NULL;
	m_id = _T("");
	m_passwd = _T("");
	m_zt = _T("");
	m_client=0;
	login=FALSE;
	sound=TRUE;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChatclientDlg)
	DDX_Control(pDX, IDC_LIST_USER, m_onLineList);
	DDX_Text(pDX, IDC_EDIT_ID, m_id);
	DDV_MaxChars(pDX, m_id, 30);
	DDX_Text(pDX, IDC_EDIT_PASS, m_passwd);
	DDV_MaxChars(pDX, m_passwd, 30);
	DDX_Text(pDX, IDC_STATIC_ZT, m_zt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChatclientDlg, CDialog)
	//{{AFX_MSG_MAP(CChatclientDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_QUXIAO, OnQuxiao)
	ON_BN_CLICKED(IDC_BUTTON_REG, OnButtonReg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_USER, OnDblclkListUser)
	ON_BN_CLICKED(IDC_BUTTON_SOUND, OnButtonSound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatclientDlg message handlers

BOOL CChatclientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	GetDlgItem(IDC_STATIC_ZXNUM)->SetWindowText("��ǰ��������:0 ��");
	GetDlgItem(IDC_STATIC_ZXNUM)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_USER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_MYNAME)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MYNAME)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_MYID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MYID)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_HIDE);

	m_onLineList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_INFOTIP);
	m_onLineList.InsertColumn(0,"����",LVCFMT_CENTER,80);
	m_onLineList.InsertColumn(1,"ID",LVCFMT_CENTER,60);
	m_onLineList.InsertColumn(2,"��Ϣ״̬",LVCFMT_CENTER,81);

	m_listimagelist.Create(16,16,ILC_COLOR32,10,30);
	HICON hIcon = NULL;	
	hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
	m_listimagelist.Add(hIcon);
	m_onLineList.SetImageList(&m_listimagelist,LVSIL_SMALL);	// ���ù���ӳ���б�

	GetDlgItem(IDC_STATIC_ZT)->SetWindowText("״̬: �ȴ��û���½...");
	// TODO: Add extra initialization here
	m_socket=NULL;
	AfxBeginThread((AFX_THREADPROC)init_socket,this);

	//����ͼ��
	m_tnid.cbSize=sizeof(NOTIFYICONDATA);
	m_tnid.hWnd=this->m_hWnd;
	m_tnid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	m_tnid.uCallbackMessage=MYWM_NOTIFYICON; //�û�����Ļص���Ϣ
	CString szToolTip="MyChat-Client V1.0";
	strcpy(m_tnid.szTip, szToolTip);
	m_tnid.uID=IDR_MAINFRAME;
	hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_tnid.hIcon=hIcon;
	
	::Shell_NotifyIcon(NIM_DELETE,&m_tnid);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CChatclientDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message){
		
	case MYWM_NOTIFYICON:       
		//������û��������Ϣ
		if(lParam==WM_LBUTTONDBLCLK)
		{  //���˫��ʱ�����ڳ���
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW);
			::Shell_NotifyIcon(NIM_DELETE,&m_tnid);
		}
		break;
	case WM_SYSCOMMAND:     
		//�����ϵͳ��Ϣ
		if(wParam==SC_MINIMIZE){ 
			//���յ���С����Ϣʱ����������
			::Shell_NotifyIcon(NIM_ADD,&m_tnid);//��ʾ����ͼ��
			AfxGetApp()->m_pMainWnd->ShowWindow(SW_HIDE);
			return 0;
		}
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatclientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CChatclientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChatclientDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}
BOOL CChatclientDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	WSACleanup();
	return CDialog::DestroyWindow();
}

void CChatclientDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_id==""){
		MessageBox("�������û�ID!","��ʾ");
		return;
	}
	else{
		char *buf=(char*)malloc(30);
		loginmsg.id=atoi((const char*)m_id.GetBuffer(30));
		itoa(loginmsg.id,buf,10);
		if (strcmp(buf,(LPCSTR)m_id.GetBuffer(30)))
				{
					MessageBox("��������ȷ���û�ID,�û�ID��һ��������!","��ʾ");
					free(buf);
					return;
				}
		free(buf);
	}
	if(m_passwd==""){
		MessageBox("����������!","��ʾ");
		return;
	}
	else{
		strncpy(loginmsg.passwd,m_passwd,30);
	}
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);

	memset(&regloginmsg,0,sizeof(regloginmsg));
	regloginmsg.type=LOGIN;
	memcpy(regloginmsg.data,&loginmsg,sizeof(loginmsg));
	send(m_socket,(char*)&regloginmsg,sizeof(regloginmsg),0); 
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);
	mythreaddata *data=(mythreaddata *)malloc(sizeof(struct mythreaddata));
	memcpy(data->recvbuf,"״̬: ��������ύ��½����.",sizeof("״̬: ��������ύ��½����."));
	data->dlg=this;
	AfxBeginThread((AFX_THREADPROC)ztthread,data);
}

void CChatclientDlg::OnQuxiao() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDOK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_QUXIAO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(TRUE);
	login=TRUE;//Ϊ��ȡ������½״̬.
	GetDlgItem(IDC_STATIC_ZT)->SetWindowText("״̬: �ȴ��û���½...");
	Sleep(500);
	login=FALSE;//��ԭ��½״̬
	
}

void CChatclientDlg::OnButtonReg() 
{
	// TODO: Add your control notification handler code here
	memset(&regloginmsg,0,sizeof(regloginmsg));
	CRegDlg regdlg(NULL,this);
	if(IDCANCEL==regdlg.DoModal()) return;
	send(m_socket,(char*)&regloginmsg,sizeof(regloginmsg),0);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_REG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);
	mythreaddata *data=(mythreaddata *)malloc(sizeof(struct mythreaddata));
	memcpy(data->recvbuf,"״̬: ��������ύע������.",sizeof("״̬: ��������ύע������."));
	data->dlg=this;
	AfxBeginThread((AFX_THREADPROC)ztthread,data);
}

void CChatclientDlg::OnDblclkListUser(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	int index=m_onLineList.GetSelectionMark();
	if (index==-1)
	{
		MessageBox("û��ѡ��Ҫ����������û�!","��ʾ",MB_OK);
		return;
	}
	char name[30],id[30];
	m_onLineList.GetItemText(index,0,name,sizeof(name));
	m_onLineList.GetItemText(index,1,id,sizeof(id));
	CChatDlg *chatdlg;
	struct Itemdata *data;
	data=(Itemdata *)m_onLineList.GetItemData(index);
	if (data->chatdlg==NULL)//�Ƿ���˴��� û�оʹ���һ���Ի���
	{
		chatdlg=new CChatDlg;
		chatdlg->Create(IDD_CHAT_DIALOG);
		memcpy(chatdlg->name,name,sizeof(name));
		memcpy(chatdlg->id,id,sizeof(id));
		chatdlg->chatclientDlg=this;
		chatdlg->ShowWindow(SW_SHOW);
		data->chatdlg=chatdlg;
		m_onLineList.SetItemData(index,(DWORD)data);
		m_onLineList.SetItemText(index,2,"");
	}
	else{
		data->chatdlg->SetFocus();//������˴���,�򼤶��Ի���
		data->chatdlg->ShowWindow(SW_SHOWDEFAULT);
	}
	*pResult = 0;
}

void CChatclientDlg::OnButtonSound() 
{
	// TODO: Add your control notification handler code here
	if (sound==true){
		sound=false;
		GetDlgItem(IDC_BUTTON_SOUND)->SetWindowText("��������");
	}
	else{
		sound=true;
		GetDlgItem(IDC_BUTTON_SOUND)->SetWindowText("�ر�����");
	}

}