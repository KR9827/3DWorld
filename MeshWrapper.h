#pragma once
#include <Siv3D.hpp>
#include <optional>

class MeshWrapper
{
public:
	MeshWrapper(const Array<Vertex3D>& vertices, const Array<TriangleIndex32>& indices);

	void UpdateMeshData(const Array<Vertex3D>& vertices, const Array<TriangleIndex32>& indices);
	void Draw(const Mat4x4& transform, const Optional<Texture>& texture = none) const;

private:
	DynamicMesh m_mesh;							// DynamicMesh：中身を書き換える前提（動的）
	Array<Vertex3D> m_currentVertices;
	Array<TriangleIndex32> m_currentIndices;
};
