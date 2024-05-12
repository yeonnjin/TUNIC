#include "stdafx.h"

#include "Easing.h"
#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Lunge_Swipe.h"

CLibrarian_State_Pattern_Lunge_Swipe::CLibrarian_State_Pattern_Lunge_Swipe(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
    m_pEasing = CEasing::Get_Instance();
    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    //Safe_AddRef(m_pEasing);
}

void CLibrarian_State_Pattern_Lunge_Swipe::OnStateEnter()
{
    // 패턴
    // 0. ANIM_FLYING_SWOOP
    // 1. 플레이어에게 돌진
    // 2. 일정거리 앞에서 멈춤 -> 애니메이션이 멈춤
    // (+) 움직임 직접 제어

    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_FLYING_SWOOP);
    m_vOriginPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPostion = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vDir = XMVector3Normalize(m_vOriginPosition - vPlayerPostion);
    m_vRetreatPosition = m_vOriginPosition + vDir * 5.f;
    m_vDashPosition = m_vOriginPosition - vDir * 25.f;

    m_pMonsterTransform->Look_At_Dir(vDir);
    m_fOriginRotation = m_pMonsterTransform->Get_RotationPerSec();
    m_pMonsterTransform->Set_RotationPerSec(1.f);

    m_pGameInstance->Play_Once(TEXT("BOSS_Pattern_Lunge_Swipe.wav"), CSound_Manager::BOSS);
}

void CLibrarian_State_Pattern_Lunge_Swipe::OnStateUpdate(_float fTimeDelta)
{
    // 15 ~ 36 : 뒤로 밀리기
    // 37 ~ 52 : 전진, 멈추기
    // 40 ~ 65 : 반바퀴 회전

    _uint iFrameIndex = m_pMonster->Get_Current_Frame(CMonster_Librarian::ANIM_FLYING_SWOOP);
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = vMonsterPosition;

    if (15 <= iFrameIndex && 36 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 14) / 22.f;
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[0], m_vRetreatPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[1], m_vRetreatPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[2], m_vRetreatPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;
    }
    else if (37 <= iFrameIndex && 52 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 36) / 16.f;
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[0], m_vDashPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[1], m_vDashPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[2], m_vDashPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;
    }

    _float fTargetAngle = 0.f;
    if (40 <= iFrameIndex && 63 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 39) / 24.f;
        fTargetAngle = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, 0.f, m_fTargetAngle, fRatio);
        m_pMonsterTransform->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, 0.f);
    }

    _float fRatio = iFrameIndex / 64.f;

    _vector vFinPosition = XMVectorLerp(vMonsterPosition, vTargetPosition, fRatio);
    m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vFinPosition);
    _float fFinAngle = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, 0, fTargetAngle, fRatio);
    m_pMonsterTransform->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(fFinAngle));

    // 애니메이션 종료 시 플레이어가 가까이 있으면 ? 근접공격 : 기본상태 
    if (true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_FLYING_SWOOP))
    {
        _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];
        if (fDistance < 5.f)
        {
            m_pMonster->Change_State(CMonster_Librarian::STATE_MELEE);
        }
        else
        {
            m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
        }
    }
}

void CLibrarian_State_Pattern_Lunge_Swipe::OnStateExit()
{
    m_pMonsterTransform->Set_RotationPerSec(m_fOriginRotation);

    m_isSlash = { false };
}

CLibrarian_State_Pattern_Lunge_Swipe* CLibrarian_State_Pattern_Lunge_Swipe::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Lunge_Swipe* pInstance = new CLibrarian_State_Pattern_Lunge_Swipe(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Lunge_Swipe"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Lunge_Swipe::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}

