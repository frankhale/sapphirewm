/*
 * Copyright (C) 1999,2000,2001 Frank Hale
 * frankhale@yahoo.com
 * http://sapphire.sourceforge.net/
 *
 * Updated: 3 Nov 2001
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

#define MAX_TOKEN_BUFFER 255

class Scanner {
private:
	char* token;

	enum char_codes { 
		letters, 
		special, 
		whitespace, 
		openbrace, 
		closebrace, 
		openbracket, 
		closebracket, 
		openparen,
		closeparen
	};
	
	char_codes char_class[128];

	FILE *in;
	
	long backup_pos;
	
	char character;
	
public:
	Scanner(char *filename);
	~Scanner();
	void getNextToken(bool spaces=false);
	char* currentToken() const { return token; }
	bool matchNextToken(char* t);
	bool match(char* t);
	void rewind() { if (in==NULL) return; ::rewind(in); }
	bool eof();
	void clearToken() { strcpy (token, ""); }
	void savePosition() { 	backup_pos = ftell(in); }
	void putBackToken() { fseek(in, backup_pos, SEEK_SET); }
	void concat();
};

#endif
