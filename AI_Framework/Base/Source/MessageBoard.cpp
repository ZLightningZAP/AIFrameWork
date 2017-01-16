#include "MessageBoard.h"

MessageBoard::MessageBoard()
{
	Reset();
}

MessageBoard::~MessageBoard()
{
}

void MessageBoard::SetMessage(string _message)
{
	Message = _message;
}

string MessageBoard::GetMsg()
{
	return Message;
}

void MessageBoard::SetFromLabel(string _fromlabel)
{
	FromLabel = _fromlabel;
}

string MessageBoard::GetFromLabel()
{
	return FromLabel;
}

void MessageBoard::SetToLabel(string _tolabel)
{
	ToLabel = _tolabel;
}

string MessageBoard::GetToLabel()
{
	return ToLabel;
}

void MessageBoard::Reset()
{
	Message = "Nil";
	FromLabel = "Nil";
	ToLabel = "Nil";
}