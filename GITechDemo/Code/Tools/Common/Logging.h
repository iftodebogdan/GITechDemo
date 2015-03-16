#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

namespace LibRendererTools
{
	class mstream
	{
	public:
		bool m_bVerbose;
		ofstream coss;
		mstream(const char* fileName, unsigned int mode, bool verbose = true)
		{
			m_bVerbose = verbose; coss.open(fileName, mode);
		}
		~mstream(void)
		{
			if (coss.is_open()) coss.close();
		}
	};

	template <class T>
	mstream& operator<< (mstream& st, T val)
	{
		st.coss << val;
#ifdef _DEBUG
		st.coss.flush();
		std::ostringstream oss;
		oss << val;
		OutputDebugString(oss.str().c_str());
#endif
		if (st.m_bVerbose)
			cout << val;
		return st;
	};
}

#endif // LOGGING_H