/*
 * scanner.hh - The header file for the scanner class which is used to process 
 * 		configuration files.
 * Copyright (C) 2000 Frank Hale
 * frankhale@lycos.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 5 July 2000
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _SCANNER_HH_
#define _SCANNER_HH_

#include <iostream.h>
#include <fstream.h>
#include <cstdlib>
#include <string>

#define MAX_TOKEN_BUFFER 256

class Scanner {
private:

	std::string s_token, s_temp_token;

	int CURRENT_TOKEN_BUFFER_LENGTH;

	enum char_codes { 
		letters, 
		special, 
		whitespace, 
		openbrace, 
		closebrace, 
		openbracket, 
		closebracket, 
		openparen,
		closeparen,
		asterix,
		quote,
		equals,
		semicolon
	};
	
	char_codes char_class[128];

	ifstream inputFile;

	long backup_pos;
	
	char character;

	bool tokensRelaxed;

public:
	Scanner(char *filename);
	~Scanner();
	void getNextToken(bool spaces=false);
	std::string currentToken() const { return s_token; }
	
	bool match(std::string m);
	void rewind() { inputFile.seekg(0); }
	bool eof();
	
	void concat(int type);

	void defaultTokens();
	void relaxedTokens();
};

#endif
