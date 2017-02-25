// demo_MFCDlg.cpp : implementation file

#include "stdafx.h"
#include "demo_MFC.h"
#include "demo_MFCDlg.h"
#include "afxdialogex.h"
#include <string> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Load SDK library
#ifdef _WIN64
#pragma comment(lib, "../../x64/NeuronDataReader.lib")//Add Lib
#elif defined _WIN32
#pragma comment(lib, "../../x86/NeuronDataReader.lib")//Add Lib
#endif

using namespace std;
ofstream myfile;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cdemo_MFCDlg dialog

Cdemo_MFCDlg::Cdemo_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cdemo_MFCDlg::IDD, pParent)	
	, m_strIPAddress(_T(""))
	, m_strTCPPort(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	sockTCPRef = NULL;
}

void Cdemo_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT1, m_strIPAddress);
	DDX_Text(pDX, IDC_TEXT2, m_strTCPPort);
	
	DDX_Control(pDX, IDC_COMBO_SELECTION_BONE_INDEX, m_wndComBoxBone);
}

BEGIN_MESSAGE_MAP(Cdemo_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &Cdemo_MFCDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TCP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonTcpConnection)
	//ON_BN_CLICKED(IDC_BUTTON_UDP_CONNECTION, &Cdemo_MFCDlg::OnBnClickedButtonUdpConnection)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX, &Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex)
    ON_MESSAGE( WM_UPDATE_MESSAGE, OnUpdateMessage )
	//ON_CBN_SELCHANGE(IDC_COMBO_SELECTION_BONE_INDEX2, &Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex2)
END_MESSAGE_MAP()


// Cdemo_MFCDlg message handlers

BOOL Cdemo_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
    
	// TODO: Add extra initialization here
	BRRegisterFrameDataCallback(this, bvhFrameDataReceived);

    BRRegisterCalculationDataCallback(this, CalcFrameDataReceive);

	m_strIPAddress = L"127.0.0.1";//Default IP Address
	m_strTCPPort = L"7001";		//Default Port

	//m_strUDPPort = L"7001";		// Default UDP Port
	    
    UpdateBvhDataShowUI();	//update the BoneID index when BVH data received
	//UpdateCalcDataShowUI();  //update the BoneID index when Calc data received

	UpdateData(FALSE);//update the Default IP Address and Port, and the Default UDP Port

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void __stdcall Cdemo_MFCDlg::bvhFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
	pthis->showBvhBoneInfo(sender, header, data);
	//myfile<< to_string(data[1]), to_string(data[2]), to_string(data[3]), to_string(data[4]), to_string(data[5]), to_string(data[6]),"\n";
}

void __stdcall Cdemo_MFCDlg::CalcFrameDataReceive( void* customedObj, SOCKET_REF sender, CalcDataHeader* header, float* data )
{
    Cdemo_MFCDlg* pthis = (Cdemo_MFCDlg*)customedObj;
    //pthis->showCalcBoneInfo( sender, header, data );    
}

