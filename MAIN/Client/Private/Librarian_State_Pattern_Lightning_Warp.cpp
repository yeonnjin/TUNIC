#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Lightning_Warp.h"

#define MAP_X_MIN -4.f
#define MAP_X_SIZE 8
#define MAP_Y 0.5f
#define MAP_Z_MIN -4.f
#define MAP_Z_SIZE 8

CLibrarian_State_Pattern_Lightning_Warp::CLibrarian_State_Pattern_Lightning_Warp(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
}

void CLibrarian_State_Pattern_Lightning_Warp::OnStateEnter()
{
    // 패턴
    // 0. 칼 들고 하늘로 올라가기
    // 1. 낙하 지점에 번개
    // 2. 칼 내려 꽂기
    // 3. 꽂은 채로 일정 시간 반복 (주변 번개 공격, 뒤에서는 공격 가능)
    // 4. 칼 뽑기

    // 0. 칼 들고 하늘로 올라가기
    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_LIGHTNING_WINDUP);
}

void CLibrarian_State_Pattern_Lightning_Warp::OnStateUpdate(_float fTimeDelta)
{
    // 0 -> 1
    if (0 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_LIGHTNING_WINDUP))
    {
        ++m_iPattern;   

        // 낙하 지점 설정
        m_vDropPosition = Get_Drop_Position();
        dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_POSITION, m_vDropPosition);
    }

    // 1. 낙하 지점에 번개
    if (1 == m_iPattern)
    {
        m_fAccLightningTime += fTimeDelta;

        // 번개 이펙트~

        // 1 -> 2
        if (m_fAccLightningTime > m_fLightningTime)
        {
            ++m_iPattern;
            m_fAccLightningTime = 0.f;
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_LIGHTNING_LAND);
        }
    }

    // 애니메이션 멈춤 상태 보정
    if (2 == m_iPattern && 0 == m_pMonster->Get_Current_Frame(CMonster_Librarian::ANIM_LIGHTNING_LAND))
    {
        _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vMonsterPosition.m128_f32[1] = 50.f;
        m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vMonsterPosition);
    }

    if (2 == m_iPattern && 1 == m_pMonster->Get_Current_Frame(CMonster_Librarian::ANIM_LIGHTNING_LAND))
    {
        _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vMonsterPosition.m128_f32[1] = 0.2f;
        m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vMonsterPosition);
    }

    // 2. 칼 내려 꽂기
    if (2 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_LIGHTNING_LAND))
    {
        ++m_iPattern;
        // 2 -> 3
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_LIGHTNING_LOOP);
    }

    // 3. 꽂은 채로 일정 시간 반복 (주변 번개 공격, 뒤에서는 공격 가능)
    if (3 == m_iPattern)
    {
        m_fAccLoopTime += fTimeDelta;

        // 3 -> 4
        if (m_fAccLoopTime > m_fLoopTime)
        {
            ++m_iPattern;
            m_fAccLoopTime = 0.f;
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_LIGHTNING_RELEASE);
        }
    }

    // 4. 칼 뽑기
    if (4 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_LIGHTNING_RELEASE))
    {
        m_iPattern = 0;

        // 애니메이션 종료 시 플레이어가 가까이 있으면 ? 근접공격 : 기본상태 
        _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
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

void CLibrarian_State_Pattern_Lightning_Warp::OnStateExit()
{
    m_iPattern = 0;
    m_fAccLightningTime = 0.f;
    m_fAccLoopTime = 0.f;
}

_vector CLibrarian_State_Pattern_Lightning_Warp::Get_Drop_Position()
{
    // Librarian Map 사이즈..
    _float fRandomX = MAP_X_MIN + rand() % MAP_X_SIZE;
    _float fRandomZ = MAP_Z_MIN + rand() % MAP_Z_SIZE;

    return _vector{ fRandomX, MAP_Y, fRandomZ, 1.f };
}

CLibrarian_State_Pattern_Lightning_Warp* CLibrarian_State_Pattern_Lightning_Warp::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Lightning_Warp* pInstance = new CLibrarian_State_Pattern_Lightning_Warp(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Lightning_Warp"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Lightning_Warp::Free()
{
    __super::Free();
}

