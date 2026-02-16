#include "Settings.h"

Settings::Settings()
{
}

Settings::~Settings()
{
}

bool Settings::Load()
{
	if (FileSystem::Exists(FILE_PATH))
	{
		const JSON json = JSON::Load(FILE_PATH);
		if (!json) return false;

		m_masterVolume = json[U"masterVolume"].getOr<double>(1.0);
		m_bgmVolume = json[U"bgmVolume"].getOr<double>(0.8);
		m_seVolume = json[U"seVolume"].getOr<double>(0.8);
		m_cameraSensitivity = json[U"cameraSensitivity"].getOr<double>(0.005);

		return true;
	}
	else
	{
		Print << U"[Settings] JSONファイルが存在しません。新規作成します。";
	}

	// ファイルがない場合
	m_masterVolume = 1.0;
	m_bgmVolume = 0.8;
	m_seVolume = 0.8;
	m_cameraSensitivity = 0.005;

	Save();

	return true;
}

void Settings::Save() const
{
	JSON json;
	json[U"masterVolume"] = m_masterVolume;
	json[U"bgmVolume"] = m_bgmVolume;
	json[U"seVolume"] = m_seVolume;
	json[U"cameraSensitivity"] = m_cameraSensitivity;

	json.save(FILE_PATH);
}
