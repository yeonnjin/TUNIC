#pragma once
#include "Base.h"
#include "fmod.h"
#include <io.h>
#include "Engine_Defines.h"
#pragma comment(lib,"fmodex_vc.lib")
#pragma comment(lib,"fmod64_vc.lib")

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class ENGINE_DLL CFMOD_Manager final : public CBase
{
public:
	CFMOD_Manager();
	virtual ~CFMOD_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySound(string pSoundKey, const _uint& eID, const float& fVolume);
	void PlayBGM(string pSoundKey, const float& fVolume);

	void StopSound(const _uint& eID);
	void StopAll();

	void SetChannelVolume(const _uint& eID, const float& fVolume);

	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);

	int  Pause(const _uint& eID);

private:
	void LoadSoundFile();

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	std::map<string, FMOD::Sound*> m_mapSound;

	enum { MAXCHANNEL = 32 };
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];
	FMOD::System * m_pSystem;
	_bool		m_bPause = false;

public:
	virtual void Free() override;
};

END

