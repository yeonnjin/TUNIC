#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Energy_Beam.h"

#include "Librarian_Effect_Beam.h"

#define HANDBONE 94

CLibrarian_State_Pattern_Energy_Beam::CLibrarian_State_Pattern_Energy_Beam(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
	m_pMonster = pMonster;
	m_pPlayer = pPlayer;
	m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));

    CLibrarian_Effect_Beam* pBeam = dynamic_cast<CLibrarian_Effect_Beam*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Librarian_Attack_Beam")));
    m_pBeam = pBeam;
    Safe_AddRef(pBeam);
    m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Librarian_Effect"), pBeam);
}

void CLibrarian_State_Pattern_Energy_Beam::OnStateEnter()
{
    // 패턴
    // 0. 플레이어 향해서 칼 앞으로 뻗는 모션 (해당 방향으로 번개) ANIM_PALPATINE_START
    // 1. 칼 잡으면서 빔 발사 
    // 2. 옆으로 조금씩 돌면서 이동 : ANIM_PALPATINE_LOOP
    // 3. 다리 살짝 접으면서 종료 : ANIM_PALPATINE_EXIT

    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_PALPATINE_START);
}

void CLibrarian_State_Pattern_Energy_Beam::OnStateUpdate(_float fTimeDelta)
{
    // 0 -> 1, 2
    if (0 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_PALPATINE_START))
    {
        ++m_iPattern;
        // 2. 옆으로 조금씩 돌면서 이동 : ANIM_PALPATINE_LOOP
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_PALPATINE_LOOP);
    }

    // 1. 칼 잡으면서 빔 발사 : 플레이어 키 위치까지 내려오든가, 빔을 더 낮게 쏴야 함~
    if (1 == m_iPattern)
    {
        ++m_iPattern;

        _vector vPlayerPostion = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        //_vector vHandPosition = XMLoadFloat4(&m_pMonster->Get_Bone_Position(HANDBONE));
        _vector vHandPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vHandPosition.m128_f32[0] += 0.4f;
        vHandPosition.m128_f32[1] += 1.3f;
        vHandPosition.m128_f32[2] += 3.2f;
        _vector vDir = XMVector3Normalize(vHandPosition - vPlayerPostion);
        vDir.m128_f32[1] = 0.f;
        m_pBeam->Start_Pattern(vHandPosition, vDir);
    }

    // 2. 옆으로 조금씩 돌면서 이동 : ANIM_PALPATINE_LOOP
    if (2 == m_iPattern)
    {
        m_fAccLoopTime += fTimeDelta;

        // 2 -> 3
        if (m_fAccLoopTime >= m_fLoopTime)
        {
            ++m_iPattern;
            m_pBeam->Set_Pooling_Inactive();
            // 3. 다리 살짝 접으면서 종료 : ANIM_PALPATINE_EXIT
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_PALPATINE_EXIT);
        }
    }

    // 3 -> 상태 종료
    if (3 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_PALPATINE_EXIT))
    {
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
    }
}

void CLibrarian_State_Pattern_Energy_Beam::OnStateExit()
{
    m_iPattern = 0;
    m_fAccLoopTime = 0.f;
}

CLibrarian_State_Pattern_Energy_Beam* CLibrarian_State_Pattern_Energy_Beam::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Energy_Beam* pInstance = new CLibrarian_State_Pattern_Energy_Beam(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Energy_Beam"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Energy_Beam::Free()
{
    __super::Free();

    Safe_Release(m_pBeam);
}
