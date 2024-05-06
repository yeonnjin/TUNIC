#include "stdafx.h"
#include "Camera_Follow.h"

#include "Player.h"
#include "Easing.h"

CCamera_Follow::CCamera_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Follow::CCamera_Follow(const CCamera_Follow& rhs)
    : CCamera{ rhs }
    , m_pEasing{ CEasing::Get_Instance()}
{
#ifndef _DEBUG
    Safe_AddRef(m_pEasing);
#endif // _DEBUG   
}

HRESULT CCamera_Follow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Follow::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    /*CAMERA_FOLLOW_DESC* pDesc = (CAMERA_FOLLOW_DESC*)pArg;

    m_pTargetTransform = pDesc->pTargetTransform;*/

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    //m_pTargetTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag, 0));


    m_vCamDistance = CAM_DISTANCE;
    // m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - /*m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);*/ _vector{ 0.f, 0.2f, 0.f, 1.f };

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pTargetTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = /*m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);*/ _vector{ 0.f, 0.2f, 0.f, 1.f };

    _vector vTargetLookAt = { vCamPosition.m128_f32[0], vTargetPosition.m128_f32[1], vTargetPosition.m128_f32[2], 1.f };
    m_pTransformCom->Look_At(vTargetLookAt);

    return S_OK;
}

HRESULT CCamera_Follow::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vObjectPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = vObjectPosition + m_vCamDistance;

    if (true == m_pPlayer->isMove())
    {
        m_fMoveTime += fTimeDelta;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[3] = 1.f;

        m_fIdleTime = 0.f;
    }
    else
    {
        m_fIdleTime += fTimeDelta * 0.01f;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[3] = 1.f;

        m_fMoveTime = 0.f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

    __super::Bind_PipeLines();

    return S_OK;
}

void CCamera_Follow::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Follow::Render()
{
    return S_OK;
}

void CCamera_Follow::Set_Level(_uint iLevel)
{
}

void CCamera_Follow::OnEnter(void* pArg)
{
    m_fMoveTime = 0.f;
    m_fIdleTime = 0.f;
    m_fLookTime = 0.f;
    
    if (nullptr != pArg)
    {
        CAMERA_FOLLOW_DESC* pDesc = (CAMERA_FOLLOW_DESC*)pArg;
        m_vLookAt = pDesc->vLookAt;
        m_isLockOn = pDesc->isLockOn;
    }
    else
    {
        _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
        m_vLookAt = { vCamPosition.m128_f32[0], vTargetPosition.m128_f32[1], vTargetPosition.m128_f32[2], 1.f };
    }

    m_fFovy = XMConvertToRadians(60.f);
}

void CCamera_Follow::OnExit()
{
    m_fMoveTime = 0.f;
    m_fIdleTime = 0.f;
    m_fLookTime = 0.f;
}

CCamera_Follow* CCamera_Follow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Follow* pInstance = new CCamera_Follow(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_Follow"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Follow::Clone(void* pArg)
{
    CCamera_Follow* pInstance = new CCamera_Follow(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CCamera_Follow"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Follow::Free()
{
    __super::Free();

    if(true == m_isClone)
        Safe_Release(m_pEasing);
}