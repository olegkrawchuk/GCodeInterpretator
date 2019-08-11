#define GCodeInterpret_EXPORTS
#include "GCodeInterpret.h"

#include <fstream>
#include <time.h>

namespace gcode
{
	std::string GetDate()
	{
		time_t now = time(0);
		tm tstruct;
		char buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d %H.%M.%I", &tstruct);

		return buf;
	}


	void TXTWriter::Write(std::string data)
	{
		std::string filename = GetDate() + ".gcode";
		std::ofstream out(filename);
		out << data;
		out.close();
	}
}