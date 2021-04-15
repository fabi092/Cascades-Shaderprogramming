#include "FileWatcher.h"
#include <iostream>

FileWatcher::FileWatcher(const char* filePath, const Delegate& delegate, long msCycle) : FileWatcher(&filePath, 1, delegate, msCycle)
{}

FileWatcher::FileWatcher(const char** filePath, int fileCount, const Delegate& delegate, long msCycle) : m_fileCount(fileCount), m_path(new std::experimental::filesystem::path[m_fileCount]), m_lastLoad(new std::chrono::system_clock::time_point[m_fileCount]), m_callBack(delegate), m_msCycle(msCycle), m_stop(false)
{
	for (int i = 0; i < m_fileCount; ++i)
	{
		if (filePath[i] == nullptr)
			continue;

		m_path[i] = std::experimental::filesystem::path(filePath[i]);
		if (!std::experimental::filesystem::exists(m_path[i]))
		{
			m_path[i] = "";
			continue;
		}

		m_lastLoad[i] = std::chrono::system_clock::now();

	}
	m_watcherThread = std::thread(&FileWatcher::CheckFile, this);
}

FileWatcher::~FileWatcher()
{
	delete[] m_path;
	delete[] m_lastLoad;
	m_stop = true;
	m_watcherThread.join();
}

void FileWatcher::CheckFile() const
{
	while (!m_stop)
	{
		std::this_thread::sleep_for(std::chrono::system_clock::duration(m_msCycle));

		for (int i = 0; i < m_fileCount; ++i)
		{
			if (m_path[i] == "")
				continue;

			std::chrono::system_clock::time_point modifyStamp;
			try
			{
				modifyStamp = std::experimental::filesystem::last_write_time(m_path[i]);
			}
			catch (std::exception e)
			{
				std::cout << "ERROR " << e.what() << " | " << __FILE__ << " (" << __LINE__ << ")" << std::endl;
				modifyStamp = m_lastLoad[i];
			}

			if (modifyStamp > m_lastLoad[i])
			{
				m_callBack.Invoke();
				m_lastLoad[i] = modifyStamp;
			}
		}
	}
}
