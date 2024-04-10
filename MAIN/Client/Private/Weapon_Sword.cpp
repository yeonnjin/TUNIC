#include "stdafx.h"
#include "Weapon_Sword.h"

#include "Bone.h"

CWeapon_Sword::CWeapon_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer_Weapon{ pDevice, pContext }
{
}

CWeapon_Sword::CWeapon_Sword(const CWeapon_Sword& rhs)
	: CPlayer_Weapon{ rhs }
{
}

HRESULT CWeapon_Sword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Sword::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_eType = OBJ_PLAYER_WEAPON;

    m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.0f));
}

HRESULT CWeapon_Sword::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Sword::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Sword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Sword::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Sword";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(0.4f, 1.8f, 0.4f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Sword::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CWeapon_Sword* CWeapon_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Sword* pInstance = new CWeapon_Sword(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CWeapon_Sword"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Sword::Clone(void* pArg)
{
    CWeapon_Sword* pInstance = new CWeapon_Sword(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CWeapon_Sword"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Sword::Free()
{
    __super::Free();
}

void CWeapon_Sword::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (true == m_isAttackFrame && OBJ_MONSTER == pGameObject->Get_ObjectType())
        pGameObject->Set_isDamage(true);
}