#include "stdafx.h"

#include "Player.h"
#include "Player_State_Dodge.h"

#include "Particle_Sphere.h"
#include "Sprite_Sweat.h"

CPlayer_State_Dodge::CPlayer_State_Dodge(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;

    m_RandomNumber = mt19937_64(m_RandomDevice());

    m_pSpriteSweat = dynamic_cast<CSprite_Sweat*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Sprite_Sweat")));
    Safe_AddRef(m_pSpriteSweat);
}

void CPlayer_State_Dodge::OnStateEnter()
{
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    //CPlayer::DIR eDir = m_pPlayer->Get_Dir();
    _vector vLook = m_pPlayer->Get_Look();

    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    if ((CPlayer::LOCK_ON_FIND == eLockOn || CPlayer::LOCK_ON_NONE == eLockOn))
    {
        m_vPreRight = pPlayerTransform->Get_State_Vector(CTransform::STATE_RIGHT);
        m_vPreUp = pPlayerTransform->Get_State_Vector(CTransform::STATE_UP);
        m_vPreLook = pPlayerTransform->Get_State_Vector(CTransform::STATE_LOOK);

        pPlayerTransform->Look_At_Dir(vLook);
    }

    _float fPlayerSP = m_pPlayer->Get_SP();

    m_eDodge = m_pPlayer->Get_Dodge();

    if(CPlayer::DODGE_ROLL == m_eDodge)
    {
        // 기력이 충분할 때 
        if (fPlayerSP >= m_fSP)
        {
            m_pPlayer->Set_SP_Minus(m_fSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE);
            m_eAnim = CPlayer::ANIM_DODGE;

            m_pGameInstance->Play_Once(TEXT("PLAYER_Dodge_Roll.wav"), CSound_Manager::PLAYER);
        }
        // 기력이 없을 때
        else
        {
            m_pPlayer->Set_SP_Minus(fPlayerSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE_GARBAGE);
            m_eAnim = CPlayer::ANIM_DODGE_GARBAGE;
            m_pSpriteSweat->Set_Using(true);

            m_pGameInstance->Play_Once(TEXT("PLAYER_Dodge_NoStamina.wav"), CSound_Manager::PLAYER);
        }
    }
    else if (CPlayer::DODGE_DASH == m_eDodge)
    {
        // 기력이 충분할 때 
        if (fPlayerSP >= m_fSP)
        {
            m_pPlayer->Set_SP_Minus(m_fDashSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_HYPERDASH);
            m_eAnim = CPlayer::ANIM_HYPERDASH;

            m_pGameInstance->Play_Once(TEXT("PLAYER_Dodge_HyperDash.wav"), CSound_Manager::PLAYER);
        }
        // 기력이 없을 때
        else
        {
            m_pPlayer->Set_SP_Minus(fPlayerSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE_GARBAGE);
            m_eAnim = CPlayer::ANIM_DODGE_GARBAGE;
            m_pSpriteSweat->Set_Using(true);

            m_pGameInstance->Play_Once(TEXT("PLAYER_Dodge_NoStamina.wav"), CSound_Manager::PLAYER);
        }
    }
}

void CPlayer_State_Dodge::OnStateUpdate(_float fTimeDelta)
{
    // 일반 닷지 상태일 때
    // 0 ~ 40 : 공격 면역
    if (CPlayer::ANIM_DODGE == m_eAnim)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(m_eAnim);
        if (0 <= iFrame && 40 >= iFrame)
        {
            m_pPlayer->Set_isImmune(true);
            Make_Cloud(fTimeDelta);
        }
        else
            m_pPlayer->Set_isImmune(false);      
    }

    // 하이퍼 대쉬 상태일 때
    // 0 ~ 20 : 공격 면역
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    if (CPlayer::ANIM_HYPERDASH == m_eAnim)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(m_eAnim);
        if (0 <= iFrame && 20 >= iFrame)
            m_pPlayer->Set_isImmune(true);
        else
            m_pPlayer->Set_isImmune(false);

        m_isEndDash = m_pPlayer->isEndDash();

        pPlayerTransform->Go_Straight(fTimeDelta * -5.f);
    }

    if (true == m_pPlayer->Get_isFinished(m_eAnim))
    {
        if (true == m_isEndDash)
        {
            //24.5
            _vector vPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
            if (vPosition.m128_f32[2] <= 24.5f)
            {
                m_isEndDash = false;
                //m_pPlayer->Set_EndDash(false);

                IF_PLAYER_ISMOVE
                    m_pPlayer->Change_State(CPlayer::STATE_MOVE);
                else
                    m_pPlayer->Change_State(CPlayer::STATE_IDLE);
            }
        }
        else
        {
            IF_PLAYER_ISMOVE
                m_pPlayer->Change_State(CPlayer::STATE_MOVE);
            else
                m_pPlayer->Change_State(CPlayer::STATE_IDLE);
        }     
    }
}

void CPlayer_State_Dodge::OnStateExit()
{
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    if ((CPlayer::LOCK_ON_FIND == eLockOn || CPlayer::LOCK_ON_NONE == eLockOn))
    {
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_RIGHT, m_vPreRight);
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_UP, m_vPreUp);
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_LOOK, m_vPreLook);
    }

    m_pPlayer->Set_isImmune(false);
}

void CPlayer_State_Dodge::Make_Cloud(_float fTimeDelta)
{
    m_fAccDelayTime += fTimeDelta;

    if (m_fAccDelayTime >= m_fDelayTime)
    {
        // Random
        uniform_real_distribution<_float>	PositionX(-0.4f, 0.4f);
        uniform_real_distribution<_float>	PositionZ(-0.4f, 0.4f);

        uniform_real_distribution<_float>	Scale(1.f, 3.f);

        uniform_real_distribution<_float>	LifeTime(0.6f, 1.f);

        // Clone
        CParticle_Sphere::PARTICLE_DESC tDesc{};
        CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
        _vector vPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vPosition.m128_f32[0] += PositionX(m_RandomNumber);
        vPosition.m128_f32[2] += PositionZ(m_RandomNumber);
        tDesc.vPosition = vPosition;
        _float fScale = Scale(m_RandomNumber);
        tDesc.vScale = _vector{ fScale , fScale , fScale };
        tDesc.fLiveTime = LifeTime(m_RandomNumber);

        m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Sphere"), TEXT("Prototype_GameObject_Particle_Sphere"), &tDesc);

        m_fAccDelayTime = 0.f;
    }
}

CPlayer_State_Dodge* CPlayer_State_Dodge::Create(CPlayer* pPlayer)
{
    CPlayer_State_Dodge* pInstance = new CPlayer_State_Dodge(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Dodge"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Dodge::Free()
{
    __super::Free();

    Safe_Release(m_pSpriteSweat);
}