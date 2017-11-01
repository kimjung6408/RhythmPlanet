#pragma once
#include<string>
#include<vector>
#include<fstream>
#include"d3dApp.h"
using namespace std;

class Music
{
private:
	float MusicLength; //곡의 길이를 floating point형태로 저장. 초 단위로 저장.

	string SongTitle;
	string Artist;
	string FolderName;
	string BPM;
	string GameDataFileName;
	string SoundFile;
	string IntroSoundFile;
	string ThemeName;
	ID3D11ShaderResourceView* BGImage;
	ID3D11ShaderResourceView* IntroImage;
	
	//Sound* musicFile

public:
	Music(string FolderName, string SongTitle ,string Artist, string BPM, string SoundFile, string IntroSoundFile, string BG_file, string Intro_BG_file, string GameDataFile, string Theme, ID3D11Device* inDevice)
	{
		this->SongTitle = SongTitle;
		this->FolderName = FolderName;
		this->BPM = BPM;
		this->Artist = Artist;
		this->SoundFile = SoundFile;
		this->IntroSoundFile = IntroSoundFile;
		this->ThemeName = Theme;
		this->GameDataFileName = GameDataFile;
		string BGPath = "Songs/" + FolderName + "/" + BG_file;
		string IntroImagePath="Songs/" + FolderName + "/" + Intro_BG_file;

		std::wstring stemp1 = std::wstring(BGPath.begin(), BGPath.end());
		LPCWSTR sw1 = stemp1.c_str();
		std::wstring stemp2 = std::wstring(IntroImagePath.begin(), IntroImagePath.end());
		LPCWSTR sw2 = stemp2.c_str();
		HR(D3DX11CreateShaderResourceViewFromFile(inDevice, sw2, 0, 0, &IntroImage, 0));
		HR(D3DX11CreateShaderResourceViewFromFile(inDevice, sw1, 0, 0, &BGImage, 0));
	}

	string GetstrBPM()
	{
		return BPM;
	}

	float GetFloatBPM()
	{
		return atof(&BPM[0]);
	}

	ID3D11ShaderResourceView* GetBGImageSRV()
	{
		return BGImage;
	}

	ID3D11ShaderResourceView* GetIntroImage()
	{
		return IntroImage;
	}

	string GetTitle()
	{
		return SongTitle;
	}

	string GetArtist()
	{
		return Artist;
	}

	string GetIntroSoundPath()
	{
		string IntroSoundPath = "Songs/" + FolderName + "/" + IntroSoundFile;
		return IntroSoundPath;
	}

	string GetSoundPath()
	{
		string SoundPath = "Songs/" + FolderName + "/" + SoundFile;
		return SoundPath;
	}

	string GetGameDataPath()
	{
		string GameDataPath = "Songs/" + FolderName + "/" + GameDataFileName;
		return GameDataPath;
	}
};