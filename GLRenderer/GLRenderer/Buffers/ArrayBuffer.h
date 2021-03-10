/**
 * ==============================================
 * \file 		UniformBuffer.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

namespace GLEngine::GLRenderer::Buffers {
template<class StoredType, std::size_t size>
class C_ArrayUniformBuffer : public C_UniformBuffer {
public:
	C_ArrayUniformBuffer(const std::string& blockName, unsigned int index);

	virtual void UploadData() const override;
	void SetElement(const StoredType& element, std::size_t index);
	void SetElement(StoredType&& element, std::size_t index);

private:
	std::array<StoredType, size> m_Storage;
	std::size_t m_Size;
};
}