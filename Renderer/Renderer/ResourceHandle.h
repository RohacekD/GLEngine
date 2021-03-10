#pragma once

namespace GLEngine::Renderer {

template<class T>
class C_ResourceHandle
{
public:
	C_ResourceHandle()
		: m_Counter(new std::atomic<unsigned int>())
	{
		addRef();
	}

	C_ResourceHandle(const C_ResourceHandle& other)
		: m_Counter(other.m_Counter)
	{
		addRef();
	}

	C_ResourceHandle(C_ResourceHandle&& other)
		: m_Counter(other.m_Counter)
	{
		other.m_Counter = nullptr;
	}

	C_ResourceHandle& operator=(const C_ResourceHandle& other)
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		m_Counter = other.m_Counter;
		addRef();
		return *this;
	}

	C_ResourceHandle& operator=(C_ResourceHandle&& other)
	{
		if (&other == this)
		{
			return *this;
		}
		release();
		m_Counter = other.m_Counter;
		other.m_Counter = nullptr;
		return *this;
	}

	~C_ResourceHandle()
	{
		if (!m_Counter)
		{
			return; // already released by move
		}

		release();
	}

	void release()
	{
		GLE_ASSERT(m_Counter, "Uninitalized C_ResourceHandle");
		m_Counter--;
		if(m_Counter->load() == 0)
			static_cast<T*>(this)->reset();
	}

	void addRef()
	{
		GLE_ASSERT(m_Counter, "Uninitalized C_ResourceHandle");
		m_Counter++;
	}
private:
	std::atomic<unsigned int>* m_Counter;
};

}

