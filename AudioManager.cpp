#include <cmath>
#include "AudioManager.h"
#include "Settings.h"
#include "Camera.h"

AudioManager::AudioManager(std::shared_ptr<Settings> settings)
	:m_settings(settings)
{
	if (m_settings)
	{
		m_masterVolume = m_settings->m_masterVolume;
		m_busVolume[static_cast<int>(Bus::BGM)] = m_settings->m_bgmVolume;
		m_busVolume[static_cast<int>(Bus::SE)] = m_settings->m_seVolume;
	}

	// グローバルオーディオに初期値を設定
	GlobalAudio::SetVolume(m_masterVolume);
	GlobalAudio::BusSetVolume(MixBus0, m_busVolume[static_cast<int>(Bus::BGM)]);
	GlobalAudio::BusSetVolume(MixBus1, m_busVolume[static_cast<int>(Bus::SE)]);
}

AudioManager::~AudioManager()
{
	StopBGM();
	m_bgmMap.clear();
	m_seMap.clear();
	m_sePanMap.clear();
}

// ロード系
bool AudioManager::PreLoadBGM(const String& id, const FilePath& path)
{
	auto [it, inserted] = m_bgmMap.try_emplace(id, Audio::Stream, path);		// it: 挿入先のイテレータ、inserted: 新規挿入が行われたかを表すフラグ
																				// try_emplace：キーが存在しなければ、新しく挿入する。
	// 既に存在していた場合
	if (!inserted) return true;

	return true;
}

bool AudioManager::PreLoadSE(const String& id, const FilePath& path)
{
	auto [it, inserted] = m_seMap.try_emplace(id, path);

	if (!inserted) return true;

	return true;
}

// 再生（bus指定）
void AudioManager::PlayBGM(const String& id, bool loop)
{
	// 登録されていない/再生中の場合は何にもしない
	if (!m_bgmMap.contains(id)) return;
	if (m_currentBGMId == id && m_bgmMap[id].isPlaying()) return;

	// 再生処理
	m_currentBGMId = id;
	m_bgmMap[id].setLoop(loop);
	m_bgmMap[id].play(MixBus0);
}

void AudioManager::StopBGM(Seconds fade)
{
	if (m_currentBGMId.isEmpty()) return;

	m_bgmMap[m_currentBGMId].stop(fade);
	m_currentBGMId.clear();
}

void AudioManager::PlaySE(const String& id)
{
	if (!m_seMap.contains(id)) return;

	// 再生処理
	m_seMap[id].playOneShot(MixBus1);
}

// 位置によってパンを変更する関数
void AudioManager::PlaySEPan(const String& id, const Vec3& sourcePos, const Camera& camera)
{
	if (!m_seMap.contains(id)) return;

	// カメラのビュー行列
	const Mat4x4 view = camera.GetCamera().getView();

	// 音源をカメラ空間へ変換
	const Vec3 camSpace = view.transformPoint(sourcePos);

	//// 音源とリスナーの距離
	//const Vec2 dir = sourcePos - listenerPos;
	//const double dist = dir.length();

	// 左右のパンの計算
	const double maxPanDist{ 20.0 };
	double pan = camSpace.x / maxPanDist;
	pan = Clamp(pan, -1.0, 1.0);

	//const double angle = std::atan2(dir.y, dir.x);						// 音源への角度（ラジアン：[-π, π]）
	//double pan = std::cos(angle);										// panの値をx座標に対して[-1, 1]に変換
	//pan = Clamp(pan, -1.0, 1.0);										// Clamp:上限と下限を設定する

	// 距離減衰の計算
	const double dist = camSpace.length();
	const double refDist{ 30.0 };

	double atten = 1.0 / (1.0 + (dist * dist) / (refDist * refDist));
	atten = Clamp(atten, 0.0, 1.0);

	// 対象が後ろの場合
	if (camSpace.z < 0)
	{
		atten *= 0.6;
	}

	//const double refDist = 200.0;										// 参照距離（音量が半分くらいになる距離）
	//double atten = 1.0 / (1.0 + (dist * dist) / (refDist * refDist));	// 距離による減衰計算
	//atten = Clamp(atten, 0.0, 1.0);

	// 音量設定
	double vol = m_busVolume[static_cast<int>(Bus::SE)] * atten;		// SEバスの音量に距離減衰を掛ける
	vol = Clamp(vol, 0.0, 1.0);

	// 再生処理
	m_seMap[id].playOneShot(MixBus1, vol, pan);
}

// マスター/バスの音量制御
void AudioManager::SetMasterVolume(double v)
{
	m_masterVolume = v;
	GlobalAudio::SetVolume(m_masterVolume);

	// 変更された音量をsettingsに反映させる
	if (m_settings) m_settings->m_masterVolume = m_masterVolume;
}

double AudioManager::GetMasterVolume() const
{
	return m_masterVolume;
}

void AudioManager::SetBusVolume(Bus bus, double v)
{
	const int index = static_cast<int>(bus);
	m_busVolume[index] = v;

	// 変更された音量をsettingsに反映させる
	if (m_settings)
	{
		if (bus == Bus::BGM)
		{
			GlobalAudio::BusSetVolume(MixBus0, m_busVolume[index]);
			m_settings->m_bgmVolume = m_busVolume[index];
		}
		if (bus == Bus::SE)
		{
			GlobalAudio::BusSetVolume(MixBus1, m_busVolume[index]);
			m_settings->m_seVolume = m_busVolume[index];
		}
	}
}

double AudioManager::GetBusVolume(Bus bus) const
{
	return m_busVolume[static_cast<int>(bus)];
}
