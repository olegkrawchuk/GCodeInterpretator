
#define GCodeInterpret_EXPORTS
#include "GCodeInterpret.h"

std::vector<unsigned char> countLines;

namespace gcode
{
	std::string ToStr(const double& value)
	{
		std::string str = std::to_string(value);
		while (str.back() == '0')
			str.pop_back();

		if (str.back() == ',' || str.back() == '.')
			str.pop_back();

		return str;
	}


	const unsigned char ZPos::Top = 180;
	const unsigned char ZPos::Bottom = 40;


	GCodeInterpret::GCodeInterpret()
	{
		_relX = 0;
		_relY = 0;
		_currentX = _relX;
		_currentY = _relY;

		history = new std::vector<std::string>();

		AddHeader();
	}
	GCodeInterpret::~GCodeInterpret()
	{
		delete history;
	}


	

	void GCodeInterpret::AddToHistory(std::string & cmd)
	{
		history->push_back(cmd);
	}

	void GCodeInterpret::pop_back(const unsigned char& count)
	{
		for (auto i = 0; i < count; ++i)
			history->pop_back();
	}



	std::string GCodeInterpret::Delay(const unsigned long& millis, bool saveToHistory)
	{
		std::string cmd = "G4 P" + std::to_string(millis);

		if (saveToHistory)
		{
			if (history->back().substr(0, 4) != cmd.substr(0, 4))
				AddToHistory(cmd);
		}

		return cmd;
	}

	std::string GCodeInterpret::OffPower(bool saveToHistory)
	{
		std::string cmd = "M18";

		if (saveToHistory)
		{
			AddToHistory(cmd);
			countLines.push_back(1);
		}

		return cmd;
	}

	std::string GCodeInterpret::SetSpeed(const unsigned long& speed, bool saveToHistory)
	{
		std::string cmd = "G0 F" + std::to_string(speed);

		if (saveToHistory)
		{
			AddToHistory(cmd);
			countLines.push_back(1);
		}

		return cmd;
	}

	std::string GCodeInterpret::MoveToStartPosition(bool saveToHistory)
	{
		std::string cmd = "G28";

		if (saveToHistory)
		{
			_currentX = 0;
			_currentY = 0;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);
			
		return cmd;
	}

	std::string GCodeInterpret::MoveX(const double& x, bool saveToHistory)
	{
		double X = CalculateX(x);

		std::string cmd = "G0 X" + ToStr(X);

		if (saveToHistory)
		{
			_currentX = X;
			AddToHistory(cmd);
			countLines.push_back(2);
		}
			
		WaitMoveProcess(saveToHistory);

		return cmd;
	}

	std::string GCodeInterpret::MoveX(const double& x, const unsigned long& speed, bool saveToHistory)
	{
		double X = CalculateX(x);

		std::string cmd = "G0 F" + std::to_string(speed) + " X" + ToStr(X);

		if (saveToHistory)
		{
			_currentX = X;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);
			
		return cmd;
	}

	std::string GCodeInterpret::MoveY(const double& y, bool saveToHistory)
	{
		double Y = CalculateY(y);

		std::string cmd = "G0 Y" + ToStr(Y);

		if (saveToHistory)
		{
			_currentY = Y;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);
			
		return cmd;
	}

	std::string GCodeInterpret::MoveY(const double& y, const unsigned long& speed, bool saveToHistory)
	{
		double Y = CalculateY(y);

		std::string cmd = "G0 F" + std::to_string(speed) + " Y" + ToStr(Y);

		if (saveToHistory)
		{
			_currentY = Y;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);

		return cmd;
	}

	std::string GCodeInterpret::MoveXY(const double& x, const double& y, bool saveToHistory)
	{
		double X = CalculateX(x);
		double Y = CalculateY(y);

		std::string cmd = 
			"G0 X" + ToStr(X) +
			" Y" + ToStr(Y);


		if (saveToHistory)
		{
			_currentX = X;
			_currentY = Y;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);

		return cmd;
	}

