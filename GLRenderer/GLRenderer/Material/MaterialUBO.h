#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

#include <GLRenderer/Material/MaterialStruct.h>

namespace GLEngine::GLRenderer {

class C_MaterialBuffer final : public Buffers::C_UniformBuffer {
public:
	C_MaterialBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

	S_Material m_Material;
};
}
