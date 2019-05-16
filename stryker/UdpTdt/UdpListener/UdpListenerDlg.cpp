/**
 * @file UdpListenerDlg.cpp
 * @brief Implementation of the dialog.
 */

/*****************************************************************************
 * Includes
 *****************************************************************************/

#include "stdafx.h"
#include "UdpListener.h"
#include "UdpListenerDlg.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/

/* If debugging, replace heap allocation operator with a debug version. */
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************************
 * Implementation
 *****************************************************************************/

/** 
 * @brief Initialize self, including parent.
 */
CUdpListenerDlg::CUdpListenerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUdpListenerDlg::IDD, pParent),
	m_callback(NULL), m_cachedStatus(STATUS_STOPPED)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

/**
 * @brief Tie the MFC objects with the dialog controls they implement.
 */
void CUdpListenerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_editLocalPort, editLocalPort);
	DDX_Control(pDX, IDC_editTextOutput, editTextOutput);

	DDX_Control(pDX, IDC_comboTdtUnit, comboTdtUnit);

	DDX_Control(pDX, IDC_buttonStartSend, buttonStartSend);
	DDX_Control(pDX, IDC_buttonStopSend, buttonStopSend);

	DDX_Control(pDX, IDC_progressStatic, progressStatic);

	DDX_Control(pDX, IDC_listOutput, listOutput);
	DDX_Control(pDX, IDC_buttonQuit, buttonQuit);
}

/*
 * Tie callback functions to the MFC dialog controls.
 */
BEGIN_MESSAGE_MAP(CUdpListenerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_buttonStartSend, OnButtonStartSend)
	ON_BN_CLICKED(IDC_buttonStopSend, OnButtonStopSend)
	ON_BN_CLICKED(IDC_buttonQuit, OnClose)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/** 
 * @brief Initialize the dialog and its elements.
 *
 * Called as a result of the WM_INITDIALOG message sent just before the dialog
 * is made visible.
 */
BOOL CUdpListenerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Initialize combo box w/TDT types

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/**
 * @brief Paint the dialog contents or the icon for minimized dialog.
 *
 * If you add a minimize button to your dialog, you will need the code below 
 * to draw the icon.  For MFC applications using the document/view model, 
 * this is automatically done for you by the framework.
 */
void CUdpListenerDlg::OnPaint()
{
	/* This code written by project creation wizard. */
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
	else {
		CDialog::OnPaint();
		setStatusColor(m_cachedStatus);
	}
}

/**
 * @brief Obtain cursor to display while the user drags the minimized window.
 */
HCURSOR CUdpListenerDlg::OnQueryDragIcon()
{
	/* This code written by project creation wizard. */
	return static_cast<HCURSOR>(m_hIcon);
}

/**
 * @brief Handle the Start button being pressed.
 *
 * Called by the MFC framework through message dispatching, this function 
 * brings control into our purview.
 */
void CUdpListenerDlg::OnButtonStartSend()
{
	if (m_callback != NULL) {
		m_callback->startButtonPressed(*this);
		m_cachedStatus = m_callback->getStatus();
	}
}

/**
 * @brief Handle the Stop button being pressed.
 *
 * Called by the MFC framework through message dispatching, this function 
 * brings control into our purview.
 */
void CUdpListenerDlg::OnButtonStopSend()
{
	if (m_callback != NULL) {
		m_callback->stopButtonPressed(*this);	
		m_cachedStatus = m_callback->getStatus();
	}
}

/**
 * @brief Handle the Quit button or "X" button.
 *
 * Called by the MFC framework through message dispatching, this function 
 * brings control into our purview.
 */
void CUdpListenerDlg::OnClose()
{
	if (m_callback != NULL) m_callback->quitButtonPressed(*this);
}

/**
 * @brief Remember new callback.
 *
 * @param callback Non-NULL callback pointer to receive events.
 */
void CUdpListenerDlg::setEventCallback(CEventCallback* callback)
{ 
	if (callback != NULL) m_callback = callback;
}

/**
 * @brief Get the contents of the UDP port field as an int.
 */
int CUdpListenerDlg::getLocalUdpPort() const
{
	CString portString;
	LPTSTR endPtr = NULL;
	int output; 

	editLocalPort.GetWindowText(portString);
	portString.Trim();
	output = (int)_tcstol((LPCTSTR)portString, &endPtr, 10);

	if (endPtr != NULL) {
		AfxMessageBox(_T("UDP port must be a positive integer."));
		output = 12345;
	}

	return output;
}

/**
 * @brief Get the contents of the "Text Received" field.
 */
CString CUdpListenerDlg::getTextReceived() const
{
    // TODO: Get input in Text Received box
	return CString("foo");
}

/**
 * @brief Get the TDT Unit number from the pull-down box.
 */
eTdtUnit CUdpListenerDlg::getTdtUnit() const
{
    // TODO: Get TDT unit
	return TDT_UNIT_NUM_UNITS;
}

/**
 * @brief Populate GUI with text received.
 */
void CUdpListenerDlg::addTextReceived(const CString& text)
{
    // TODO: Add received text to GUI
}

/**
 * @brief Set the color of the status display according to app state.
 */
void CUdpListenerDlg::setStatusColor(eStatus status)
{
	COLORREF color;

	switch (status) {
	case STATUS_STOPPED:
		color = RGB(128,128,128); // Gray
		break;
	case STATUS_LISTENING:
		color = RGB(0,0,255); // Blue
		break;
	case STATUS_RECEIVING:
		color = RGB(0,255,0); // Green
		break;
	case STATUS_ERROR:
		/* Fall through */
	default:
		color = RGB(255,0,0); // Red
		break;
	}

	CRect rect;
	CPaintDC dcp(&progressStatic);
	progressStatic.GetClientRect(rect);
	dcp.FillSolidRect(rect, color);
}
