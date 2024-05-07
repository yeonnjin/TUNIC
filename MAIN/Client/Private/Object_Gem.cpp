#include "stdafx.h"
#include "Object_Gem.h"

#include "Player.h"

#define GRAVITY 9.8f

CObject_Gem::CObject_Gem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Gem::CObject_Gem(const CObject_Gem& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Gem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Gem::Initialize(void* pArg)
{
    m_eInteractiveType = INTERACTIVE_GEM;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    OBJECT_GEM_DESC* pDesc = (OBJECT_GEM_DESC*)pArg;

    m_vPrePosition = pDesc->vPosition;
    m_vPrePosition.m128_f32[1] += 1.f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    m_iGemCount = pDesc->iGemCount;
    m_iIndex = pDesc->iIndex;
    m_iNavigationIndex = pDesc->iNavigationIndex;

    Compute_ColliderMatrix();
    Compute_Dir();

    m_eRigid = RIGID_NONBLOCK;

    return S_OK;
}

HRESULT CObject_Gem::Tick(_float fTimeDelta)
{
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;
 
    //// Navigation
    //if (false == m_pNavigationCom->isMove(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION)))
    //{
    //    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    //}
    //m_vPrePosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

    // Height
    if(false == m_isLand && false == m_isCollision)
        Move_Pop(fTimeDelta);

    if (true == m_isCollision)
    {
        Move_Collision(fTimeDelta);
    }
    //_float fHeight = m_pNavigationCom->Compute_Height(m_vPrePosition, 0.5f);
    //if (false == isnan(fHeight))
    //{
    //    // 땅에 도착했을 때
    //    if (m_vPrePosition.m128_f32[1] <= fHeight)
    //    {
    //        //m_isLand = true;
    //        m_vPrePosition.m128_f32[1] = fHeight;
    //        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    //    }          
    //    // 땅에 떨어지지 않았을 때
    //    else
    //    {
    //        Move_Pop(fTimeDelta);
    //    }
    //}
    
    // Collider
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    // Render
    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    return S_OK;
}

void CObject_Gem::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CObject_Gem::Render()
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

HRESULT CObject_Gem::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Gem";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NavigationDesc{};
    NavigationDesc.iCurrentIndex = m_iNavigationIndex;
    if (FAILED(__super::Add_Component(LEVEL_BEACH, TEXT("Prototype_Component_Navigation"),
    	TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
    	return E_FAIL;

    return S_OK;
}

HRESULT CObject_Gem::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlDiffuse", &m_vMtrlDiffuse, sizeof(_vector))))
        return E_FAIL;

    return S_OK;
}

void CObject_Gem::Compute_ColliderMatrix()
{
    /*_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;*/
}

void CObject_Gem::Compute_Dir()
{
    m_pTransformCom->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians((_float)m_iIndex / (_float)m_iGemCount * 360.f));
    m_vOriginPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    m_fOriginY = m_vOriginPosition.m128_f32[1];
}

void CObject_Gem::Move_Pop(_float fTimeDelta)
{
    m_fAccPopTime += fTimeDelta;

    _float fX = Get_Speed() * cosf(XMConvertToRadians(70.f)) * m_fAccPopTime;
    _float fY = Get_Speed() * sinf(XMConvertToRadians(70.f)) * m_fAccPopTime - 0.5f * GRAVITY * pow(m_fAccPopTime, 2);

    _vector vPosition{};
    _vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);

    vPosition = m_vOriginPosition + XMVector3Normalize(vLook) * fX;
    vPosition.m128_f32[1] = m_fOriginY + fY;

    if (false == m_pNavigationCom->isMove(vPosition))
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    }
    m_vPrePosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    
    _float fOffset{};
    switch (m_iGemCount)
    {
    case 3:
        fOffset = 0.5f;
        break;
    case 5:
        fOffset = 0.8f;
        break;
    case 7:
        fOffset = 3.8f;
        break;
    case 9:
        fOffset = 6.5f;
        break;
    default:
        break;
    }

    _float fHeight = m_pNavigationCom->Compute_Height(vPosition, fOffset);
    if (vPosition.m128_f32[1] <= fHeight)
    {
        vPosition.m128_f32[1] = fHeight;
        m_isLand = true;
    }
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    m_vPrePosition = vPosition;
}

void CObject_Gem::Move_Collision(_float fTimeDelta)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    if (true == m_pTransformCom->Move_To_Target(vPlayerPosition, fTimeDelta, 0.5f))
    {
        m_isDead = true;
        pPlayer->Set_Gem();
    }
}

CObject_Gem* CObject_Gem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Gem* pInstance = new CObject_Gem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Gem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Gem::Clone(void* pArg)
{
    CObject_Gem* pInstance = new CObject_Gem(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Gem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Gem::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pNavigationCom);
}

void CObject_Gem::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (OBJ_PLAYER == pGameObject->Get_ObjectType() && true == m_isLand)
    {
        m_isCollision = true;
    }
}
