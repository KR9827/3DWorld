#include "AnimationHelper.h"

namespace AnimationHelper
{
	int32 FindKeyIndex(float currentTime, const aiVectorKey* keys, int32 numKeys)
	{
		// currentTime が keys[i].mTime と keys[i + 1].mTime の間にある時、keys[i] と keys[i + 1]を使って補間する
		for (int32 i = 0; i < numKeys - 1; ++i)
		{
			if (currentTime < keys[i + 1].mTime)
			{
				return i;
			}
		}

		// animetionTimeが最後のキーより後ろの場合は補間せずに最後の2フレームの間で補間させる
		return numKeys - 2;
	}

	int32 FindKeyIndex(float currentTime, const aiQuatKey* keys, int32 numKeys)
	{
		for (int32 i = 0; i < numKeys - 1; ++i)
		{
			if (currentTime < keys[i + 1].mTime)
			{
				return i;
			}
		}

		return numKeys - 2;
	}

	float GetInterpolationFactor(float currentTime, float startTime, float endTime)
	{
		float delta{ endTime - startTime };
		if (delta == 0.0f)
		{
			return 0.0f;
		}
		else
		{
			return (currentTime - startTime) / delta;
		}
	}

	Vec3 InterpolatePosition(aiNodeAnim* channel, float animationTime)
	{
		if (channel->mNumPositionKeys == 1)
		{
			return Vec3{ channel->mPositionKeys[0].mValue.x, channel->mPositionKeys[0].mValue.y, channel->mPositionKeys[0].mValue.z };
		}

		// 使うキーフレームを特定する
		int32 index{ FindKeyIndex(animationTime, channel->mPositionKeys, channel->mNumPositionKeys) };
		aiVector3D start{ channel->mPositionKeys[index].mValue };
		aiVector3D end{ channel->mPositionKeys[index + 1].mValue };
		float factor{ GetInterpolationFactor(animationTime, static_cast<float>(channel->mPositionKeys[index].mTime), static_cast<float>(channel->mPositionKeys[index + 1].mTime)) };

		// 位置はvector3Dなので線形補間
		return Lerp(Vec3{ start.x, start.y, start.z }, Vec3{ end.x, end.y, end.z }, factor);
	}

	aiQuaternion InterpolateRotation(aiNodeAnim* channel, float animationTime)
	{
		if (channel->mNumRotationKeys == 1)
		{
			return channel->mRotationKeys[0].mValue;
		}

		int32 index{ FindKeyIndex(animationTime, channel->mRotationKeys, channel->mNumRotationKeys) };
		aiQuaternion start{ channel->mRotationKeys[index].mValue };
		aiQuaternion end{ channel->mRotationKeys[index + 1].mValue };

		float factor{ GetInterpolationFactor(animationTime, static_cast<float>(channel->mRotationKeys[index].mTime), static_cast<float>(channel->mRotationKeys[index + 1].mTime)) };

		aiQuaternion out;
		aiQuaternion::Interpolate(out, start, end, factor);
		out.Normalize();

		return out;
	}

	Vec3 InterpolateScale(aiNodeAnim* channel, float animationTime)
	{
		if (channel->mNumScalingKeys == 1)
		{
			return Vec3{ channel->mScalingKeys[0].mValue.x, channel->mScalingKeys[0].mValue.y, channel->mScalingKeys[0].mValue.z };
		}

		int32 index{ FindKeyIndex(animationTime, channel->mScalingKeys, channel->mNumScalingKeys) };
		aiVector3D start{ channel->mScalingKeys[index].mValue };
		aiVector3D end{ channel->mScalingKeys[index + 1].mValue };

		float factor{ GetInterpolationFactor(animationTime, static_cast<float>(channel->mScalingKeys[index].mTime), static_cast<float>(channel->mScalingKeys[index + 1].mTime)) };

		// スケールはVector3Dなので線形補間
		return Lerp(Vec3{ start.x, start.y, start.z }, Vec3{ end.x, end.y, end.z }, factor);
	}

	Vec3 Lerp(const Vec3& start, const Vec3& end, float factor)
	{
		return start + factor * (end - start);
	}

	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float factor)
	{
		return q1.slerp(q2, factor);
	}

	void PrintAiMatrix(const aiMatrix4x4& m, const String& label)
	{
		if (!label.isEmpty())
		{
			Console << U"[{}]"_fmt(label);
		}

		Console << U"[ {:.3f}, {:.3f}, {:.3f}, {:.3f} ]"_fmt(m.a1, m.a2, m.a3, m.a4);
		Console << U"[ {:.3f}, {:.3f}, {:.3f}, {:.3f} ]"_fmt(m.b1, m.b2, m.b3, m.b4);
		Console << U"[ {:.3f}, {:.3f}, {:.3f}, {:.3f} ]"_fmt(m.c1, m.c2, m.c3, m.c4);
		Console << U"[ {:.3f}, {:.3f}, {:.3f}, {:.3f} ]"_fmt(m.d1, m.d2, m.d3, m.d4);
	}
}
