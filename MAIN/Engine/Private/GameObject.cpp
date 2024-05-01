#include "GameObject.h"
#include "GameInstance.h"


CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_pGameInstance(rhs.m_pGameInstance)
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_Component(const wstring & strComTag)
{
	auto	iter = m_Components.find(strComTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

/* 실제 게임 내에서 사용되는 객체가 호출하는 함수 */
HRESULT CGameObject::Initialize(void* pArg)
{
	if(nullptr != pArg)
	{
		GAMEOBJECT_DESC*		pGameObjectDesc = (GAMEOBJECT_DESC*)pArg;		
	}

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);
	
	return S_OK;
}

HRESULT CGameObject::Tick(_float fTimeDelta)
{
	if (true == m_isDead)
		return E_FAIL;

	return S_OK;
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Set_Dead(_bool isDead) 
{
	m_isDead = isDead;
	m_pTransformCom->Set_Dead(isDead);
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & strPrototypeTag, const wstring & strComponentTag, CComponent** ppOut, void* pArg)
{
	CComponent*		pComponent = m_pGameInstance->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	auto	iter = m_Components.find(strComponentTag);
	if (iter != m_Components.end())
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Compute_Damage_CoolTime(_float fTimeDelta)
{
	m_fAccDamageCoolTime += fTimeDelta;

	// 쿨타임이 지났고 면역 상태가 아닐 때, 공격을 받은 상태 : 데미지 받음
	if (m_fAccDamageCoolTime > m_fDamageCoolTime && true == m_isDamage && false == m_isImmune)
	{
		m_fAccDamageCoolTime = 0.f;
		--m_iHP;
		Damage_Event();
	}
	// 쿨타임이 지나지 않았거나, 면역 상태일 때 공격을 받은 상태 : 데미지 면역
	else if (((m_fAccDamageCoolTime <= m_fDamageCoolTime) || (true == m_isImmune)) && true == m_isDamage)
	{
		m_isDamage = false;
	}
}

_float CGameObject::Get_Speed()
{
	return m_pTransformCom->Get_SpeedPerSec();
}

void CGameObject::Rigid_Event(CGameObject* pGameObject)
{
	RIGID eObjectRigid = pGameObject->Get_RigidType();

	CTransform* pObjectTransform = dynamic_cast<CTransform*>(pGameObject->Get_Component(g_strTransformTag));

	_vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vObjectPosition = pObjectTransform->Get_State_Vector(CTransform::STATE_POSITION);

	// 나 -> 상대
	_vector vCollisionDir = vObjectPosition - vPosition;
	vCollisionDir.m128_f32[1] = 0.f;
	vCollisionDir.m128_f32[3] = 0.f;
	vCollisionDir = XMVector3Normalize(vCollisionDir);

	float deltaTime = 0.016f; // 예: 60fps의 경우

	// 밀리지 않는 상태일 때
	if (RIGID_BLOCK == m_eRigid)
	{
		// 상대가 밀리는 상태라면 : 밀기
		if (RIGID_PUSH == eObjectRigid)
		{
			vObjectPosition += vCollisionDir * m_pTransformCom->Get_SpeedPerSec() * deltaTime;
			pObjectTransform->Set_State(CTransform::STATE_POSITION, vObjectPosition);
		}
		// 밀리지 않는 상태라면 : 제자리
		else if (RIGID_BLOCK == eObjectRigid)
		{
			vPosition -= vCollisionDir * m_pTransformCom->Get_SpeedPerSec() * deltaTime;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	// 통과 가능한 상태일 때
	else if (RIGID_NONBLOCK == m_eRigid)
	{

	}
	// 밀릴 수 있는 상태일 때
	else if (RIGID_PUSH == m_eRigid)
	{
		// 상대가 밀리지 않는 상태라면 : 밀리기
		if (RIGID_BLOCK == eObjectRigid)
		{
			vPosition -= vCollisionDir * pObjectTransform->Get_SpeedPerSec() * deltaTime;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
		// 상대도 밀리는 상태라면 : 서로 밀기
		else if(RIGID_PUSH == m_eRigid)
		{
			vPosition -= vCollisionDir * m_pTransformCom->Get_SpeedPerSec() * deltaTime;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

			vObjectPosition += vCollisionDir * pObjectTransform->Get_SpeedPerSec() * deltaTime;
			pObjectTransform->Set_State(CTransform::STATE_POSITION, vObjectPosition);
		}
	}
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