void Cdemo_MFCDlg::showBvhBoneInfo(SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	USES_CONVERSION;
	char s[256];

	// show frame index
	char strFrameIndex[60];
	_itoa_s(header->FrameIndex, strFrameIndex, 10);		//int transform into string by decimalism
	GetDlgItem(IDC_STATIC_FRAME_INDEX)->SetWindowText(A2W(strFrameIndex));

	// calculate data index for selected bone
	int dataIndex = 0;
    int curSel = m_wndComBoxBone.GetCurSel();   // Gets the currently selected option in the drop down box
    if ( curSel == CB_ERR ) return;

	if (header->WithDisp)
	{
		dataIndex = curSel * 6;
		if (header->WithReference)
		{
			dataIndex += 6;
		}

		float dispX = data[dataIndex + 0];
		float dispY = data[dataIndex + 1];
		float dispZ = data[dataIndex + 2];

		char strBuff[32];
		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
		GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
		GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
		GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

		float angX = data[dataIndex + 4];
		float angY = data[dataIndex + 3];
		float angZ = data[dataIndex + 5];

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
		GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
		GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

		sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
		GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));

		

		char strBvhData[64];
		sprintf_s(strBvhData, "%0.3f\t", dispX);
		sprintf_s(strBvhData, "%0.3f\t", dispY);
		sprintf_s(strBvhData, "%0.3f\t", dispZ);

		sprintf_s(strBvhData, "%0.3f\t", angX);
		sprintf_s(strBvhData, "%0.3f\t", angY);
		sprintf_s(strBvhData, "%0.3f\t", angZ);
		//myfile << dispX, dispX, dispX, angX, angY, angZ, "\n";
		
		//myfile << to_string(data[41]), to_string(data[42]), to_string(data[43]), to_string(data[44]), to_string(data[45]), to_string(data[46]), "\n";
		//myfile <<data[dataIndex + 4] << ", " << data[dataIndex + 3] << "," << data[dataIndex + 5]<<"," << "\n";
		//myfile << "1" << "," << data[dataIndex + 4] << "," << data[dataIndex + 3] << "," << data[dataIndex + 5] << "," << data[dataIndex + 10] << "," << data[dataIndex + 9] << "," << data[dataIndex + 11] << "," << data[dataIndex + 16] << "," << data[dataIndex + 15] << "," << data[dataIndex + 17] << "," << data[dataIndex + 22] << "," << data[dataIndex + 21] << "," << data[dataIndex + 23] << "," << data[dataIndex + 28] << "," << data[dataIndex + 27] << "," << data[dataIndex + 29] << "," << data[dataIndex + 34] << "," << data[dataIndex + 33] << "," << data[dataIndex + 35] << "," << data[dataIndex + 40] << "," << data[dataIndex + 39] << "," << data[dataIndex + 41] << "," << data[dataIndex + 46] << "," << data[dataIndex + 45] << "," << data[dataIndex + 47] << "," << data[dataIndex + 52] << "," << data[dataIndex + 51] << "," << data[dataIndex + 53] << "," << data[dataIndex + 58] << "," << data[dataIndex + 57] << "," << data[dataIndex + 59] << "," << data[dataIndex + 64] << "," << data[dataIndex + 63] << "," << data[dataIndex + 65] << "," << data[dataIndex + 70] << "," << data[dataIndex + 69] << "," << data[dataIndex + 71] << "," << data[dataIndex + 76] << "," << data[dataIndex + 75] << "," << data[dataIndex + 77] << "," << data[dataIndex + 82] << "," << data[dataIndex + 81] << "," << data[dataIndex + 83] << "," << data[dataIndex + 88] << "," << data[dataIndex + 87] << "," << data[dataIndex + 89] << "," << data[dataIndex + 94] << "," << data[dataIndex + 93] << "," << data[dataIndex + 95] << "," << data[dataIndex + 100] << "," << data[dataIndex + 99] << "," << data[dataIndex + 101] << "," << data[dataIndex + 106] << "," << data[dataIndex + 105] << "," << data[dataIndex + 107] << "," << data[dataIndex + 112] << "," << data[dataIndex + 111] << "," << data[dataIndex + 113] << "," << data[dataIndex + 118] << "," << data[dataIndex + 117] << "," << data[dataIndex + 119] << "," << data[dataIndex + 124] << "," << data[dataIndex + 123] << "," << data[dataIndex + 125] << "," << data[dataIndex + 130] << "," << data[dataIndex + 129] << "," << data[dataIndex + 131] << "," << data[dataIndex + 136] << "," << data[dataIndex + 135] << "," << data[dataIndex + 137] << "," << data[dataIndex + 142] << "," << data[dataIndex + 141] << "," << data[dataIndex + 143] << "," << data[dataIndex + 148] << "," << data[dataIndex + 147] << "," << data[dataIndex + 149] << "\n";
		myfile << "1" << "," << data[dataIndex + 4] << "," << data[dataIndex + 3] << "," << data[dataIndex + 5] << "," << data[dataIndex + 10] << "," << data[dataIndex + 9] << "," << data[dataIndex + 11] << "," << data[dataIndex + 16] << "," << data[dataIndex + 15] << "," << data[dataIndex + 17] << "," << data[dataIndex + 22] << "," << data[dataIndex + 21] << "," << data[dataIndex + 23] << "," << data[dataIndex + 28] << "," << data[dataIndex + 27] << "," << data[dataIndex + 29] << "," << data[dataIndex + 34] << "," << data[dataIndex + 33] << "," << data[dataIndex + 35] << "," << data[dataIndex + 40] << "," << data[dataIndex + 39] << "," << data[dataIndex + 41] << "," << data[dataIndex + 46] << "," << data[dataIndex + 45] << "," << data[dataIndex + 47] << "," << data[dataIndex + 52] << "," << data[dataIndex + 51] << "," << data[dataIndex + 53] << "," << data[dataIndex + 58] << "," << data[dataIndex + 57] << "," << data[dataIndex + 59] << "," << data[dataIndex + 64] << "," << data[dataIndex + 63] << "," << data[dataIndex + 65] << "," << data[dataIndex + 70] << "," << data[dataIndex + 69] << "," << data[dataIndex + 71] << "," << data[dataIndex + 76] << "," << data[dataIndex + 75] << "," << data[dataIndex + 77] << "," << data[dataIndex + 82] << "," << data[dataIndex + 81] << "," << data[dataIndex + 83] << "," << data[dataIndex + 88] << "," << data[dataIndex + 87] << "," << data[dataIndex + 89] << "," << data[dataIndex + 94] << "," << data[dataIndex + 93] << "," << data[dataIndex + 95] << "," << data[dataIndex + 100] << "," << data[dataIndex + 99] << "," << data[dataIndex + 101] << "," << data[dataIndex + 106] << "," << data[dataIndex + 105] << "," << data[dataIndex + 107] << "," << data[dataIndex + 112] << "," << data[dataIndex + 111] << "," << data[dataIndex + 113] << "," << data[dataIndex + 118] << "," << data[dataIndex + 117] << "," << data[dataIndex + 119] << "," << "\n";
	}
	else
	{
		if (curSel == 0)
		{
			dataIndex = 0;
			if (header->WithReference)
			{
				dataIndex += 6;
			}

			// show hip's displacement
			float dispX = data[dataIndex + 0];
			float dispY = data[dataIndex + 1];
			float dispZ = data[dataIndex + 2];

			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispX);
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispY);
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", dispZ);
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

			// show hip's angle
			float angX = data[dataIndex + 4];
			float angY = data[dataIndex + 3];
			float angZ = data[dataIndex + 5];

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));


			char strBvhData[64];
			sprintf_s(strBvhData, "%0.3f\t", dispX);
			sprintf_s(strBvhData, "%0.3f\t", dispY);
			sprintf_s(strBvhData, "%0.3f\t", dispZ);

			sprintf_s(strBvhData, "%0.3f\t", angX);
			sprintf_s(strBvhData, "%0.3f\t", angY);
			sprintf_s(strBvhData, "%0.3f\t", angZ);
			//myfile << dispX, dispX, dispX, angX, angY, angZ, "\n";
			//myfile << to_string(data[1]), to_string(data[2]), to_string(data[3]), to_string(data[4]), to_string(data[5]), to_string(data[6]), "\n";
			//myfile <<  data[dataIndex + 4] << ", " << data[dataIndex + 3] << "," << data[dataIndex + 5] << "," << "\n";
			//myfile << "1" << "," << data[dataIndex + 4] << "," << data[dataIndex + 3] << "," << data[dataIndex + 5] << "," << data[dataIndex + 10] << "," << data[dataIndex + 9] << "," << data[dataIndex + 11] << "," << data[dataIndex + 16] << "," << data[dataIndex + 15] << "," << data[dataIndex + 17] << "," << data[dataIndex + 22] << "," << data[dataIndex + 21] << "," << data[dataIndex + 23] << "," << data[dataIndex + 28] << "," << data[dataIndex + 27] << "," << data[dataIndex + 29] << "," << data[dataIndex + 34] << "," << data[dataIndex + 33] << "," << data[dataIndex + 35] << "," << data[dataIndex + 40] << "," << data[dataIndex + 39] << "," << data[dataIndex + 41] << "," << data[dataIndex + 46] << "," << data[dataIndex + 45] << "," << data[dataIndex + 47] << "," << data[dataIndex + 52] << "," << data[dataIndex + 51] << "," << data[dataIndex + 53] << "," << data[dataIndex + 58] << "," << data[dataIndex + 57] << "," << data[dataIndex + 59] << "," << data[dataIndex + 64] << "," << data[dataIndex + 63] << "," << data[dataIndex + 65] << "," << data[dataIndex + 70] << "," << data[dataIndex + 69] << "," << data[dataIndex + 71] << "," << data[dataIndex + 76] << "," << data[dataIndex + 75] << "," << data[dataIndex + 77] << "," << data[dataIndex + 82] << "," << data[dataIndex + 81] << "," << data[dataIndex + 83] << "," << data[dataIndex + 88] << "," << data[dataIndex + 87] << "," << data[dataIndex + 89] << "," << data[dataIndex + 94] << "," << data[dataIndex + 93] << "," << data[dataIndex + 95] << "," << data[dataIndex + 100] << "," << data[dataIndex + 99] << "," << data[dataIndex + 101] << "," << data[dataIndex + 106] << "," << data[dataIndex + 105] << "," << data[dataIndex + 107] << "," << data[dataIndex + 112] << "," << data[dataIndex + 111] << "," << data[dataIndex + 113] << "," << data[dataIndex + 118] << "," << data[dataIndex + 117] << "," << data[dataIndex + 119] << "," << data[dataIndex + 124] << "," << data[dataIndex + 123] << "," << data[dataIndex + 125] << "," << data[dataIndex + 130] << "," << data[dataIndex + 129] << "," << data[dataIndex + 131] << "," << data[dataIndex + 136] << "," << data[dataIndex + 135] << "," << data[dataIndex + 137] << "," << data[dataIndex + 142] << "," << data[dataIndex + 141] << "," << data[dataIndex + 143] << "," << data[dataIndex + 148] << "," << data[dataIndex + 147] << "," << data[dataIndex + 149] << "\n";
			myfile << "1" << "," << data[dataIndex + 4] << "," << data[dataIndex + 3] << "," << data[dataIndex + 5] << "," << data[dataIndex + 10] << "," << data[dataIndex + 9] << "," << data[dataIndex + 11] << "," << data[dataIndex + 16] << "," << data[dataIndex + 15] << "," << data[dataIndex + 17] << "," << data[dataIndex + 22] << "," << data[dataIndex + 21] << "," << data[dataIndex + 23] << "," << data[dataIndex + 28] << "," << data[dataIndex + 27] << "," << data[dataIndex + 29] << "," << data[dataIndex + 34] << "," << data[dataIndex + 33] << "," << data[dataIndex + 35] << "," << data[dataIndex + 40] << "," << data[dataIndex + 39] << "," << data[dataIndex + 41] << "," << data[dataIndex + 46] << "," << data[dataIndex + 45] << "," << data[dataIndex + 47] << "," << data[dataIndex + 52] << "," << data[dataIndex + 51] << "," << data[dataIndex + 53] << "," << data[dataIndex + 58] << "," << data[dataIndex + 57] << "," << data[dataIndex + 59] << "," << data[dataIndex + 64] << "," << data[dataIndex + 63] << "," << data[dataIndex + 65] << "," << data[dataIndex + 70] << "," << data[dataIndex + 69] << "," << data[dataIndex + 71] << "," << data[dataIndex + 76] << "," << data[dataIndex + 75] << "," << data[dataIndex + 77] << "," << data[dataIndex + 82] << "," << data[dataIndex + 81] << "," << data[dataIndex + 83] << "," << data[dataIndex + 88] << "," << data[dataIndex + 87] << "," << data[dataIndex + 89] << "," << data[dataIndex + 94] << "," << data[dataIndex + 93] << "," << data[dataIndex + 95] << "," << data[dataIndex + 100] << "," << data[dataIndex + 99] << "," << data[dataIndex + 101] << "," << data[dataIndex + 106] << "," << data[dataIndex + 105] << "," << data[dataIndex + 107] << "," << data[dataIndex + 112] << "," << data[dataIndex + 111] << "," << data[dataIndex + 113] << "," << data[dataIndex + 118] << "," << data[dataIndex + 117] << "," << data[dataIndex + 119] << "," << "\n";

		}
		else
		{
			dataIndex = 3 + curSel * 3;
			if (header->WithReference)
			{
				dataIndex += 6;
			}

			// show displacement
			char strBuff[32];
			sprintf_s(strBuff, sizeof(strBuff), "NaN");
			GetDlgItem(IDC_STATIC_DISP_X)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Y)->SetWindowText(A2W(strBuff));
			GetDlgItem(IDC_STATIC_DISP_Z)->SetWindowText(A2W(strBuff));

			// show angle
			float angX = data[dataIndex + 1];
			float angY = data[dataIndex + 0];
			float angZ = data[dataIndex + 2];

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angX);
			GetDlgItem(IDC_STATIC_ANGLE_X)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angY);
			GetDlgItem(IDC_STATIC_ANGLE_Y)->SetWindowText(A2W(strBuff));

			sprintf_s(strBuff, sizeof(strBuff), "%0.3f", angZ);
			GetDlgItem(IDC_STATIC_ANGLE_Z)->SetWindowText(A2W(strBuff));
			
			char strBvhData[64];
			
			sprintf_s(strBvhData, "%0.3f\t", angX);
			sprintf_s(strBvhData, "%0.3f\t", angY);
			sprintf_s(strBvhData, "%0.3f\t", angZ);

			//myfile << data[dataIndex + 1] << ", " << data[dataIndex + 0] << "," << data[dataIndex + 2] << "," << "\n";
			//myfile << "1" << "," << data[dataIndex + 1] << "," << data[dataIndex + 0] << "," << data[dataIndex + 2] << "," << data[dataIndex + 7] << "," << data[dataIndex + 6] << "," << data[dataIndex + 8] << "," << data[dataIndex + 13] << "," << data[dataIndex + 12] << "," << data[dataIndex + 14] << "," << data[dataIndex + 19] << "," << data[dataIndex + 18] << "," << data[dataIndex + 20] << "," << data[dataIndex + 25] << "," << data[dataIndex + 24] << "," << data[dataIndex + 26] << "," << data[dataIndex + 31] << "," << data[dataIndex + 30] << "," << data[dataIndex + 32] << "," << data[dataIndex + 37] << "," << data[dataIndex + 36] << "," << data[dataIndex + 38] << "," << data[dataIndex + 43] << "," << data[dataIndex + 42] << "," << data[dataIndex + 44] << "," << data[dataIndex + 49] << "," << data[dataIndex + 48] << "," << data[dataIndex + 50] << "," << data[dataIndex + 55] << "," << data[dataIndex + 54] << "," << data[dataIndex + 56] << "," << data[dataIndex + 61] << "," << data[dataIndex + 60] << "," << data[dataIndex + 62] << "," << data[dataIndex + 67] << "," << data[dataIndex + 66] << "," << data[dataIndex + 68] << "," << data[dataIndex + 73] << "," << data[dataIndex + 72] << "," << data[dataIndex + 74] << "," << data[dataIndex + 79] << "," << data[dataIndex + 78] << "," << data[dataIndex + 80] << "," << data[dataIndex + 85] << "," << data[dataIndex + 84] << "," << data[dataIndex + 86] << "," << data[dataIndex + 91] << "," << data[dataIndex + 90] << "," << data[dataIndex + 92] << "," << data[dataIndex + 97] << "," << data[dataIndex + 96] << "," << data[dataIndex + 98] << "," << data[dataIndex + 103] << "," << data[dataIndex + 102] << "," << data[dataIndex + 104] << "," << data[dataIndex + 109] << "," << data[dataIndex + 108] << "," << data[dataIndex + 110] << "," << data[dataIndex + 115] << "," << data[dataIndex + 114] << "," << data[dataIndex + 116] << "," << data[dataIndex + 121] << "," << data[dataIndex + 120] << "," << data[dataIndex + 122] << "," << data[dataIndex + 127] << "," << data[dataIndex + 126] << "," << data[dataIndex + 128] << "," << data[dataIndex + 133] << "," << data[dataIndex + 132] << "," << data[dataIndex + 134] << "," << data[dataIndex + 139] << "," << data[dataIndex + 138] << "," << data[dataIndex + 140] << "," << data[dataIndex + 145] << "," << data[dataIndex + 144] << "," << data[dataIndex + 146] << "\n";
			myfile << "1"  << "," << data[dataIndex + 1] << "," << data[dataIndex + 0] << "," << data[dataIndex + 2] << "," << data[dataIndex + 7] << "," << data[dataIndex + 6] << "," << data[dataIndex + 8] << "," << data[dataIndex + 13] << "," << data[dataIndex + 12] << "," << data[dataIndex + 14] << "," << data[dataIndex + 19] << "," << data[dataIndex + 18] << "," << data[dataIndex + 20] << "," << data[dataIndex + 25] << "," << data[dataIndex + 24] << "," << data[dataIndex + 26] << "," << data[dataIndex + 31] << "," << data[dataIndex + 30] << "," << data[dataIndex + 32] << "," << data[dataIndex + 37] << "," << data[dataIndex + 36] << "," << data[dataIndex + 38] << "," << data[dataIndex + 43] << "," << data[dataIndex + 42] << "," << data[dataIndex + 44] << "," << data[dataIndex + 49] << "," << data[dataIndex + 48] << "," << data[dataIndex + 50] << "," << data[dataIndex + 55] << "," << data[dataIndex + 54] << "," << data[dataIndex + 56] << "," << data[dataIndex + 61] << "," << data[dataIndex + 60] << "," << data[dataIndex + 62] << "," << data[dataIndex + 67] << "," << data[dataIndex + 66] << "," << data[dataIndex + 68] << "," << data[dataIndex + 73] << "," << data[dataIndex + 72] << "," << data[dataIndex + 74] << "," << data[dataIndex + 79] << "," << data[dataIndex + 78] << "," << data[dataIndex + 80] << "," << data[dataIndex + 85] << "," << data[dataIndex + 84] << "," << data[dataIndex + 86] << "," << data[dataIndex + 91] << "," << data[dataIndex + 90] << "," << data[dataIndex + 92] << "," << data[dataIndex + 97] << "," << data[dataIndex + 96] << "," << data[dataIndex + 98] << "," << data[dataIndex + 103] << "," << data[dataIndex + 102] << "," << data[dataIndex + 104] << "," << data[dataIndex + 109] << "," << data[dataIndex + 108] << "," << data[dataIndex + 110] << "," << data[dataIndex + 115] << "," << data[dataIndex + 114] << "," << data[dataIndex + 116] << "," << "\n";
		}
	}
}

