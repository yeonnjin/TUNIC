#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CMap final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_NORMAL, TYPE_SPECULAR, TYPE_END };

public:
	typedef struct Map_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_bool			isLoad;
		_float3			vPosition;
		_float4x4		TransformMatrix;
		wstring			strModelComTag;
	}MAP_DESC;

private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	// File
	MAPOBJFILE* Get_MapObj_File() { Ready_MapObj_File(); return &m_tMapObjFile; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	wstring			m_strModelComTag;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	// FILE
	MAPOBJFILE		m_tMapObjFile;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_MapObj_File();

public:
	static CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END