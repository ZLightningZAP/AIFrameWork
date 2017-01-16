#pragma once
#include <iostream>
#include <string>
using namespace std;

class MessageBoard
{
public:
	MessageBoard();
	~MessageBoard();

	void Reset();
	void SetMessage(string _message);
	string GetMessage();
	void SetFromLabel(string _fromlabel);
	string GetFromLabel();
	void SetToLabel(string _tolabel);
	string GetToLabel();

private:
	string Message, FromLabel, ToLabel;
};
