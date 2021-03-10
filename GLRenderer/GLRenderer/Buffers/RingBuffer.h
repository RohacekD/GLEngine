#pragma once


namespace GLEngine::GLRenderer::Buffers {
template<class T>
class C_RingBuffer
{
public:
	C_RingBuffer(std::size_t num);
	std::size_t GetSize() const;
	//void 

private:
	std::vector<T>	m_Buffers;
	std::size_t		m_Num;
};

}

