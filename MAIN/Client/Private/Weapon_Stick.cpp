#include "stdafx.h"
#include "Weapon_Stick.h"

#include "Bone.h"

CWeapon_Stick::CWeapon_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Weapon{ pDevice, pContext }
{
}

CWeapon_Stick::CWeapon_Stick(const CWeapon_Stick& rhs)
    : CPlayer_Weapon{ rhs }
{
}

HRESULT CWeapon_Stick::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Stick::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_eType = OBJ_PLAYER_WEAPON;

    //m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.0f));

    return S_OK;
}

HRESULT CWeapon_Stick::Tick(_float fTimeDelta)
{
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CWeapon_Stick::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon_Stick::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Stick::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Stick";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(0.2f, 0.2f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Stick::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CWeapon_Stick* CWeapon_Stick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Stick* pInstance = new CWeapon_Stick(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CWeapon_Stick"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Stick::Clone(void* pArg)
{
    CWeapon_Stick* pInstance = new CWeapon_Stick(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CWeapon_Stick"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Stick::Free()
{
    __super::Free();
}

void CWeapon_Stick::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (true == m_isAttackFrame && OBJ_MONSTER == pGameObject->Get_ObjectType())
        pGameObject->Set_isDamage(true);
}
