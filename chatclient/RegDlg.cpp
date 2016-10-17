// RegDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chatclient.h"
#include "RegDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegDlg dialog


CRegDlg::CRegDlg(CWnd* pParent,CChatclientDlg* pchatclientDlg)
	: CDialog(CRegDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegDlg)
	m_passwd = _T("");
	m_name = _T("");
	m_passwd2 = _T("");
	//}}AFX_DATA_INIT
	chatclientDlg=pchatclientDlg;
}


void CRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegDlg)
	DDX_Text(pDX, IDC_EDIT_PASS, m_passwd);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_PASS2, m_passwd2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegDlg, CDialog)
	//{{AFX_MSG_MAP(CRegDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegDlg message handlers

void CRegDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_name==""){
		MessageBox("�������ǳ�!","��ʾ");
		return;
	}
	else{
		strncpy(regmsg.username,m_name,30);
	}
	if(m_passwd==""){
		MessageBox("����������!","��ʾ");
		return;
	}
	if(m_passwd2==""){
		MessageBox("���ظ���������!","��ʾ");
		return;
	}
	if (m_name.GetLength()>30||m_passwd.GetLength()>30||m_passwd2.GetLength()>30)
	{
		MessageBox("�û��������벻�ܳ���30���ַ�!","��ʾ");
		return;
	}
	if(strcmp(m_passwd.GetBuffer(30),m_passwd2.GetBuffer(30))){
		MessageBox("�����������벻һֵ,����������!","��ʾ");
		m_passwd="";
		m_passwd2="";
		UpdateData(TRUE);
		return;
	}
	else{
		strncpy(regmsg.passwd,m_passwd,30);
	}
	chatclientDlg->regloginmsg.type=USERREG;
	memcpy(chatclientDlg->regloginmsg.data,&regmsg,sizeof(regmsg));
	CDialog::OnOK();
}

void CRegDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
