/**
 * @file CalibrationDlg.cpp
 * @brief 
 */

/*****************************************************************************
 * Includes
 *****************************************************************************/

#include <afxwin.h>		/* AfxMessageBox */
#include <afx.h>
#include "app/stdafx.h"
#include "app/AppCore.h"
#include "app/EyeTrackingDlg.h"
#include "CalibrationDlg.h"
#include "utils/Utils.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/
#define DEFAULT_SEPARATION_HORIZONTAL (30.0)
#define DEFAULT_SEPARATION_VERTICAL (15.0)

/*****************************************************************************
 * Member Functions
 *****************************************************************************/

IMPLEMENT_DYNAMIC(CCalibrationDlg, CDialog)
CCalibrationDlg::CCalibrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCalibrationDlg::IDD, pParent), m_oldHSeparation(-1.0), m_oldVSeparation(-1.0)
{}

CCalibrationDlg::~CCalibrationDlg()
{}

void CCalibrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EditSize, m_Size);
	DDX_Control(pDX, IDC_HSeparation, m_HSeparation);
	DDX_Control(pDX, IDC_VSeparation, m_VSeparation);
	DDX_Control(pDX, IDC_CalibrationDefaultSeparation, m_DefaultSeparation);
	DDX_Control(pDX, IDC_Check_ShowAll, m_ShowAll);
	DDX_Control(pDX, IDC_EYE_LIST, m_EyeList);
	DDX_Control(pDX, IDC_STATIC_DEBUG, m_Debug);
	DDX_Control(pDX, IDC_AcceptDelay, m_AcceptDelay);
	DDX_Control(pDX, IDC_CHECK_CYCLE, m_CheckCycle);
	DDX_Control(pDX, IDC_Dot1, m_Dot1);
	DDX_Control(pDX, IDC_Dot2, m_Dot2);
	DDX_Control(pDX, IDC_Dot3, m_Dot3);
	DDX_Control(pDX, IDC_Dot4, m_Dot4);
	DDX_Control(pDX, IDC_Dot5, m_Dot5);
	DDX_Control(pDX, IDC_Dot6, m_Dot6);
	DDX_Control(pDX, IDC_Dot7, m_Dot7);
	DDX_Control(pDX, IDC_Dot8, m_Dot8);
	DDX_Control(pDX, IDC_Dot9, m_Dot9);
}

BEGIN_MESSAGE_MAP(CCalibrationDlg, CDialog)
	ON_BN_CLICKED(IDC_CalibrationDefaultSeparation, OnBnClickedDefaultSeparation)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_CYCLE, OnBnClickedCheckCycle)
	ON_BN_CLICKED(IDC_Check_ShowAll, OnBnClickedCheckShowall)
END_MESSAGE_MAP()

BOOL CCalibrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EyeList.ResetContent();
	m_EyeList.AddString("Left");
	m_EyeList.AddString("Right");
	m_EyeList.AddString("Both");

	load();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCalibrationDlg::load() 
{
	m_HSeparation.SetWindowText(etu::FloatToString(g_pCalibration->getHSeparation()));
	m_VSeparation.SetWindowText(etu::FloatToString(g_pCalibration->getVSeparation()));
	m_Size.SetWindowText(etu::FloatToString(g_pCalibration->getSize()));
	m_AcceptDelay.SetWindowText(etu::IntToString(g_pCalibration->getAcceptDelay()));
	m_EyeList.SetCurSel(g_pCalibration->getEye());

	if (g_pCalibration->showAll())
		m_ShowAll.SetCheck(BST_CHECKED);
	else 
		m_ShowAll.SetCheck(BST_UNCHECKED);

	if (g_pCalibration->getCycle()) {
		m_CheckCycle.SetCheck(BST_CHECKED);
	} 
	else {
		m_CheckCycle.SetCheck(BST_UNCHECKED);
	}

	CCalibrationDlg::OnBnClickedCheckShowall();
}

void CCalibrationDlg::save() 
{
	char buffer[100];

	m_HSeparation.GetWindowText(buffer, 100);
	g_pCalibration->setHSeparation(etu::StringToFloat(buffer, g_pCalibration->getHSeparation()));
	
	m_VSeparation.GetWindowText(buffer, 100);
	g_pCalibration->setVSeparation(etu::StringToFloat(buffer, g_pCalibration->getVSeparation()));

	m_Size.GetWindowText(buffer, 100);
	g_pCalibration->setSize(etu::StringToFloat(buffer, g_pCalibration->getSize()));

	m_AcceptDelay.GetWindowText(buffer, 100);
	g_pCalibration->setAcceptDelay(etu::StringToInt(buffer, g_pCalibration->getAcceptDelay()));
	g_pCalibration->setEye(m_EyeList.GetCurSel());
	g_pCalibration->setShowAll(m_ShowAll.GetCheck() == BST_CHECKED);
	g_pCalibration->setCycle(m_CheckCycle.GetCheck() == BST_CHECKED);
	
	g_pCalibration->sendSettings(g_pSettings->theSerialComm);
}

void CCalibrationDlg::OnBnClickedDefaultSeparation()
{
	bool isOn = (m_DefaultSeparation.GetCheck() == BST_CHECKED);

	m_HSeparation.EnableWindow(!isOn);
	m_VSeparation.EnableWindow(!isOn);

	if (isOn) {
		if ((m_oldHSeparation < 0) && (m_oldVSeparation < 0)) {
			m_oldHSeparation = g_pCalibration->getHSeparation();
			m_oldVSeparation = g_pCalibration->getVSeparation();
		}

		m_HSeparation.SetWindowText(etu::FloatToString(DEFAULT_SEPARATION_HORIZONTAL));
		m_VSeparation.SetWindowText(etu::FloatToString(DEFAULT_SEPARATION_VERTICAL));
	}
	else {
		if ((m_oldHSeparation >= 0) && (m_oldVSeparation >= 0)) {
			m_HSeparation.SetWindowText(etu::FloatToString(m_oldHSeparation));
			m_VSeparation.SetWindowText(etu::FloatToString(m_oldVSeparation));
			m_oldHSeparation = -1.0;
			m_oldVSeparation = -1.0;
		}
	}
}

//apply changes button
void CCalibrationDlg::OnBnClickedButton1()
{
	if (g_pActiveStimulus && g_pActiveStimulus->StimulusType() != "Calibration Stimulus") {
		g_pEyeTrackingDlg->OnBnClickedRun();
	}

	save();
}

void CCalibrationDlg::setDebugString(CString str) 
{
	char buffer[256];
	sprintf(buffer, "%s\0", str);
	m_Debug.SetWindowText(buffer);
}

void CCalibrationDlg::OnBnClickedOk()
{	
	OnBnClickedButton1();
}

void CCalibrationDlg::OnBnClickedCancel()
{}

void CCalibrationDlg::OnBnClickedCheckCycle()
{}

void CCalibrationDlg::OnBnClickedCheckShowall()
{
	m_Dot1.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot2.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot3.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot4.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot5.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot6.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot7.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot8.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
	m_Dot9.EnableWindow(!(m_ShowAll.GetCheck() == BST_CHECKED));
}
