#include "stdafx.h"
#include "Camera_Focus.h"

#include "Player.h"
#include "Easing.h"

CCamera_Focus::CCamera_Focus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
    , m_pEasing{ CEasing::Get_Instance() }
{
}

CCamera_Focus::CCamera_Focus(const CCamera_Focus& rhs)
    : CCamera{ rhs }
    , m_pEasing{ rhs.m_pEasing }
{
    Safe_AddRef(m_pEasing);
}

HRESULT CCamera_Focus::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Focus::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CAMERA_FOCUS_DESC* pDesc = (CAMERA_FOCUS_DESC*)pArg;

    m_pTargetTransform = pDesc->pTargetTransform;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_CamDistance = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));

    return S_OK;
}

HRESULT CCamera_Focus::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_M, KEY_PRESS))
    {
        m_CamDistance.m128_f32[1] += 1.f;
        m_CamDistance.m128_f32[2] -= 1.f;
    }

    if (true == m_pGameInstance->Get_DIKeyState(DIK_N, KEY_PRESS))
    {
        m_CamDistance.m128_f32[1] -= 1.f;
        m_CamDistance.m128_f32[2] += 1.f;
    }

    return S_OK;
}

void CCamera_Focus::Late_Tick(_float fTimeDelta)
{
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION) + m_CamDistance;
    
    if (true == m_pPlayer->isMove())
    {   
        if (false == m_isFirst)
            m_isFirst = true;

        m_fCurTime += fTimeDelta;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[3] = 1.f;       
    }
    else if (false == m_pPlayer->isMove())
    {
        if (true == m_isFirst)
        {
            m_isFirst = false;
            m_fCurTime = 0.f;
        }

        m_fCurTime += fTimeDelta * 0.01f;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[3] = 1.f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

    __super::Bind_PipeLines();
}

HRESULT CCamera_Focus::Render()
{
    return S_OK;
}

CCamera_Focus* CCamera_Focus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Focus* pInstance = new CCamera_Focus(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_Focus"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Focus::Clone(void* pArg)
{
    CCamera_Focus* pInstance = new CCamera_Focus(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CCamera_Focus"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Focus::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}