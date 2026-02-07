#pragma once
#include <Siv3D.hpp>

struct SkinnedVertex
{
	Vec3 originalPos;						// 元の位置
	Vec3 pos;								// スキニング後の位置
	Vec3 normal;							// 法線
	Vec2 tex;								// UV座標
	
	Array<int32> boneIndices;				// 影響を与えるボーンのインデックス
	Array<float> boneWeight;				// ボーンに対応する重み
};
