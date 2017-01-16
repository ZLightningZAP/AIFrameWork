#ifndef MESSAGE_BOARD_H
#define MESSAGE_BOARD_H

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
	string GetMsg();
	void SetFromLabel(string _fromlabel);
	string GetFromLabel();
	void SetToLabel(string _tolabel);
	string GetToLabel();

private:
	string Message;
	string FromLabel;
	string ToLabel;
};
#endif
