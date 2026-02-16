#pragma once
#include <Siv3D.hpp>

class Settings
{
public:
	Settings();
	~Settings();

	// 各オーディオの音量変数
	double m_masterVolume{ 1.0 };
	double m_bgmVolume{ 0.8 };
	double m_seVolume{ 0.8 };

	// カメラの感度
	double m_cameraSensitivity{ 0.005 };

	// 設定ファイルのパス
	static inline const FilePath FILE_PATH{ U"Data/Settings.json" };

	bool Load();
	void Save() const;
};
