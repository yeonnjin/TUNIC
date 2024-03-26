#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager : public CBase
{
private:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strOutputText, const _float2& vPosition, _fvector vColor, _float fRadian);

private:
	map<const wstring, class CMyFont*>	m_Fonts;

private:
	class CMyFont* Find_Font(const wstring& strFontTag);

public:
	static CFont_Manager* Create();
	virtual void Free() override;
};

END