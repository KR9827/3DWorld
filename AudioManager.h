#pragma once
#include <Siv3D.hpp>
#include <memory>



enum class Bus
{
	BGM,
	SE,
	Count,
};


class AudioManager
{
public:
	AudioManager(std::shared_ptr<class Settings> settings);
	~AudioManager();

	// ロード系
	bool PreLoadBGM(const String& id, const FilePath& path);
	bool PreLoadSE(const String& id, const FilePath& path);

	// 再生（bus指定）
	void PlayBGM(const String& id, bool loop = true);
	void StopBGM(Seconds fade = 0s);
	void PlaySE(const String& id);

	void PlaySEPan(const String& id, const Vec3& sourcePos, const class Camera& camera);		// 位置によってパンを変更する関数

	// マスター/バスの音量制御
	void SetMasterVolume(double v);
	double GetMasterVolume() const;
	void SetBusVolume(Bus bus, double v);
	double GetBusVolume(Bus bus) const;

private:
	std::shared_ptr<class Settings> m_settings;

	// 音源の実体
	HashTable<String, Audio> m_bgmMap;					// unordered_mapのsiv3Dバージョン
	HashTable<String, Audio> m_seMap;
	HashTable<String, Audio> m_sePanMap;

	String m_currentBGMId;

	// 音量
	double m_masterVolume{ 0.0 };
	double m_busVolume[static_cast<int>(Bus::Count)] = {};

};
