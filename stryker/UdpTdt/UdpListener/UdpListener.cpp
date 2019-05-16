/**
 * @file UdpListener.cpp
 * @brief Implementation of main application behavior, in CUdpListenerApp.
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

/* If debugging, redefine heap allocation to use debugging features. */
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************************
 * Globals
 *****************************************************************************/

/** The singleton instance of CUdpListenerApp. */
CUdpListenerApp g_udpListenerApp;

/*****************************************************************************
 * Message Map
 *****************************************************************************/

/* Map for message pump, required by MFC. */
BEGIN_MESSAGE_MAP(CUdpListenerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

/*****************************************************************************
 * Implementation
 *****************************************************************************/

/**
 * @brief Create uninitialized instance.
 *
 * Major initialization occurs in the virtual InitInstance function.
 */
CUdpListenerApp::CUdpListenerApp()
{}

/**
 * @brief Tear down.
 */
CUdpListenerApp::~CUdpListenerApp()
{}

/**
 * @brief Initialize the application object.
 *
 * Initializes subsystems and creates and draws the dialog.
 * @return TRUE to start the message pump, FALSE to exit the application.
 */
BOOL CUdpListenerApp::InitInstance()
{
	/* InitCommonControlsEx() is required on Windows XP if an application
	 * manifest specifies use of ComCtl32.dll version 6 or later to enable
	 * visual styles.  Otherwise, any window creation will fail. */
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	/* Create and display dialog. */
	CUdpListenerDlg m_dialog;
	m_dialog.Create(CUdpListenerDlg::IDD, NULL);
	m_pMainWnd = &m_dialog;
	m_dialog.setEventCallback(this);
	BOOL response = m_dialog.ShowWindow(m_nCmdShow); //SW_SHOW

	MSG msg;
	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			// error
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	/*
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	*/

	/* In a dialog app, the stuff happens as a result of displaying the 
	 * dialog in this function, which is why the message pump is run here.
	 * When the function is about to return, the dialog has been closed,
	 * hence return FALSE. */
	return FALSE;
}

/**
 * @brief Respond to the user pressing the start button.
 *
 * Reads the TDT unit from the dialog and looks for a connection to it; tries
 * to establish a connection if none. Reads the port from the dialog and binds 
 * to it. Listens for UDP and handles incoming messages.
 * @param dlg Reference to the CUdpListenerDlg that generated the event.
 */
void CUdpListenerApp::startButtonPressed(const CUdpListenerDlg& dlg)
{
	/* Debugging */
	static eStatus s_status = STATUS_ERROR;

	// TODO: Run UDP listener, update TDT unit & gui w/text
	m_status = s_status;

	/* Debugging -- in one run, see error first, then ping-pong receiving/listening. */
	switch (s_status) {
	case STATUS_ERROR:
		s_status = STATUS_LISTENING;
		break;
	case STATUS_STOPPED:
		s_status = STATUS_ERROR;
		break;
	case STATUS_LISTENING:
		s_status = STATUS_RECEIVING;
		break;
	case STATUS_RECEIVING:
		s_status = STATUS_LISTENING;
		break;
	default:
		s_status = STATUS_ERROR;
		break;
	}
}

/**
 * @brief Respond to user pressing the stop button.
 *
 * Stops listening for UDP.
 * @param dlg Reference to the CUdpListenerDlg that generated the event.
 */
void CUdpListenerApp::stopButtonPressed(const CUdpListenerDlg& dlg)
{
	// TODO: Stop running the listener
	m_status = STATUS_STOPPED;
}

/**
 * @brief Respond to the user pressing the quit button.
 *
 * Stops listening and closes port. Leaves TDT connection untouched.
 * exits.
 * @param dlg Reference to the CUdpListenerDlg that generated the event.
 */
void CUdpListenerApp::quitButtonPressed(const CUdpListenerDlg& dlg)
{
	// TODO: Stop running the app
	this->ExitInstance();
	exit(0);
}

/**
 * @brief Get the current status.
 */
eStatus CUdpListenerApp::getStatus()
{
	return m_status;
}
