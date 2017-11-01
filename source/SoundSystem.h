#pragma once
#pragma comment(lib, "fmodex_vc.lib")
#include<iostream>
#include<fmod.h>
#include<Windows.h>
#include"globalDeviceContext.h"

#define EFFECT_SOUND_COUNT 6
//0~3 UI ȿ����
#define SOUND_MAIN_SCROLL 0
#define SOUND_SELECT 1
#define SOUND_MUSIC_SCROLL 2
#define SOUND_OPTION_SCROLL 3
//4 ���� ������ �̵� ȿ����
#define SOUND_PREV_SCENE 4
//��ź ������ �Ҹ�
#define SOUND_BOMB 5


//5~ �ΰ��� ȿ����

typedef int SoundIndex;

class SoundSystem
{
private:
	//ä�� 0 : ����
	//ä�� 1 : ȭ��� Ű ���� ȿ����
	//ä�� 2 : scene ��ȯ ȿ����
	//ä�� 3 : �ΰ��� ȿ���� 1
	//ä�� 4 : �ΰ��� ȿ���� 2
	//ä�� 5 : �ΰ��� ȿ���� 3
	//ä�� 6 : �ΰ��� ȿ���� 4
	FMOD_SYSTEM* pFMOD_System;

	//�̸� �ε��� ��, �뷮 �� �ȵǴ� ȿ������.
	FMOD_SOUND* pGlobalSounds[EFFECT_SOUND_COUNT];

	FMOD_SOUND* pMusic;
	FMOD_CHANNEL* MusicChannel;

	bool MusicIsPlay;
public:
	SoundSystem()
	{
		FMOD_System_Create(&pFMOD_System);
		FMOD_System_Init(pFMOD_System, 32, FMOD_INIT_NORMAL, NULL);

		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/TitleScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_MAIN_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/MusicScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_MUSIC_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/OptionScroll.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_OPTION_SCROLL]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/SceneBack.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_PREV_SCENE]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/Select.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_SELECT]);
		FMOD_System_CreateSound(pFMOD_System, "Sounds/Effect/bomb.ogg", FMOD_DEFAULT, nullptr, &pGlobalSounds[SOUND_BOMB]);
		pMusic = nullptr;
		MusicIsPlay = false;
	}
	
	// SOUND_MAIN_SCROLL : ����ȭ�� �޴� ��ũ�� ȿ����
	// SOUND_SELECT : ����ȭ�� �޴� �� ���� ��ũ�� ���� ȿ����
	// SOUND_SCROLL : ���� ��ũ�� ȿ����
	// SOUND_PREV_SCENE : ���� ������ ���ư��� ȿ����
	// SOUND_MUSIC_SCROLL : �� ���� â������ ��ũ�� ȿ����
	// SOUND_BOMB : ��ź ������ �Ҹ�
	void PlaySoundEffect(SoundIndex Sound)
	{
		FMOD_CHANNEL* Channel;
		if(0<= Sound && Sound<=3)
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		else if(Sound==SOUND_PREV_SCENE)
		{
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		}
		else if (5 <= Sound)
		{
			FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pGlobalSounds[Sound], false, &Channel);
		}
	}

	void PlayMusic(const char* musicPath, bool loop)
	{
		if(loop)
			FMOD_System_CreateStream(pFMOD_System, musicPath, FMOD_LOOP_NORMAL, NULL, &pMusic);
		else
			FMOD_System_CreateStream(pFMOD_System, musicPath, FMOD_DEFAULT, NULL, &pMusic);

		FMOD_System_PlaySound(pFMOD_System, FMOD_CHANNEL_FREE, pMusic, false, &MusicChannel);
		MusicIsPlay = true;
	}

	void StopMusic()
	{
		if (MusicIsPlay)
		{
			MusicIsPlay = false;
			FMOD_Sound_Release(pMusic);
		}
	}

	void Update()
	{
		FMOD_System_Update(pFMOD_System);
	}

	//���� ������� ���� �� ���̸� �и������� ������ �����Ѵ�.
	unsigned int getMusicTotalLength() {
		FMOD_SOUND* pCurrentMusic;
		unsigned int TotalTimeMillis;
		FMOD_Channel_GetCurrentSound(MusicChannel, &pCurrentMusic);
		FMOD_Sound_GetLength(pCurrentMusic, &TotalTimeMillis, FMOD_TIMEUNIT_MS);
		return TotalTimeMillis;
	}

	//���� ������� ���� ���� �������� time�� �и������� ������ �����Ѵ�.
	unsigned int getMusicCurrentPlayTime() {
			unsigned int CurrentTimeMillis;
			FMOD_Channel_GetPosition(MusicChannel, &CurrentTimeMillis, FMOD_TIMEUNIT_MS);
			return CurrentTimeMillis;
	}

	void Release()
	{
		delete MusicChannel;

		for (int i = 0; i < EFFECT_SOUND_COUNT; i++)
		{
			FMOD_Sound_Release(pGlobalSounds[i]);
			delete pGlobalSounds[i];
		}

		delete pGlobalSounds;


		FMOD_Sound_Release(pMusic);
		delete pMusic;

		FMOD_System_Close(pFMOD_System);
		FMOD_System_Release(pFMOD_System);
	}

	~SoundSystem()
	{
		Release();
		delete pFMOD_System;
	}
};