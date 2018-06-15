
// AutoVercodeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AutoVercode.h"
#include "AutoVercodeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
bool bmpzoom(const wchar_t* szSrcBmp, const wchar_t* szDstBmp, long, long);
bool RotateBmp(LPCWSTR strSrcFile, LPCWSTR strDestFile, int nAngle);

CCriticalSection g_cs;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


CAutoVercodeDlg::CAutoVercodeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoVercodeDlg::IDD, pParent),m_nBmpWidth(0),m_nBmpHeight(0),m_pBuffer(NULL),
	m_pChar1(NULL),m_nCharWidth(0),m_nDenoise(0), m_pChar2(NULL), m_pChar3(NULL), m_pChar4(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoVercodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORG_BMP, m_stOrgBmp);
	DDX_Control(pDX, IDC_OPT_BMP, m_stOptBmp);
	DDX_Control(pDX, IDC_OPT_CHAR1, m_stChar1);
	DDX_Control(pDX, IDC_OPT_CHAR2, m_stChar2);
	DDX_Control(pDX, IDC_OPT_CHAR3, m_stChar3);
	DDX_Control(pDX, IDC_OPT_CHAR4, m_stChar4);
}

BEGIN_MESSAGE_MAP(CAutoVercodeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_AUTO, &CAutoVercodeDlg::OnBnClickedBtnAuto)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CAutoVercodeDlg::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDC_CHK_L1, &CAutoVercodeDlg::OnBnClickedChkL1)
	ON_BN_CLICKED(IDC_BTN_GENERATE, &CAutoVercodeDlg::OnBnClickedBtnGenerate)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CAutoVercodeDlg ��Ϣ�������

