#pragma once

namespace Engine
{
	/* Engine */
	typedef struct
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	/* Mesh */
	typedef struct
	{
		class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	}MESH_MATERIAL;

	/* Channel */
	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vTranslation;
		float			fTime;
	}KEYFRAME;

	/* Vertext */
	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXPOSNORTEX;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];
	}VTXMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점에게 영향을 주는 뼈들의 인덱스와 가중치 */
		XMUINT4			vBlendIndices;	// 이 메시에게 영향을 주는 뼈들의 인덱스
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXANIMMESH;

	typedef struct ENGINE_DLL
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXMAP;

	/* Model_File */
	typedef struct ENGINE_DLL
	{
		_char						szName[MAX_PATH];

		_int						iBoneIndex = { -1 };

		_uint						iNumKeyFrames = { 0 };
		vector<KEYFRAME>			KeyFrames;
	}CHANNELFILE;

	typedef struct ENGINE_DLL
	{
		_char						szName[MAX_PATH];

		_float						fDuration = { 0.f };
		_float						fTicksPerSecond = { 0.f };
		_float						fTrackPosition = { 0.f };

		_uint						iNumChannels = { 0 };
		vector<CHANNELFILE>			Channels;
	}ANIMFILE;

	typedef struct ENGINE_DLL
	{
		_char						szName[MAX_PATH];

		_float4x4					TransformationMatrix;

		_int						iParentBoneIndex;
	}BONEFILE;

	typedef struct ENGINE_DLL
	{
		_char						szTexturePath[MAX_PATH];

		_uint						iTextureIndex;
	}MATERIALFILE;

	typedef struct ENGINE_DLL
	{
		_char						szName[MAX_PATH];

		_uint						iMaterialIndex = { 0 };

		_uint						iNumFaces = { 0 };

		_uint						iNumBones = { 0 };
		vector<_uint>				Bones;

		_uint						iNumVertices;
		VTXMESH*					pMeshVertices = { nullptr };
		VTXANIMMESH*				pAnimMeshVertices = { nullptr };

		_uint						iNumIndices;
		_uint*						pIndices = { nullptr };

		_uint						iNumOffsetMatrices = { 0 };
		vector<_float4x4>			OffsetMatrices;
	}MESHFILE;

	typedef struct ENGINE_DLL
	{
		// ModelTag
		//_char						szModelComTag[MAX_PATH];
		wstring						strModelComTag;

		// Type
		_uint						iType;

		// Mesh	
		_uint						iNumMeshes = { 0 };
		vector<MESHFILE>			Meshes;

		// Material
		_uint						iNumMaterials = { 0 };
		vector<MATERIALFILE>		Materials;
		//MATERIALFILE				Materials[AI_TEXTURE_TYPE_MAX];

		// Bone
		_float4x4					TransformMatrix;

		_uint						iNumBones;
		vector<BONEFILE>			Bones;

		// Animation
		_uint						iNumAnimations = { 0 };
		_uint						iCurrentAnimIndex = { 0 };
		_bool						isLoop = { false };
		vector<ANIMFILE>			Animations;

		_float4x4					MeshBoneMatrices[512] = {};
	}MODELFILE;

	/* Map Parsing */
	typedef struct ENGINE_DLL
	{
		// 필요한 거
		// 1. 오브젝트 전체 개수? -> 상위 단
		// ========================================
		// 2. 오브젝트 위치
		// 3. 오브젝트 이름 -> 복사할 프로토타입 태그
		// 4. 얘가 가지고 있는 모델이랑 컴포넌트 이름?

		_float4x4				TransformMatrix;
		_char					szModelComTag[MAX_PATH];
	}MAPOBJFILE;
}