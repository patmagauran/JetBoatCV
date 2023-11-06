#pragma once
#include <vector>
#include <mutex>
template <typename T>
class AppendOnlyVector
{
	private:
	std::mutex m_data_mutex;

	std::vector<T> m_data;
	size_t expandCap;
	size_t originalSize;
	public:
	AppendOnlyVector(size_t size = 512);
	AppendOnlyVector(const AppendOnlyVector& other);
	~AppendOnlyVector();

	void push_back(const T& item);
	void push_back(T&& item);


	void reserve(size_t count);

	size_t size() const;
	size_t capacity() const;

	const T& operator[](size_t index) const;
	T& operator[](size_t index);

	const T& at(size_t index) const;
	T& at(size_t index);

	

};

template<typename T>
inline AppendOnlyVector<T>::AppendOnlyVector(size_t size) : m_data(std::vector<T>()), expandCap(0), originalSize(size) {
	this->reserve(size);
}

template<typename T>
inline AppendOnlyVector<T>::AppendOnlyVector(const AppendOnlyVector& other)
{
	this->m_data = other.m_data;
	this->expandCap = other.expandCap;
	this->originalSize = other.originalSize;
}

template<typename T>
inline AppendOnlyVector<T>::~AppendOnlyVector()
{
}

template<typename T>
inline void AppendOnlyVector<T>::push_back(const T& item)
{
	if (this->size() > this->expandCap) {
		reserve(this->originalSize);
	}
	m_data.push_back(item);
}

template<typename T>
inline void AppendOnlyVector<T>::push_back(T&& item)
{
	if (this->size() > this->expandCap) {
		reserve(this->originalSize);
	}
	m_data.push_back(item);
}

template<typename T>
inline void AppendOnlyVector<T>::reserve(size_t count)
{
	std::lock_guard<std::mutex> lock(m_data_mutex);
	m_data.reserve(count);
	expandCap += count * 0.75;
}

template<typename T>
inline size_t AppendOnlyVector<T>::size() const
{
	return m_data.size();
}

template<typename T>
inline size_t AppendOnlyVector<T>::capacity() const
{
	return m_data.capacity();
}

template<typename T>
inline const T& AppendOnlyVector<T>::operator[](size_t index) const
{
	return m_data[index];
}

template<typename T>
inline T& AppendOnlyVector<T>::operator[](size_t index)
{
	return m_data[index];
}

template<typename T>
inline const T& AppendOnlyVector<T>::at(size_t index) const
{
	return m_data.at(index);
}

template<typename T>
inline T& AppendOnlyVector<T>::at(size_t index)
{
	return m_data.at(index);
}