BOOL CAutoVercodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAutoVercodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAutoVercodeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAutoVercodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoVercodeDlg::Denoise()
{
	int i = 0; 
	int j = 0;

	if (m_nDenoise & LEVEL_ISOLATE_PNT)
	{
		
		for (i = 0; i < m_nBmpWidth; i++)
		{
			for (j = 0; j < m_nBmpHeight; j++)
			{
				// ��ȥ�����ĵ�
				if ( (i == 0) || (m_pBuffer[i-1][j] == 0) )	// �ϱߵ�Ϊ0
				{
					if ( (i == (m_nBmpWidth-1)) || (m_pBuffer[i+1][j] == 0) ) // �±ߵ�Ϊ0
					{
						if ( (j == 0) || (m_pBuffer[i][j-1] == 0) ) // ��ߵ�Ϊ0
						{
							if ( (j == (m_nBmpHeight-1)) || (m_pBuffer[i][j+1] == 0) ) // �ұߵ�Ϊ0
							{
								m_pBuffer[i][j] = 0;
							}
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// ��ȥ���ŵ�����
		// ��ȥ���� 
		int count = 0;
		int iStart = 0, iEnd = 0;
		for (i = 0; i < m_nBmpWidth; i++)
		{
			count = 0;
			for (j = 0; j < m_nBmpHeight; j++)
			{
				if (m_pBuffer[i][j] != 0)
				{
					count ++;
				}
				//else
				//{
				//	if (count >= (m_nBmpWidth/2))	// �����ĺ��
				//	{
				//		iEnd = j;
				//		break;
				//	}
				//	else
				//	{
				//		count = 0;
				//		iStart = j;
				//	}
				//}
			}
			if (count >= (m_nBmpHeight-5))
			{
				// �µ�ĸ������ߵ�����ɾ�� 
				for (int k = 0; k < m_nBmpHeight; k++)
				{
					if ((i > 0) && (m_pBuffer[i-1][k] != 0))
					{

					}
					else if((i != (m_nBmpWidth-1)) && (m_pBuffer[i+1][k] != 0))
					{

					}
					else
					{
						m_pBuffer[i][k] = 0;
					}

				}
			}
		}
		// ����,��Խ�����ַ����ȼ�Ϊ��������
		for (j = 0; j < m_nBmpHeight; j++)
		{
			count = 0;
			for (i = 0; i < m_nBmpWidth; i++)
			{
				if (m_pBuffer[i][j] != 0)
				{
					count ++;
				}
				//else
				//{
				//	if (count >= (m_nBmpHeight-5))		// �������ݵ�
				//	{
				//		iEnd = i;
				//		break;
				//	}
				//	else
				//	{
				//		count = 0;
				//		iStart = i;
				//	}
				//}
			}
			if (count >= (m_nBmpWidth/2))
			{
				// �����ĸ��ź��ߵ�����ɾ�� 
				for (int k = 0; k < m_nBmpWidth; k++)
				{
					if ((j > 0) && (m_pBuffer[k][j-1] != 0))	// Ϊ�����ĵ�
					{

					}
					else if((j != (m_nBmpHeight-1)) && (m_pBuffer[k][j+1] != 0)) 
					{

					}
					else
					{
						m_pBuffer[k][j] = 0;
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// ��β����������0 
		for (i = 0; i < 5; i++)
		{
			for (j = 0; j < m_nBmpWidth; j++)
			{
				m_pBuffer[j][i] = 0;
			}
		}
		for (i = m_nBmpHeight-2; i < m_nBmpHeight; i++)
		{
			for (j = 0; j < m_nBmpWidth; j++)
			{
				m_pBuffer[j][i] = 0;
			}
		}
	}


}

void CAutoVercodeDlg::ShowSplitedImg()
{

	m_nCharWidth = m_nBmpWidth / 4;		// ���ĸ���ĸ�Ŀ�Ⱦ���


	m_pChar1 = new int *[m_nCharWidth];
	m_pChar2 = new int *[m_nCharWidth];
	m_pChar3 = new int *[m_nCharWidth];
	m_pChar4 = new int *[m_nCharWidth];
	for (int i = 0; i < m_nCharWidth; i++)
	{
		m_pChar1[i] = new int[m_nBmpHeight];
		m_pChar2[i] = new int[m_nBmpHeight];
		m_pChar3[i] = new int[m_nBmpHeight];
		m_pChar4[i] = new int[m_nBmpHeight];
	}

	int tmp = 0;
	for (int j = 0, l = 0; l < m_nCharWidth; l++,j++)
	{
		for (int k = 0; k < m_nBmpHeight; k++)
		{
			tmp = m_pBuffer[j][k];
			m_pChar1[l][k] = tmp;
		}
	}
	for (int j = m_nCharWidth, l = 0; l < m_nCharWidth; l++,j++)
	{
		for (int k = 0; k < m_nBmpHeight; k++)
		{
			m_pChar2[l][k] = m_pBuffer[j][k];
		}
	}
	for (int j = 2*m_nCharWidth, l = 0; l < m_nCharWidth; l++,j++)
	{
		for (int k = 0; k < m_nBmpHeight; k++)
		{
			m_pChar3[l][k] = m_pBuffer[j][k];
		}
	}
	for (int j = 3*m_nCharWidth, l = 0; l < m_nCharWidth; l++,j++)
	{
		for (int k = 0; k < m_nBmpHeight; k++)
		{
			m_pChar4[l][k] = m_pBuffer[j][k];
		}
	}

	int avg = 0;
	CRect rect1, rect2, rect3, rect4;
	CImage img1, img2, img3, img4;
	img1.Create(m_nCharWidth, m_nBmpHeight, 24);
	img2.Create(m_nCharWidth, m_nBmpHeight, 24);
	img3.Create(m_nCharWidth, m_nBmpHeight, 24);
	img4.Create(m_nCharWidth, m_nBmpHeight, 24);
	for (int i = 0; i < m_nCharWidth; i++)
	{
		for (int j = 0; j < m_nBmpHeight; j++)
		{
			avg = m_pChar1[i][j];
			img1.SetPixelRGB(i, j, avg, avg, avg);

			avg = m_pChar2[i][j];
			img2.SetPixelRGB(i, j, avg, avg, avg);

			avg = m_pChar3[i][j];
			img3.SetPixelRGB(i, j, avg, avg, avg);

			avg = m_pChar4[i][j];
			img4.SetPixelRGB(i, j, avg, avg, avg);
		}
	}
	m_stChar1.GetClientRect(&rect1);
	m_stChar2.GetClientRect(&rect2);
	m_stChar3.GetClientRect(&rect3);
	m_stChar4.GetClientRect(&rect4);

	CDC *pDC = m_stChar1.GetWindowDC();
	img1.Draw(pDC->m_hDC, rect1);
	ReleaseDC(pDC);


	pDC = m_stChar2.GetWindowDC();
	img2.Draw(pDC->m_hDC, rect2);
	ReleaseDC(pDC);

	pDC = m_stChar3.GetWindowDC();
	img3.Draw(pDC->m_hDC, rect3);
	ReleaseDC(pDC);

	pDC = m_stChar4.GetWindowDC();
	img4.Draw(pDC->m_hDC, rect4);
	ReleaseDC(pDC);
}

void CAutoVercodeDlg::OnBnClickedBtnAuto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_stOptBmp.RedrawWindow();

	CRect rect;
	m_stOptBmp.GetClientRect(&rect);     //mPictureΪPicture Control�ؼ���������ÿؼ����������
	CImage image;					   //ʹ��ͼƬ��
	image.Load(m_strFilePath);           //װ��·����ͼƬ��Ϣ��ͼƬ��

	int height = image.GetHeight();
	int width = image.GetWidth();
	COLORREF pixel;
	int avg = 0;
	int r = 0, g = 0, b = 0;

	if (m_pBuffer)
	{
		for (int i = 0; i < m_nBmpHeight; i++)
		{
			delete m_pBuffer[i];
		}
		delete[] m_pBuffer;
	}

	m_nBmpHeight = height;
	m_nBmpWidth = width;
	m_pBuffer = new int *[m_nBmpWidth];
	


	for (int i = 0; i < m_nBmpWidth; i++)
	{
		m_pBuffer[i] = new int[m_nBmpHeight];
	}
	//////////////////////////////////////////////////////////////////////////
	// ��ֵ��ͼƬ
	for (int x=0; x < width; x++) 
	{ 
		for(int y=0; y < height; y++) 
		{ 
			pixel = image.GetPixel(x,y);
			r = GetRValue(pixel);
			avg = (r < 220) ? 255 : 0;

			r = pixel & 0xFF;
			g = (pixel >> 8) & 0xFF;
			b = (pixel >> 16) & 0xFF;

			avg = ((r+g+b) > 500) ? 0 : 255;		// ��ɫ/��ɫ����
			//image.SetPixelRGB(x,y,avg,avg,avg);

			m_pBuffer[x][y] = avg;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// ��ʾ��ֵ�����ͼƬ
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			avg = m_pBuffer[x][y];
			image.SetPixelRGB(x,y,avg,avg,avg);
		}
	}

	CDC* pDC = m_stOptBmp.GetWindowDC(); //�����ʾ�ؼ��ľ��
	image.Draw(pDC->m_hDC,rect);    //ͼƬ���ͼƬ���ƺ���
	ReleaseDC(pDC);	


	Denoise();	// ȥ��

	ShowSplitedImg();	// ��ʾͼƬ4����


	CString strCode;
	strCode += GetCode(m_pChar1);
	strCode += GetCode(m_pChar2);
	strCode += GetCode(m_pChar3);
	strCode += GetCode(m_pChar4);


	SetDlgItemText(IDC_STATIC_CODE, strCode);

	if (m_pBuffer)
	{
		for (int i = 0; i < m_nBmpWidth; i++)
		{
			delete m_pBuffer[i];
		}
		delete[] m_pBuffer;

		m_pBuffer = NULL;
	}

	if (m_pChar1 && m_pChar2 && m_pChar3 && m_pChar4)
	{
		for (int i = 0; i < m_nCharWidth; i++)
		{
			delete m_pChar1[i];
			delete m_pChar2[i];
			delete m_pChar3[i];
			delete m_pChar4[i];
		}
		delete[] m_pChar1;
		delete[] m_pChar2;
		delete[] m_pChar3;
		delete[] m_pChar4;

		m_pChar1 = NULL;
		m_pChar2 = NULL;
		m_pChar3 = NULL;
		m_pChar4 = NULL;
	}

}

wchar_t CAutoVercodeDlg::GetCode(int **pChar)
{
	map<wchar_t, vector<unsigned char> >::const_iterator cit = m_mapPatterns.begin();
	vector<unsigned char>::const_iterator vit;
	int patCount = 0; // �ַ��������Ŀ,��0xFF������
	int totalCount = 0;
	int count = 0;
	double similar = 0.0;
	double totalSimi = 0.0;
	map<double, wchar_t> results; 

	for (; cit != m_mapPatterns.end(); cit++)
	{
		vit = cit->second.begin();
		count = 0;
		patCount = 0;
		totalCount = 0;for (int i = 0; i < m_nCharWidth; i++)
		{
			for (int j = 0; j < m_nBmpHeight; j++)
			{
				if (*vit == 0xFF)
				{
					patCount++;
					if (*vit == pChar[i][j])
					{
						count++;
					}
				}
				if (*vit == pChar[i][j])
				{
					totalCount++;
				}
				vit++;
			}
		}
		totalSimi = (totalCount * 1.0) / (m_nCharWidth*m_nBmpHeight * 1.0);
		similar = (count * 1.0) / (patCount);
		if ((totalSimi >= 0.8) && (similar >= 0.9))
		{
			results.insert(pair<double, wchar_t>(totalSimi+similar, cit->first));
		}
	}
	if (results.empty())
	{
		return TEXT('-');
	}
	return results.rbegin()->second;	// ����ƥ�����ߵ�
}

void CAutoVercodeDlg::OnBnClickedBtnBrowse()
{
	//�Ի����� �ı�����ļ�,  _T("*.bmp")�� �򿪵ĶԻ����е�Ĭ��ֵ
	CFileDialog dlg(TRUE, NULL, _T("*.bmp"), 
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY, _T("image files (*.bmp) |*.bmp |"), this);  

	if(dlg.DoModal() != IDOK)  //����û����ȷ�� DoModal�����ǻ�ȡ�û�ѡ����
	{
		return;
	}

	m_strFilePath = dlg.GetPathName();

	CRect rect;                  //��������������
	m_stOrgBmp.GetClientRect(&rect);     //mPictureΪPicture Control�ؼ���������ÿؼ����������
	CImage image;					   //ʹ��ͼƬ��
	image.Load(m_strFilePath);           //װ��·����ͼƬ��Ϣ��ͼƬ��
	
	CDC* pDC = m_stOrgBmp.GetWindowDC(); //�����ʾ�ؼ��ľ��
	image.Draw(pDC->m_hDC,rect);    //ͼƬ���ͼƬ���ƺ���
	ReleaseDC(pDC);		

	m_stOptBmp.RedrawWindow();
}


void CAutoVercodeDlg::OnBnClickedChkL1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_CHK_L1))->GetCheck() == BST_CHECKED)
	{
		m_nDenoise |= LEVEL_ISOLATE_PNT;
	}
	else
	{
		m_nDenoise &= ~LEVEL_ISOLATE_PNT;
	}
}
//////////////////////////////////////////////////////////////////////////
// ����ģ���Թ�ƥ��
void CAutoVercodeDlg::GenerateBinPattern(CString strPath, CString strTitle)
{
	CImage image;
	image.Load(strPath);
	vector<unsigned char> tmpVec;

	if (image.IsNull())
	{
		return;
	}
	int width = image.GetWidth();
	int height = image.GetHeight();

	CString strPatternPath = TEXT("patterns\\");
	strPatternPath += strTitle;
	strPatternPath += TEXT(".pat");

	FILE *pFile;
	_wfopen_s(&pFile, strPatternPath, TEXT("wb"));

	COLORREF pixel;
	int total = 0;
	unsigned char pix = 0x00;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			pixel = image.GetPixel(i, j);
			
			total = pixel & 0xFF;
			total += (pixel >> 8) & 0xFF;
			total += (pixel >> 16) & 0xFF;
			
			pix = (total < 100) ? 0xFF : 0x00;
			
			tmpVec.push_back(pix);

			fwrite(&pix, 1, 1, pFile);
		}
	}
	
	fclose(pFile);

	m_mapPatterns.insert(pair<wchar_t, vector<unsigned char> >(strTitle.GetAt(0), tmpVec));

	image.Destroy();
}

void CAutoVercodeDlg::OnBnClickedBtnGenerate()
{
	CFileFind tempFind;
	BOOL bFound; //�ж��Ƿ�ɹ��ҵ��ļ�
	bFound = tempFind.FindFile(TEXT("lib\\*.bmp"));  
	m_mapPatterns.clear();

	CString strTmp;   //����ҵ������ļ��� ����ļ���·��
	while (bFound)      //���������ļ�
	{
		bFound = tempFind.FindNextFile(); //��һ��ִ��FindNextFile��ѡ�񵽵�һ���ļ����Ժ�ִ��Ϊѡ��
	
		if (tempFind.IsDots() || tempFind.IsDirectory())   //�ҵ������ļ��У���������ļ����µ��ļ�
		{
			continue;
		}
		else
		{
			GenerateBinPattern(tempFind.GetFilePath(), tempFind.GetFileTitle());
		}
	}
	tempFind.Close();

	
	m_nCharWidth = 15;
	m_nBmpHeight = 20;
	CRect rect1, rect2, rect3, rect4;
	CImage img1, img2, img3, img4;
	img1.Create(m_nCharWidth, m_nBmpHeight, 24);
	img2.Create(m_nCharWidth, m_nBmpHeight, 24);
	img3.Create(m_nCharWidth, m_nBmpHeight, 24);
	img4.Create(m_nCharWidth, m_nBmpHeight, 24);

	FILE *pFile;
	_wfopen_s(&pFile, TEXT("patterns\\0.pat"), TEXT("rb"));
	FILE *pFile2;
	_wfopen_s(&pFile2, TEXT("patterns\\z.pat"), TEXT("rb"));
	
	unsigned char pix;
	int avg;

	for (int i = 0; i < m_nCharWidth; i++)
	{
		for (int j = 0; j < m_nBmpHeight; j++)
		{
			fread_s(&pix, 1, 1, 1, pFile);
			avg = (pix == 0xFF) ? 0 : 255;
			img1.SetPixelRGB(i, j, avg, avg, avg);

			fread_s(&pix, 1, 1, 1, pFile2);
			avg = (pix == 0xFF) ? 0 : 255;
			img4.SetPixelRGB(i, j, avg, avg, avg);

			img2.SetPixel(i, j, RGB(255, 255, 255));
			img3.SetPixel(i, j, RGB(255, 255, 255));
		}
	}


	for (int i = 0; i < m_nCharWidth; i++)
	{
		img2.SetPixel(i, 0, RGB(0, 0, 0));
	}
	for (int i = 0; i < m_nBmpHeight; i++)
	{
		img3.SetPixel(0, i, RGB(0,0,0));
	}
	fclose(pFile);
	fclose(pFile2);

	m_stChar1.GetClientRect(&rect1);
	m_stChar2.GetClientRect(&rect2);
	m_stChar3.GetClientRect(&rect3);
	m_stChar4.GetClientRect(&rect4);

	CDC* pDC = m_stChar1.GetWindowDC(); //�����ʾ�ؼ��ľ��
	img1.Draw(pDC->m_hDC,rect1);    //ͼƬ���ͼƬ���ƺ���
	ReleaseDC(pDC);	

	pDC = m_stChar2.GetWindowDC();
	img2.Draw(pDC->m_hDC, rect2);
	ReleaseDC(pDC);

	pDC = m_stChar3.GetWindowDC();
	img3.Draw(pDC->m_hDC, rect3);
	ReleaseDC(pDC);

	pDC = m_stChar4.GetWindowDC();
	img4.Draw(pDC->m_hDC, rect4);
	ReleaseDC(pDC);	
}

void CAutoVercodeDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ


	CDialogEx::OnClose();
}
