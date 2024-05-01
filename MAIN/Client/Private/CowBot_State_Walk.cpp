#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Walk.h"

#include "Easing.h"

CCowBot_State_Walk::CCowBot_State_Walk(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CCowBot_State_Walk::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_WALK);

    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    // TODO : 나중에 설정된 지역 내 위치로 이동하도록 바꾸기
    m_vTargetPosition = _vector{    m_vEnterPosition.m128_f32[0] + (_float)(rand() % 3 + 1) - (_float)(rand() % 3 + 1), 
                                    m_vEnterPosition.m128_f32[1], 
                                    m_vEnterPosition.m128_f32[2] + (_float)(rand() % 3 + 1) - (_float)(rand() % 3 + 1), 1.f};

    if (m_vTargetPosition.m128_f32[0] > 24.f)
        m_vTargetPosition.m128_f32[0] = 24.f;
    if (m_vTargetPosition.m128_f32[0] < 12.f)
        m_vTargetPosition.m128_f32[0] = 12.f;
    if (m_vTargetPosition.m128_f32[2] > 85.f)
        m_vTargetPosition.m128_f32[2] = 85.f;
    if (m_vTargetPosition.m128_f32[2] < 75.f)
        m_vTargetPosition.m128_f32[2] = 75.f;

    /* 목표 방향 설정 */
    m_vTargetDir = m_vEnterPosition - m_vTargetPosition;
    m_vTargetDir.m128_f32[1] = 0.f;
    m_vTargetDir.m128_f32[3] = 0.f;
    m_vTargetDir = XMVector3Normalize(m_vTargetDir);
    _vector vLookDir = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK);
    vLookDir.m128_f32[1] = 0.f;
    vLookDir.m128_f32[3] = 0.f;
    vLookDir = XMVector3Normalize(vLookDir);

    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;
}

void CCowBot_State_Walk::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    // 플레이어 바라보기
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    /*if (false == m_isLook)
    {
        m_vTargetDir = XMVector3Normalize(vMonsterPosition - m_vTargetPosition);
        _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
        if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        {
            m_fAccLookTime += fTimeDelta;
            _float fRatio = m_fAccLookTime / m_fLookTime;
            if (fRatio >= 1)
            {
                fRatio = 1.f;
                m_isLook = true;
            }

            _vector vDir;
            vDir.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[0], m_vTargetDir.m128_f32[0], fRatio);
            vDir.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[1], m_vTargetDir.m128_f32[1], fRatio);
            vDir.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[2], m_vTargetDir.m128_f32[2], fRatio);
            vDir.m128_f32[3] = 0.f;

            m_pMonsterTransform->Look_At_Dir(vDir);
        }
    }*/

    _bool isArrive = false;
    isArrive = m_pMonsterTransform->Move_To_Target(m_vTargetPosition, fTimeDelta, 0.5f);
    m_pMonsterTransform->Look_At_Dir(m_vTargetDir, true);

    if (m_fAccChangeTime > m_fChangeTime)
    {
        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];
        if (FIND_DISTANCE > fDistance)
        {
            m_pMonster->Change_State(CMonster_CowBot::STATE_RUN);
        }
        else
        {
            if (true == isArrive)
            {
                m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
            }
        }
    }    
}

void CCowBot_State_Walk::OnStateExit()
{
    m_fRandomTime = 0.f;
    m_fAccChangeTime = 0.f;
}

CCowBot_State_Walk* CCowBot_State_Walk::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Walk* pInstance = new CCowBot_State_Walk(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Walk"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Walk::Free()
{
    __super::Free();
}