	std::string GCodeInterpret::MoveXY(const double& x, const double& y, const unsigned long& speed, bool saveToHistory)
	{
		double X = CalculateX(x);
		double Y = CalculateY(y);

		std::string cmd = 
			"G0 F" + std::to_string(speed) + 
			" X" + ToStr(X) +
			" Y" + ToStr(Y);

		if (saveToHistory)
		{
			_currentX = X;
			_currentY = Y;
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		WaitMoveProcess(saveToHistory);

		return cmd;
	}

	std::string GCodeInterpret::Capture(bool saveToHistory)
	{
		std::string cmd = "M106";

		if (saveToHistory)
		{
			AddToHistory(cmd);
			countLines.push_back(2);
		}

		Delay(500, saveToHistory);

		return cmd;
	}

	std::string GCodeInterpret::Release(bool saveToHistory)
	{
		std::string cmd = "M107";

		if (saveToHistory)
		{
			AddToHistory(cmd);
			countLines.push_back(2);
		}
			
		Delay(500, saveToHistory);

		return cmd;
	}

	/*std::string GCodeInterpret::MoveZ(const ZPos& pos, bool saveToHistory)
	{
		return MoveZ((unsigned short)pos, saveToHistory);
	}*/

	std::string GCodeInterpret::MoveZ(const unsigned short& z, bool saveToHistory)
	{
		if (z < 0 || z > 180)
			throw std::exception("OutOfRangeException. ƒопустимый диапазон значений параметра z: 0 - 180");

		std::string cmd = "M280 P0 S" + std::to_string(z);

		if (saveToHistory)
		{
			AddToHistory(cmd);
			countLines.push_back(1);
		}

		return cmd;
	}

	std::string GCodeInterpret::Up(bool saveToHistory)
	{
		return MoveZ(ZPos::Top, saveToHistory);
	}

	std::string GCodeInterpret::Down(bool saveToHistory)
	{
		return MoveZ(ZPos::Bottom, saveToHistory);
	}

	void GCodeInterpret::AddHeader()
	{
		Up();
		MoveToStartPosition();
	}

	void GCodeInterpret::AddFooter()
	{
		if (history->back() != OffPower(false))
		{
			Up();
			MoveToStartPosition();
			OffPower();
		}
	}

	void GCodeInterpret::RemoveFooter()
	{
		if (history->back() == OffPower(false))
		{
			RemoveLast(3);
		}
	}

	std::string GCodeInterpret::WaitMoveProcess(bool saveToHistory)
	{
		std::string cmd = "M400";

		if (saveToHistory)
		{
			if(history->back() != cmd)
				AddToHistory(cmd);
		}

		return cmd;
	}

	void GCodeInterpret::SetRelativeCoordinates(const double& x, const double& y)
	{
		_relX = x;
		_relY = y;
	}

	double& GCodeInterpret::GetCurrentX()
	{
		return _currentX;
	}

	double& GCodeInterpret::GetCurrentY()
	{
		return _currentY;
	}

	std::vector<std::string>& GCodeInterpret::GetHistory()
	{
		return *history;
	}

	void GCodeInterpret::ClearHistory()
	{
		history->clear();
		countLines.clear();
		AddHeader();
	}

	double GCodeInterpret::CalculateX(const double& x)
	{
		return x + _relX;
	}

	double GCodeInterpret::CalculateY(const double& y)
	{
		return y + _relY;
	}

	void GCodeInterpret::SaveHistory(Writer * writer)
	{
		if (!history->empty())
		{
			AddFooter();

			std::string str;

			for (auto const& cmd : *history)
				str += cmd + "\n";

			str.pop_back();

			writer->Write(str);

			RemoveFooter();
			
		}
	}
	void GCodeInterpret::RemoveLast(size_t count)
	{
		auto size = countLines.size();
		if (size < count)
			count = size;
		
		for (auto i = 0; i < count; ++i)
		{
			pop_back(countLines.back());
			countLines.pop_back();
		}
	}
}
