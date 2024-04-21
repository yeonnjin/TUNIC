#include "stdafx.h"

#include "Player.h"
#include "NPC_Merchant.h"
#include "Merchant_State_Idle.h"

#include "Easing.h"

CMerchant_State_Idle::CMerchant_State_Idle(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    m_pNPC = pNPC;
    m_pPlayer = pPlayer;

    m_pNPCTransform = dynamic_cast<CTransform*>(m_pNPC->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CMerchant_State_Idle::OnStateEnter()
{
    m_pNPC->Set_Blending(true, CNPC_Merchant::ANIM_IDLE);

    m_vEnterPosition = m_pNPCTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    /* 목표 방향 설정 */
    m_vTargetDir = XMVector3Normalize(m_vEnterPosition - vPlayerPosition);
    _vector vLookDir = XMVector3Normalize(m_pNPCTransform->Get_State_Vector(CTransform::STATE_LOOK));
    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;

    /* 이동할 위치 */
    m_vTargetPosition = m_vEnterPosition;
    m_vTargetPosition.m128_f32[0] = vPlayerPosition.m128_f32[0];
}

void CMerchant_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    //// 플레이어 바라보기
    _vector vNPCPosition = m_pNPCTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    //if (false == m_isLook)
    //{
    //    m_vTargetDir = XMVector3Normalize(vNPCPosition - vPlayerPosition);
    //    _vector vLookDir = XMVector3Normalize(m_pNPCTransform->Get_State_Vector(CTransform::STATE_LOOK));
    //    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
    //    {
    //        m_fAccLookTime += fTimeDelta;
    //        _float fRatio = m_fAccLookTime / m_fLookTime;
    //        if (fRatio >= 1)
    //        {
    //            fRatio = 1.f;
    //            m_isLook = true;
    //        }

    //        _vector vDir;
    //        vDir.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[0], m_vTargetDir.m128_f32[0], fRatio);
    //        vDir.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[1], m_vTargetDir.m128_f32[1], fRatio);
    //        vDir.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[2], m_vTargetDir.m128_f32[2], fRatio);
    //        vDir.m128_f32[3] = 0.f;

    //        m_pNPCTransform->Look_At_Dir(vDir);
    //    }
    //}
    //else
    //{
    //    m_pNPCTransform->Look_At_For_LandOject(dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION), true);
    //}

    // 이동할 타겟 위치 (플레이어 따라다님)

    if (true == m_pPlayer->isMove() /*&& true == m_isTargetPosition*/)
    {
        m_vEnterPosition = vNPCPosition;
        m_vTargetPosition = vNPCPosition;
        m_vTargetPosition.m128_f32[0] = vPlayerPosition.m128_f32[0];
        m_fAccTargetTime = 0.f;
        m_isTargetPosition = false;
    }

    if (false == m_isTargetPosition)
    {
        m_fAccTargetTime += fTimeDelta;
        _float fRatio = m_fAccTargetTime / m_fTargetTime;
        if (m_fAccTargetTime >= m_fTargetTime - 1.f)
        {
           // fRatio = 1.f;
            m_fAccTargetTime = 0.f;
            m_isTargetPosition = true;
        }

        _vector vTargetPosition{};
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InOutQuad, m_vEnterPosition.m128_f32[0], m_vTargetPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InOutQuad, m_vEnterPosition.m128_f32[1], m_vTargetPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InOutQuad, m_vEnterPosition.m128_f32[2], m_vTargetPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;

        m_pNPCTransform->Set_State(CTransform::STATE_POSITION, vTargetPosition);
    }

    if (125 == m_pNPC->Get_Current_Frame(CNPC_Merchant::ANIM_IDLE))
        m_pNPC->Set_Blending(true, CNPC_Merchant::ANIM_IDLE);
}

void CMerchant_State_Idle::OnStateExit()
{
    m_isTargetPosition = true;
    m_isLook = true;

    m_fAccTargetTime = 0.f;
    m_fAccLookTime = 0.f;
}

CMerchant_State_Idle* CMerchant_State_Idle::Create(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    CMerchant_State_Idle* pInstance = new CMerchant_State_Idle(pNPC, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CMerchant_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMerchant_State_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}