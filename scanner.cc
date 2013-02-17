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


#include "sapphire.hh"

Scanner::Scanner(char* filename)
{
	if((in = fopen(filename, "r"))==NULL)
	{
		printf("can't open file: %s\n", filename);
		
		wm->getRootMenu()->defaultMenu();
	} else
		backup_pos = ftell(in);
	
	int i;
	
	for(i='a'; i<='z'; ++i) char_class[i]=letters;
	for(i='A'; i<='Z'; ++i) char_class[i]=letters;
	for(i='0'; i<='9'; ++i) char_class[i]=letters;

	char_class['`']=letters;
	char_class['\'']=letters;
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
	char_class[';']=letters;
	char_class[',']=letters;
	char_class['|']=letters;
	char_class['\"']=letters;
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
	
	char_class['*']=letters;

	char_class[' ']=whitespace;
	char_class['\0']=whitespace;
	char_class['\n']=whitespace;
	char_class['\r']=whitespace;
	char_class['\t']=whitespace;
	char_class[EOF]=whitespace;
	
	token = new char[MAX_TOKEN_BUFFER];
	
	character='\0';
}

Scanner::~Scanner() 
{ 
	if (in!=NULL) fclose(in); 
	delete [] token; 
}

void Scanner::concat()
{
	int i=strlen(token);
	
	if(i == MAX_TOKEN_BUFFER) {
		token = new char[i];
	}
	
	token[i]=character;
	token[i+1]='\0';
}

void Scanner::getNextToken(bool spaces) 
{ 
	if (in==NULL) return;
	if (!spaces) {
		char_class[' ']=whitespace;	
		strcpy (token, "");
	} else {
		char_class[' ']=letters;
	}
		
	bool backslash=false;
	switch(char_class[character])
	{
		case letters:
			while((char_class[character]==letters) || backslash)
			{
				concat();
				fscanf(in, "%c", &character);				
				if (character=='\\') {
					fscanf(in, "%c", &character);
					backslash=true;
				} else
					backslash=false;
			}
			
			return;
		break;
		
		case special:
			while(char_class[character]==special)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;

		case openparen:
			while(char_class[character]==openparen)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;

		case closeparen:
			while(char_class[character]==closeparen)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;


		case openbrace:
			while(char_class[character]==openbrace)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;

		case closebrace:
			while(char_class[character]==closebrace)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;

		case openbracket:
			while(char_class[character]==openbracket)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;

		case closebracket:
			while(char_class[character]==closebracket)
			{
				concat();
				fscanf(in, "%c", &character);
			}
			return;		
		break;
		
		case whitespace:
			while(char_class[character]==whitespace)
			{
				if(spaces) {  concat(); }
				
				fscanf(in, "%c", &character);
				
				if(feof(in)) return;
			}
			getNextToken();
		break;		
	}
}

bool Scanner::match(char* t)
{
	return ( (strcmp(t, token)==0) ? true : false );
}

bool Scanner::matchNextToken(char* t)
{
	if(match(t)) return true;
	else 
		putBackToken();

	return false;
}

bool Scanner::eof() 
{ 
	if ( in==NULL ) return true;
	return ( feof(in) ? true : false );
}
