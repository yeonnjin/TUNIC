#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Idle.h"

#include "Easing.h"

CLibrarian_State_Idle::CLibrarian_State_Idle(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();

    m_vOriginPosition = { 20.f, 1.f, 0.f, 1.f };
    m_fHeight = m_vOriginPosition.m128_f32[1];
}

void CLibrarian_State_Idle::OnStateEnter()
{
    // 패턴
    // 0. 일정 거리 떨어진 곳에서 돌면서 이동
    // 1. 패턴이 바뀔 때, 플레이어 위치 기준 회전 방향 결정 (보스의 룩보다 플레이어가 왼쪽에 있으면 반시계, 오른쪽에 있으면 시계 방향)
 
    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
       
    /* 회전 방향 설정 */
    _vector vMonsterLook = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK);
    _vector vTargetLook = m_vEnterPosition - m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    // 외적
    _vector vCross = XMVector3Normalize(XMVector3Cross(vMonsterLook, vTargetLook));

    // 내적
    _vector vNor = { 0.f, 1.f, 0.f };
    _vector vDot = XMVector3Dot(vCross, vNor);

    // TargetLook 이 오른쪽에 있을 때 : Clockwise
    if (vDot.m128_f32[0] >= 0.f && vDot.m128_f32[0] <= 1.f)
    {
        m_isClockwise = true;     
    }
    else
    {
        m_isClockwise = false;      
    }

    if (false == XMVector4Equal(m_vTargetPosition, m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION)))
        m_isTargetPosition = false;

    /* 목표 지점 설정 */
    Compute_TargetPosition();
    _float fDistance = XMVector3Length(vPlayerPosition - m_vEnterPosition).m128_f32[0];
    if (fDistance < 5.f)
    {
        m_isRetreat = true;
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_RETREAT);
    }
    else if (true == m_isClockwise)
    {
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_IDLE_LEFT);
    }
    else if (false == m_isClockwise)
    {
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_IDLE_RIGHT);
    }
}

void CLibrarian_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    // 애니메이션 제어
    if (true == m_isRetreat && m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_RETREAT))
    {
        if (true == m_isClockwise)
        {
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_IDLE_LEFT);
        }
        else if (false == m_isClockwise)
        {
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_IDLE_RIGHT);
        }
    }

    // 플레이어 바라보기
    m_pMonsterTransform->Look_At_For_LandOject(dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION), true);

    // 타겟 위치로 이동 
    if(false == m_isTargetPosition)
    {
        m_fAccTargetTime += fTimeDelta;
        _float fRatio = m_fAccTargetTime / m_fTargetTime;
        if (fRatio >= 1)
        {
            fRatio = 1.f;
            m_isTargetPosition = true;
        }

        _vector vTargetPosition{};
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vEnterPosition.m128_f32[0], m_vTargetPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vEnterPosition.m128_f32[1], m_vTargetPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, m_vEnterPosition.m128_f32[2], m_vTargetPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;

        m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vTargetPosition);
    }
    else
    {
        m_fAccIdleTime += fTimeDelta;

        // 주위 회전
        if (true == m_isClockwise)
        {
            m_fRadian += fTimeDelta * 0.3f;
            if (m_fRadian >= XMConvertToRadians(360.f))
                m_fRadian = 0.f;
        }
        else
        {
            m_fRadian -= fTimeDelta * 0.3f;
            if (m_fRadian <= XMConvertToRadians(-360.f))
                m_fRadian = XMConvertToRadians(360.f);
        }

        Compute_Height(fTimeDelta);

        _vector vRotationPosition = XMVector3TransformCoord(m_vOriginPosition, XMMatrixRotationY(m_fRadian));
        vRotationPosition.m128_f32[1] = m_fTargetHeight;
        m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vRotationPosition);

        if (m_fAccIdleTime > m_fIdleTime)
        {
            m_fAccIdleTime = 0.f;
            m_pMonster->Change_State((CMonster_Librarian::STATE)m_pMonster->Get_Pattern());
        }
    }
}

void CLibrarian_State_Idle::OnStateExit()
{
    m_isTargetPosition = true;
    m_isUp = false;
    m_isRetreat = false;

    m_fAccTargetTime = 0.f;
    m_fAccHeightTime = 0.f;
    m_fAccIdleTime = 0.f;
}

void CLibrarian_State_Idle::Compute_TargetPosition()
{
    /* 상태 진입 시 원래 위치 벡터와 이루는 각도 구하기 */

    // 내적
    _vector vOriginPosition = _vector{ 0.f, 1.f, 0.f, 1.f };
    _vector vMonster = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION) - vOriginPosition);
    _vector vOrigin = XMVector3Normalize(m_vOriginPosition - vOriginPosition);
    _float fDot = XMVector3Dot(vMonster, vOrigin).m128_f32[0];


    /* 각도 방향 판단 */

    // 외적
    _vector vCross = XMVector3Normalize(XMVector3Cross(vMonster, vOrigin));

    // 내적
    _vector vNor = { 0.f, 1.f, 0.f };
    _vector vDot = XMVector3Dot(vCross, vNor);

    // 오른쪽에 있을 때 : Clockwise
    if (vDot.m128_f32[0] >= 0.f && vDot.m128_f32[0] <= 1.f)
        m_fRadian = acosf(fDot) * -1;
    else
        m_fRadian = acosf(fDot);

    // 목표 위치 구하기
    m_vTargetPosition = XMVector3TransformCoord(m_vOriginPosition, XMMatrixRotationY(m_fRadian));
}

void CLibrarian_State_Idle::Compute_Height(_float fTimeDelta)
{
    _float fTargetHeight;
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);  

    if (true == m_isUp)
    {          
        m_fAccHeightTime += fTimeDelta;
        _float fRatio = m_fAccHeightTime / m_fHeightTime;
        if (fRatio >= 1)
        {
            fRatio = 1.f;
            m_isUp = false;
            m_fAccHeightTime = 0.f;
        }

        m_fTargetHeight = m_pEasing->Get_Ease(CEasing::Ease_InSine, m_fHeight - 3, m_fHeight, fRatio);
    }
    else
    {
        m_fAccHeightTime += fTimeDelta;
        _float fRatio = m_fAccHeightTime / m_fHeightTime;
        if (fRatio >= 1)
        {
            fRatio = 1.f;
            m_isUp = true;
            m_fAccHeightTime = 0.f;
        }

        m_fTargetHeight = m_pEasing->Get_Ease(CEasing::Ease_InSine, m_fHeight, m_fHeight - 3, fRatio);
    }
}

CLibrarian_State_Idle* CLibrarian_State_Idle::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Idle* pInstance = new CLibrarian_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Idle::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}
