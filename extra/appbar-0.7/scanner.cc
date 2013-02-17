/*
 * scanner.cc - processes text files and breaks there data into tokens.
 * Copyright (C) 2000 Frank Hale
 * frankhale@yahoo.com
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

#include "scanner.hh"

Scanner::Scanner(char* filename)
{
	inputFile.open(filename);
	
	if (! inputFile.is_open())
	{ 
		cout << "Error opening input file: " << filename << endl; 
		exit (1); 
	} 

	backup_pos = inputFile.tellg();

	tokensRelaxed=false;

	character='\0';

	defaultTokens();

	s_token = "";
}

Scanner::~Scanner() 
{ 
	inputFile.close();
}

void Scanner::defaultTokens()
{
	int i;
	
	tokensRelaxed=false;

	for(i='a'; i<='z'; ++i) char_class[i]=letters;
	for(i='A'; i<='Z'; ++i) char_class[i]=letters;
	for(i='0'; i<='9'; ++i) char_class[i]=letters;

	char_class['`']=letters;
	char_class['!']=letters;
	char_class['@']=letters;
	char_class['#']=letters;
	char_class['$']=letters;
	char_class['%']=letters;
	char_class['^']=letters;
	char_class['&']=letters;
	char_class['_']=letters;
	char_class['+']=letters;
	char_class['=']=equals;
	char_class['\\']=letters;
	char_class['?']=letters;
	char_class[':']=letters;
	char_class[';']=semicolon;
	char_class[',']=letters;
	char_class['|']=letters;
	char_class['\"']=quote;
	char_class['~']=letters;
	char_class['/']=letters;
	char_class['-']=letters;
	char_class['.']=letters;
	char_class['<']=letters;
	char_class['>']=letters;
	
	// These characters are used in configuration file constructs
	char_class['{']=openbrace;
	char_class['}']=closebrace;
	char_class['[']=openbracket;
	char_class[']']=closebracket;
	char_class['(']=openparen;
	char_class[')']=closeparen;
	
	char_class['*']=asterix;

	char_class[' ']=whitespace;
	char_class['\0']=whitespace;
	char_class['\n']=whitespace;
	char_class['\r']=whitespace;
	char_class['\t']=whitespace;

	char_class[EOF]=whitespace;
}

void Scanner::relaxedTokens()
{
	int i;
	
	tokensRelaxed=true;

	for(i='a'; i<='z'; ++i) char_class[i]=letters;
	for(i='A'; i<='Z'; ++i) char_class[i]=letters;
	for(i='0'; i<='9'; ++i) char_class[i]=letters;

	char_class['`']=letters;
	char_class['!']=letters;
	char_class['@']=letters;
	char_class['#']=letters;
	char_class['$']=letters;
	char_class['%']=letters;
	char_class['^']=letters;
	char_class['&']=letters;
	char_class['_']=letters;
	char_class['+']=letters;
	char_class['=']=letters;
	char_class['\\']=letters;
	char_class['?']=letters;
	char_class[':']=letters;
	char_class[';']=semicolon;
	char_class[',']=letters;
	char_class['|']=letters;
	char_class['\"']=letters;
	char_class['~']=letters;
	char_class['/']=letters;
	char_class['-']=letters;
	char_class['.']=letters;
	char_class['<']=letters;
	char_class['>']=letters;
	char_class['{']=letters;
	char_class['}']=letters;
	char_class['[']=letters;
	char_class[']']=letters;
	char_class['(']=letters;
	char_class[')']=letters;
	char_class['*']=letters;
	char_class[' ']=letters;
	char_class['\0']=letters;
	char_class['\n']=letters;
	char_class['\r']=letters;
	char_class['\t']=letters;
}

void Scanner::concat(int type)
{
	s_token.empty();	

	while(char_class[character]==type)
	{
		s_token += character;

		//cout << "concat: " << s_token << endl;

		if( inputFile.eof() ) return;
		
		inputFile.get(character);

	}
	
}

void Scanner::getNextToken(bool spaces) 
{ 
	if(! tokensRelaxed)
	{
		if (!spaces) {
			char_class[' ']=whitespace;	

			while(char_class[character]==whitespace)
			{
				if( inputFile.eof() ) return;
		
				inputFile.get(character);
			}
			
			s_token="";	

		} else {
			// I don't think this is working
			char_class[' ']=letters;
		}
	}

	if (! inputFile.eof() ) { concat( char_class[character] ); }
}

bool Scanner::match( std::string m )
{
	if ( s_token == m )
	{
		getNextToken();
		return true;
	} else
		return false;
}


bool Scanner::eof() 
{ 
	return ( inputFile.eof() ? true : false );
}
