
// AutoVercodeDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <map>
#include <vector>
#include <set>
#include "afxcmn.h"
using std::pair;
using std::vector;
using std::map;
using std::set;
using std::multimap;

enum
{
	LEVEL_ISOLATE_PNT = 0x0001,
};


// CAutoVercodeDlg 对话框
class CAutoVercodeDlg : public CDialogEx
{
// 构造
public:
	CAutoVercodeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AUTOVERCODE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CString m_strFilePath;
	int m_nBmpWidth;
	int m_nBmpHeight;
	int m_nCharWidth;
	int m_nDenoise;
	CString m_wsCodes[4];
	int **m_pBuffer;
	int **m_pChar1;
	int **m_pChar2;
	int **m_pChar3;
	int **m_pChar4;


	void Denoise();
	void ShowSplitedImg();

	void GenerateBinPattern(CString strPath, CString strTitle);
	wchar_t GetCode(int **pChar);

	map<wchar_t, vector<unsigned char> > m_mapPatterns;

public:
	CStatic m_stOrgBmp;
	CStatic m_stOptBmp;
	afx_msg void OnBnClickedBtnAuto();
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedChkL1();
	CStatic m_stChar1;
	CStatic m_stChar2;
	CStatic m_stChar3;
	CStatic m_stChar4;
	afx_msg void OnBnClickedBtnGenerate();

	afx_msg void OnClose();
};
