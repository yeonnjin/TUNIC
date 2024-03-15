#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_SPHERE : public CBounding
{
public:
	typedef struct Bounding_SPHERE_Desc : public CBounding::BOUNDING_DESC
	{
		_float3	vSize;
	}BOUNDING_SPHERE_DESC;

private:
	CBounding_SPHERE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_SPHERE() = default;

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif // _DEBUG

private:
	BoundingBox*	m_pBoundingDesc = { nullptr };

public:
	static CBounding_SPHERE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END