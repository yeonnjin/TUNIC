#include "stdafx.h"
#include "Monster_Spinner.h"

#include "Spinner_State_Idle.h"
#include "Spinner_State_Hidden.h"
#include "Spinner_State_Explode.h"
#include "Spinner_State_Attack.h"
#include "Spinner_State_Damage.h"

#include "Player.h"
#include "Player_Weapon.h"

CMonster_Spinner::CMonster_Spinner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMonster_Spinner::CMonster_Spinner(const CMonster_Spinner& rhs)
    : CMonster{ rhs }
{
}

_bool CMonster_Spinner::Get_isCollision()
{
    return m_pColliderCom->Check_Collision((CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider")));
}

void CMonster_Spinner::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CMonster_Spinner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Spinner::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    //_float4 vPosition = _float4(-35.f, 2.5f, 86.f, 1.f);
    _float4 vPosition = _float4(33.f, 4.f, -86.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pModelCom->Set_Animation_Index(ANIM_HIDDEN);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    m_iHP = 3;

    return S_OK;
}

HRESULT CMonster_Spinner::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CMonster_Spinner::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMonster_Spinner::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Spinner::Add_Components()
{
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 1.8f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NavigationDesc{};
    NavigationDesc.iCurrentIndex = 112;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Spinner::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pModelCom->Add_State(STATE_IDLE, CSpinner_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_HIDDEN, CSpinner_State_Hidden::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_EXPLODE, CSpinner_State_Explode::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_ATTACK, CSpinner_State_Attack::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DAMAGE, CSpinner_State_Damage::Create(this, pPlayer));
    m_pModelCom->Change_State(STATE_HIDDEN);
    m_eState = STATE_HIDDEN;

    return S_OK;
}

void CMonster_Spinner::Update_State()
{
    switch (m_eState)
    {
    case STATE_IDLE:
        break;
    case STATE_HIDDEN:
        break;
    case STATE_EXPLODE:
        break;
    case STATE_ATTACK:
        break;
    case STATE_DAMAGE:
        break;
    case STATE_END:
        break;
    default:
        break;
    }
}

HRESULT CMonster_Spinner::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

void CMonster_Spinner::Set_Animation()
{
    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_HIDDEN, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FORWARD, true);

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_FORWARD, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_RECOIL, true);

    // BLEND TIME
    m_pModelCom->Set_Blend_Time(ANIM_START_SPIN, 0.1f);
    m_pModelCom->Set_Blend_Time(ANIM_FORWARD, 0.1f);
}

CMonster_Spinner* CMonster_Spinner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Spinner* pInstance = new CMonster_Spinner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CMonster_Spinner"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Spinner::Clone(void* pArg)
{
	CMonster_Spinner* pInstance = new CMonster_Spinner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CMonster_Spinner"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Spinner::Free()
{
	__super::Free();
}

void CMonster_Spinner::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);

    if(OBJ_PLAYER == pGameObject->Get_ObjectType() && m_eState != STATE_EXPLODE && m_eState != STATE_DAMAGE)
    {
        // 패링 상태일 떄 : STUN(DAMAGE)
        if (true == dynamic_cast<CPlayer*>(pGameObject)->isParrying())
        {
            Change_State(STATE_DAMAGE);
        }
        // 패링 상태가 아닐 때 : 데미지 주기
        else
        {
            dynamic_cast<CPlayer*>(pGameObject)->Set_isDamage(true);
        }
    }
}

void CMonster_Spinner::Damage_Event()
{
    if (0 >= m_iHP)
        Change_State(STATE_EXPLODE);
}
