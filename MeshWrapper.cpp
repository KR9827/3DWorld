#include "MeshWrapper.h"

namespace s3d {

	// Vertex3Dを==で比較できるようにする
	inline bool operator==(const Vertex3D& lhs, const Vertex3D& rhs)
	{
		// 座標と法線とUVが全て同じならtrue、1つでも違うならfalse
		return lhs.pos == rhs.pos && lhs.normal == rhs.normal && lhs.tex == rhs.tex;
	}

	inline bool operator!=(const Vertex3D& lhs, const Vertex3D& rhs)
	{
		return !(lhs == rhs);
	}

	// TriangleIndex32を==で比較できるようにする
	inline bool operator==(const TriangleIndex32& a, const TriangleIndex32& b)
	{
		// 三角形を構成する番号が同じならtrue
		return a.i0 == b.i0 && a.i1 == b.i1 && a.i2 == b.i2;
	}

	inline bool operator!=(const TriangleIndex32& a, const TriangleIndex32& b)
	{
		return !(a == b);
	}
}


/// @brief コンストラクタ
/// @param vertices 頂点
/// @param indices インデックス
MeshWrapper::MeshWrapper(const Array<Vertex3D>& vertices, const Array<TriangleIndex32>& indices)
	: m_currentVertices(vertices)
	, m_currentIndices(indices)
{
	MeshData meshData{ m_currentVertices, m_currentIndices };			// CPU側のメッシュ情報を取得
	m_mesh = DynamicMesh{ meshData };									// GPU用Meshを作成
}


/// @brief メッシュ更新する
/// @param vertices 頂点
/// @param indices インデックス
/// @note スキニング後に呼ぶ
void MeshWrapper::UpdateMeshData(const Array<Vertex3D>& vertices, const Array<TriangleIndex32>& indices)
{
	// Console << U"currentVertices：" << m_currentVertices.size() << U"vertices：" << vertices.size();

	// 新しいデータで上書き
	m_currentVertices = vertices;
	m_currentIndices = indices;
	MeshData meshData{ m_currentVertices, m_currentIndices };
	m_mesh.fill(meshData);										// すでにあるGPUバッファの中身だけ更新
}

void MeshWrapper::Draw(const Mat4x4& transform, const Optional<Texture>& texture) const
{
	if (texture)
	{
		// テクスチャ付き描画
		m_mesh.draw(transform, *texture);
	}
	else
	{
		m_mesh.draw(transform);
	}
}
