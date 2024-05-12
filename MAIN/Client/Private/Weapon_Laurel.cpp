#include "stdafx.h"
#include "Weapon_Laurel.h"

CWeapon_Laurel::CWeapon_Laurel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer_Weapon{ pDevice, pContext }
{
}

CWeapon_Laurel::CWeapon_Laurel(const CWeapon_Laurel& rhs)
    : CPlayer_Weapon{ rhs }
{
}

HRESULT CWeapon_Laurel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CWeapon_Laurel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_eType = OBJ_PLAYER_WEAPON;

    return S_OK;
}

HRESULT CWeapon_Laurel::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    //CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    //CTransform* pTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    //_vector vPosition = pTransform->Get_State_Vector(CTransform::STATE_POSITION);
    //vPosition.m128_f32[1] += 3.f;
    //
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    return S_OK;
}

void CWeapon_Laurel::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    if(m_isUsing)
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
//    if (true == m_isUsing)
//    {
//        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
//
//#ifdef _DEBUG
//        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
//#endif
//    }
}

HRESULT CWeapon_Laurel::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    } 

    return S_OK;
}

HRESULT CWeapon_Laurel::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Item_Laurel";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(1.2f, 1.2f, 0.4f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon_Laurel::Bind_ShaderResources()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlDiffuse", &m_vMtrlDiffuse, sizeof(_vector))))
        return E_FAIL;

    return S_OK;
}

CWeapon_Laurel* CWeapon_Laurel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWeapon_Laurel* pInstance = new CWeapon_Laurel(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CWeaponCWeapon_Laurel_Shield"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon_Laurel::Clone(void* pArg)
{
    CWeapon_Laurel* pInstance = new CWeapon_Laurel(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CWeapon_Laurel"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon_Laurel::Free()
{
    __super::Free();
}
