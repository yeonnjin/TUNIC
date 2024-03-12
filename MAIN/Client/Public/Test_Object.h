#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CTest_Object final : public CGameObject
{
public:
	typedef struct Test_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4		TransformMatrix;
		wstring			strModelComTag;
	}TEST_DESC;

private:
	CTest_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTest_Object(const CTest_Object& rhs);
	virtual ~CTest_Object() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	wstring			m_strModelComTag;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTest_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END