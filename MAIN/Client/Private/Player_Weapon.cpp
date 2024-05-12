#include "stdafx.h"
#include "Player_Weapon.h"

#include "Bone.h"

CPlayer_Weapon::CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CPlayer_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Weapon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(nullptr != pArg)
	{
		PLAYER_WEAPON_DESC* pDesc = (PLAYER_WEAPON_DESC*)pArg;

		m_pSocketBone = pDesc->pSocketBone;
		m_eWeapon = pDesc->eWeapon;

		Safe_AddRef(m_pSocketBone);
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon::Tick(_float fTimeDelta)
{
	if (true == m_isUsing)
	{
		__super::Tick(fTimeDelta);

		m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, this);
	}

	return S_OK;
}

void CPlayer_Weapon::Late_Tick(_float fTimeDelta)
{
	if (true == m_isUsing)
	{
		_matrix SocketMatrix = XMLoadFloat4x4(m_pSocketBone->Get_CombinedTransformationMatrix());

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		// 본인의 월드 행렬 * 소켓 본의 최종 행렬 * 부모 본의 행렬
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

#ifdef _DEBUG
		//for (auto& pColliderCom : m_pColliderCom)
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	}
}

HRESULT CPlayer_Weapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}
	return S_OK;
}

HRESULT CPlayer_Weapon::Render_LightDepth() // 	virtual HRESULT			Render_LightDepth() override;
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));

	_vector vShadowEye = pPlayer->Get_ShadowEye();
	vShadowEye.m128_f32[1] += 10.f;
	_vector vShadowLookAt = pPlayer->Get_ShadowLookAt();
	vShadowLookAt.m128_f32[1] += 10.f;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vShadowEye, vShadowLookAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 2000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_Camera_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayer_Weapon::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_Camera_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pSocketBone);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
}