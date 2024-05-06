#include "stdafx.h"
#include "Weapon_Wand.h"
#include "Effect_WandBeam.h"

#include "Bone.h"

CWeapon_Wand::CWeapon_Wand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Weapon{ pDevice, pContext }
{
}

CWeapon_Wand::CWeapon_Wand(const CWeapon_Wand& rhs)
	: CPlayer_Weapon{ rhs }
{
}

HRESULT CWeapon_Wand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Wand::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eType = OBJ_END;

	//m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));

    return S_OK;
}

HRESULT CWeapon_Wand::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

    if (true == m_isAttackFrame)
    {
        CEffect_WandBeam::EFFECT_WANDBEAM_DESC tDesc = {};
        memcpy(&tDesc.vStartPosition, &m_WorldMatrix.m[3], sizeof(_float4));
        //tDesc.vStartPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
        tDesc.vLookDir = m_vDir;

        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Effect_WandBeam"), &tDesc)))
            return E_FAIL;

        m_isAttackFrame = false;
    }

	return S_OK;
}

void CWeapon_Wand::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Wand::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0, TEX_DIFFUSE)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    m_pModelCom->Render(0);

    if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 1, TEX_DIFFUSE)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    m_pModelCom->Render(1);

    return S_OK;
}

HRESULT CWeapon_Wand::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Wandbow";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(1.8f, 0.4f, 0.4f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Wand::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlDiffuse", &m_vMtrlDiffuse, sizeof(_vector))))
        return E_FAIL;

    return S_OK;
}

CWeapon_Wand* CWeapon_Wand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Wand* pInstance = new CWeapon_Wand(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CWeapon_Wand"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Wand::Clone(void* pArg)
{
    CWeapon_Wand* pInstance = new CWeapon_Wand(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CWeapon_Wand"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Wand::Free()
{
    __super::Free();
}

void CWeapon_Wand::Collision_Event(Engine::CGameObject* pGameObject)
{
}