LRESULT Cdemo_MFCDlg::OnUpdateMessage( WPARAM wParam, LPARAM lParam )
{
    UpdateData( FALSE );
    return 0;
}

void Cdemo_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cdemo_MFCDlg::OnPaint()
{
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
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cdemo_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cdemo_MFCDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
	myfile.close();
}

void Cdemo_MFCDlg::OnBnClickedButtonTcpConnection()
{	
	myfile.open("test_case/test4.csv");
	//myfile << "Dx36,Dy36,Dz36,Rx36,Ry36,Rz36,Dx37,Dy37,Dz37,Rx37,Ry37,Rz37,Dx38,Dy38,Dz38,Rx38,Ry38,Rz38,Dx39,Dy39,Dz39,Rx39,Ry39,Rz39,Dx40,Dy40,Dz40,Rx40,Ry40,Rz40,Dx41,Dy41,Dz41,Rx41,Ry41,Rz41,Dx42,Dy42,Dz42,Rx42,Ry42,Rz42,Dx43,Dy43,Dz43,Rx43,Ry43,Rz43,Dx44,Dy44,Dz44,Rx44,Ry44,Rz44,Dx45,Dy45,Dz45,Rx45,Ry45,Rz45,Dx46,Dy46,Dz46,Rx46,Ry46,Rz46,Dx47,Dy47,Dz47,Rx47,Ry47,Rz47,Dx48,Dy48,Dz48,Rx48,Ry48,Rz48,Dx49,Dy49,Dz49,Rx49,Ry49,Rz49,Dx50,Dy50,Dz50,Rx50,Ry50,Rz50,Dx51,Dy51,Dz51,Rx51,Ry51,Rz51,Dx52,Dy52,Dz52,Rx52,Ry52,Rz52,Dx53,Dy53,Dz53,Rx53,Ry53,Rz53,Dx54,Dy54,Dz54,Rx54,Ry54,Rz54,Dx55,Dy55,Dz55,Rx55,Ry55,Rz55,Dx56,Dy56,Dz56,Rx56,Ry56,Rz56,Dx57,Dy57,Dz57,Rx57,Ry57,Rz57,Dx58,Dy58,Dz58,Rx58,Ry58,Rz58,\n";
	myfile << "label,Rx39,Ry39,Rz39,Rx40,Ry40,Rz40,Rx41,Ry41,Rz41,Rx42,Ry42,Rz42,Rx43,Ry43,Rz43,Rx44,Ry44,Rz44,Rx45,Ry45,Rz45,Rx46,Ry46,Rz46,Rx47,Ry47,Rz47,Rx48,Ry48,Rz48,Rx49,Ry49,Rz49,Rx50,Ry50,Rz50,Rx51,Ry51,Rz51,Rx52,Ry52,Rz52,Rx53,Ry53,Rz53,Rx54,Ry54,Rz54,Rx55,Ry55,Rz55,Rx56,Ry56,Rz56,Rx57,Ry57,Rz57,Rx58,Ry58,Rz58,\n";
	
	UpdateData();

	if (sockTCPRef)
	{
		// close socket
		BRCloseSocket(sockTCPRef);

		// reconnect
		sockTCPRef = NULL;

		// change the title of button
		GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Connect");
	}
	else
	{
		USES_CONVERSION;
		
		// get port number
		char* port = W2A(m_strTCPPort);
		long nPort = 0;
		try
		{
			nPort = atoi(port);
		}
		catch (CException* e)
		{
			AfxMessageBox(L"Port number error", MB_OK);
			return;
		}
		
		// connect to remote server
		sockTCPRef = BRConnectTo(W2A(m_strIPAddress), nPort);

		// if success, change the title of button
		if (sockTCPRef)
		{
			GetDlgItem(IDC_BUTTON_TCP_CONNECTION)->SetWindowText(L"Disconnect");
		}
		else
		{
			// if failed, show message
			AfxMessageBox(A2W(BRGetLastErrorMessage()), MB_OK);
		}
	}
}


void Cdemo_MFCDlg::UpdateBvhDataShowUI()
{
    CString BoneID;
    m_wndComBoxBone.ResetContent();		//重置列表框里的内容
    for ( int i = 0; i < BVHBoneCount; i++ )
    {
        BoneID.Format( L"%s%d", L"Bone", i );
        m_wndComBoxBone.AddString( BoneID );
    }
    m_wndComBoxBone.SetCurSel( 0 );
}

void Cdemo_MFCDlg::OnCbnSelchangeComboSelectionBoneIndex()
{
	UpdateData();
}

