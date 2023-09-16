#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <string.h>

#include <stack>
#include <fstream>

#include <sstream>

using namespace std ; 


class SIC {

public:
	
	struct DATA {
		int line = 0 ; // 原本在哪個行數 
		char loc[10] ;  // 指令位置
		char lebal[100] ; 
		char opcode[20] ;
		char oprand[20] ;
		char objCode[20] ;
		bool done = false ;
	};

	struct Instruction_Set {
		char name[60] ;
		char opcode[3] ;
		int format ;
	};
	
	const struct Instruction_Set Opcode_Table[ 26 ] = {  // SIC
		{ "ADD", "18", 3 },		{ "AND", "40", 3 },	{ "COMP", "28", 3 },	{ "DIV", "24", 3 },
		{ "J", "3C", 3 },		{ "JEQ", "30", 3 },	{ "JGT", "34", 3 },		{ "JLT", "38", 3 },
		{ "JSUB", "48", 3 },	{ "LDA", "00", 3 },	{ "LDCH", "50", 3 },	{ "LDL", "08", 3 },
		{ "LDX", "04", 3 },		{ "MUL", "20", 3 },	{ "OR", "44", 3 },		{ "RD", "D8", 3 },
		{ "RSUB", "4C", 3 },	{ "STA", "0C", 3 },	{ "STCH", "54", 3 },	{ "STL","14", 3 },
		{ "STSW", "E8", 3 },	{ "STX", "10", 3 },	{ "SUB", "1C", 3 },		{ "TD", "E0", 3 },
		{ "TIX", "2C", 3 },		{ "WD", "DC", 3 }	
	};
	
	struct Symbol_Table {
		char name[60] ;
		char location[10] ;
	};
	
	struct Integer_Table {
		char name[60] ;
	};
	
	struct String_Table {
		char name[60] ;
	};
	
	FILE * inFile ;
	vector<DATA> output ;
	DATA node ; 
	
	int hashSize = 100 ;
	Symbol_Table * symbolTable = new Symbol_Table[hashSize]() ;    // Table5
	Integer_Table * integerTable = new Integer_Table[hashSize]() ; // Table6
	String_Table * stringTable = new String_Table[hashSize]() ;    // Table7
	
	int Line = 5 ;
	char Loc[10] ;
	char ch[1] = "" ; 
	
	bool pass1 = true, pass2 = false ;

	void readFile_and_Pass1() {  // =========================== 讀檔 
		output.clear() ;
		
		string fileName = "SIC_input.txt" ;            // 打開SIC測試檔 
        inFile = fopen( fileName.c_str(), "r" ) ;
        
        if ( inFile == nullptr ) {
        	string fileName = "SICXE_input.txt" ;      // 打開SIC/XE測試檔 
        	inFile = fopen( fileName.c_str(), "r" ) ;
        	if ( inFile == nullptr ) {
        		cout << endl << "###" << fileName << " does not exist! ###" << endl; // 都打不開 
        	} // end if
        } // end if
        
        char temp[60] = "" ;
        int count = 0 ;
        memset( temp, 0, 60 ) ;
        memset( Loc, 0, 10 ) ;
        memset( &node,0,sizeof(DATA) ) ;
        
        while ( ( ( ch[0] = getc( inFile ) ) != '\n') || strlen(temp) == 0 ) { // 處理初始位置(START) 
        	if ( ch[0] == '\t' ) {
        		if ( strcmp( temp, "START" ) == 0 ) 
        			strcat( node.opcode, temp ) ;
        		else 
        			strcat( node.lebal, temp ) ;
				memset( temp, 0, 60 ) ;
				count = 0 ;
			} // if
			else if ( ch[0] != '\n' ) {
				temp[count] = ch[0] ;
				count++ ;
			} // else
		} // while
		
		strcat( Loc, temp ) ;
		strcat( node.loc, temp ) ;
		strcat( node.oprand, temp ) ;
		
		node.line = Line ;							
		output.push_back( node ) ;
		
		memset( &node,0,sizeof(DATA) ) ;  // 清空node 
		memset( temp, 0, 60 ) ;		      // 清空temp 
		count = 0 ;
        //int tabnum = 0 ;
        
        while ( ( ch[0] = getc( inFile ) ) != EOF ) {  // 切token 
    		if ( ch[0] == '\n' || ch[0] == '\t' || ( checkDelimiter( ch[0] ) ) || ch[0] == ' ' ) {
    			if ( checkDelimiter( ch[0] ) ) {       // 先判斷有delimiter的情況 
    				if ( ch[0] == '\'' ) {
    					char str[60] ;
        				memset( str, 0, 60 ) ;
					
        				if ( strlen(temp) != 0 ) strcat( node.oprand, temp ) ;
        					strcat( node.oprand, "\'" ) ; 
        				
        				for ( int i = 0 ; ( ch[0] = getc( inFile ) ) != '\'' ; i++ ) // 讀 '字串' 
        					str[i] = ch[0] ;
        					
						strcat( node.oprand, str ) ;
						strcat( node.oprand, "\'" ) ;
							
						if ( node.oprand[0] == 'X' ) strcat( node.objCode, str ) ;
						
						else if ( node.oprand[0] == 'C' ) {
							char temp[6] = "" ;
							for ( int i = 0 ; i < strlen( str ) ; i++ ) {
								int a = (int)str[i] ;
								char aa[5] ;
								itoa ( a, aa, 16 ) ;       // dec to hex
								strcat( temp, aa ) ;
							} // for
							
							for( int i = 0 ; i < strlen(temp) ; i++ )  // 轉換成大寫
								temp[i] = toupper( temp[i] ) ;
								
							int i ;
							for ( i = 0 ; i < 6-strlen(temp) ; i++ ) 
								node.objCode[i] = '0' ;
							
							strcat( node.objCode, temp ) ;
							node.done = true ;
						} // else if
						
						hashing( str, 7 ) ;
						writeTableFile( "Table7", 7 ) ;
    				} // if
    				else if ( ch[0] == '.' ) {
    					char trash[100] = "" ;
    					fscanf( inFile, "%[^\n]", trash ) ;
    					char c[0] ;
						c[0] = getc( inFile ) ;   // 讀掉換行 
						
    					Line += 5 ;
    					node.line = Line ; 
    					strcat( node.lebal, "." ) ;
    					strcat( node.lebal, trash ) ;
    					node.done = true ;
						output.push_back( node ) ;
						memset( &node,0,sizeof(DATA) ) ;   // 清空node
					} // else if
					
    				else {
    					if ( strlen(temp) != 0 ) {
							judgeTable( temp ) ;  
						} // if 
						
        				int len = strlen( node.oprand ) ;
        				node.oprand[len] = ch[0] ;
        				node.oprand[len+1] = '\0' ;
					} // else
				} // if
				
    			else if ( temp[0] != '\0' ) {          // 表示裡面有東西 
    				//if ( ch[0] == '\t' ) tabnum++ ;
    				judgeTable( temp ) ;               // 將內容判斷在哪個table，並且檢查格式 
				} // else if 
				
				if ( ch[0] == '\n' ) {                 // 加上行數和指令位置 
    				Line += 5 ;
    				node.line = Line ; 
    				strcat( node.loc, Loc ) ;
    				if ( strcmp( node.opcode, "BYTE" ) == 0 && node.oprand[0] == 'X' )
    					addHex( "1" ) ; // Loc + 1
					else if ( strcmp( node.opcode, "RESB" ) != 0 ) 
						addHex( "3" ) ; // Loc + 3
				
					convert_to_Mcode() ;
					output.push_back( node ) ;
					memset( &node,0,sizeof(DATA) ) ;   // 清空node 
				} // if
				
    			count = 0 ;
        		memset( temp, 0, 60 ) ;                // 清空temp 
			} // else if
			else {
				temp[count] = ch[0] ;
				count++ ;
			} // else
        } // while
        
        /*
        if ( tabnum != 2 ) {
        	cout << tabnum << endl ;
        	DATA temp ;
        	temp = output[output.size()-1] ;
        	strcpy( temp.oprand, temp.lebal ) ;
        	strcpy( temp.lebal, "" ) ;
        	output[output.size()-1] = temp ;
		} // if
        */
        fclose( inFile ) ;
	} // readFIle()

	void Pass2() {  // ======================================== Pass2 
		for ( int i = 0 ; i < output.size() ; i++ ) {
			if ( output[i].done == false ) {  // pass1尚未翻譯完成 
				
				if ( checkTable( output[i].oprand, "5" ) ) {   // 如果pass2在symbol table找到該lebal 
					for ( int j = 0 ; j < 100 ; j++ ) { 
		 				if ( strcmp( symbolTable[j].name, output[i].oprand ) == 0 ) {
		 					strcat( output[i].objCode, symbolTable[j].location ) ;
		 					output[i].done = true ;
		 					break ;
						 } // if
					} // for 
				} // if
				else {
					bool findX = false ;
					char str[5] ;
					for ( int j = 0 ; j < strlen( output[i].oprand ) ; j++ ) 
						if ( output[i].oprand[j] == 'X' ) findX = true ;
						
					if ( findX ) {
						for ( int j = 0 ; j < strlen( output[i].oprand ) ; j++ ) {
							if ( output[i].oprand[j] == ',' ) break ;
							str[j] = output[i].oprand[j] ;
						} // for
						
						for ( int j = 0 ; j < 100 ; j++ ) { 
		 					if ( strcmp( symbolTable[j].name, str ) == 0 ) {
		 						strcat( output[i].objCode, symbolTable[j].location ) ;
		 						output[i].done = true ;
		 						break ;
							 } // if
						} // for 
						
						char saveLoc[5] = "" ;
						strcpy( saveLoc, Loc ) ;
						strcpy( Loc, output[i].objCode ) ;
						addHex( "8000" ) ;
						strcpy( output[i].objCode, Loc ) ;
						strcpy( Loc, saveLoc ) ;
					} // if
					else 
						strcpy( output[i].objCode, "Error: Undefined Symbol!" ) ;
				} // else
			} // if
		} // for	
	} // pass2()

	void convert_to_Mcode() {  // ============================= 轉成機器碼 
		int i ;
		
		if ( strlen( node.objCode ) != 0 ) return ;  // 如果objCode裡面有東西，直接return 
		
		if ( strcmp( node.opcode, "WORD" ) == 0 ) {  // 處理 WORD指令 
			int i ;
			char hexNum[5] ;
			int num = 0 , sum = 0 ;
			for ( int j = 0 ; j < strlen( node.oprand ) ; j++ ) {
				num = (int)node.oprand[j] - '0' ;
				sum = sum * 10 + num ;
			} // for
			
			itoa ( sum, hexNum, 16 ) ;                // dec to hex
			
			for ( i = 0 ; i < 6-strlen(hexNum) ; i++ ) node.objCode[i] = '0' ;
			strcat( node.objCode, hexNum ) ;
			node.done = true ;
		} // if	
		
		else if ( strcmp( node.opcode, "RESB" ) == 0 ) {
			char hexNum[5] ;
			int num = 0 , sum = 0 ;
			for ( int j = 0 ; j < strlen( node.oprand ) ; j++ ) {
				num = (int)node.oprand[j] - '0' ;
				sum = sum * 10 + num ;
			} // for
			
			itoa ( sum, hexNum, 16 ) ;                // dec to hex
			addHex( hexNum ) ;
			node.done = true ;
		} // else if
		else if ( strcmp( node.opcode, "RESW" ) == 0  ) {
			
			node.done = true ;
		} // else if
		
		else if ( strcmp( node.opcode, "RSUB" ) == 0 ) {   // 處理 RSUB指令 
			strcat( node.objCode, "4C0000" ) ;
			node.done = true ;
		} // else if
			
		else {
			for ( i = 0 ; i < 26 ; i++ ) 
				if ( strcmp( Opcode_Table[i].name, node.opcode ) == 0  ) 
					break ;
		
			strcat( node.objCode, Opcode_Table[i].opcode ) ;
			if ( checkTable( node.oprand, "5" ) ) {   // 如果pass1在symbol table找到該lebal 
				for ( int i = 0 ; i < 100 ; i++ ) { 
		 			if ( strcmp( symbolTable[i].name, node.oprand ) == 0 ) {
		 				strcat( node.objCode, symbolTable[i].location ) ;
		 				node.done = true ;
		 				break ;
					 } // if
				} // for 
			} // if
		} // else 
		
	} // convert_to_Mcode()
	
	void judgeTable( char temp[60] ) {  // ==================== 判斷在哪個table
		if ( checkTable( temp, "3" ) ) {         // register
			cout << temp << "	register" << endl ;
      		strcat( node.oprand, temp ) ;	
		} // if
		
		else if ( checkTable( temp , "1" ) ) {   // instruction
			cout << temp << "	instrution" << endl ;
			strcat( node.opcode, temp ) ;
												
		} // else if
		
		else if ( checkTable( temp, "2" ) ) {    // pseudo
			cout << temp << "	pseudo" << endl ;
			strcat( node.opcode, temp ) ;
			
		} // else if
		else {      
			bool num = true ;
			for ( int i = 0 ; i < strlen(temp) ; i++ ) 
				if ( temp[i] > '9' || temp[i] < '0' ) num = false ; 
								
			if ( num ) {                         // 存進table6(Integer) 
				hashing( temp, 6 ) ;
				writeTableFile( "Table6", 6 ) ;
			} // if
			else {                               // 存進table5(symbol) 
				hashing( temp, 5 ) ;
				writeTableFile( "Table5", 5 ) ;	
			} // else
			
		} // else
	} // judgeTable()
	
	void hashing( char str[60], int cases ) {  // ============= 建hashing table 
		int sum = 0 ;
		
		for( int i = 0 ; i < strlen(str) ; i++ ) 
			sum += (int)str[i] ;
					
		int * key = new int[hashSize]() ;
		for ( ; sum >= hashSize ; sum %= hashSize ) ;
			
		if ( cases == 5 ) {  // ------------------------------------- symbol
			if ( symbolTable[sum].name[0] == NULL ) {                      // 找到空位
			
				if( ch[0] == '\t' ) {        // 為定義label的位置 (第一行)
					strcat( symbolTable[sum].name, str ) ;
					strcat( symbolTable[sum].location, Loc ) ;
				} // if
				else if ( ch[0] == '\n' ) {  // 為要找尋label的位置 (第三行) 
					if ( pass1 ) {
					
					
					} // if
					else if ( pass2 )
						cout << str << " ERROR: Undefinite Symbol!" << endl ;   // 要再且pass2的時候!! 
				} // else if
			} // if
			else {
				bool same = false ;
				for ( ; symbolTable[sum].name[0] != NULL ; sum++ )
					if ( strcmp( symbolTable[sum].name, str ) == 0 ) same = true ;
				
				if ( same && ch[0] == '\t' )                                    // 重複定義(error)
					cout << str <<  " ERROR: Duplicate Definition!" << endl ;
				else if ( same && ch[0] == '\n' ) {					// pass1 / pass2 找尋symbol 
			
				
				} // else if  
				else if ( !same ) {
					for ( ; symbolTable[sum].name[0] != NULL ; sum++ ) ;       // 該格有data，往下找空位 
	
					strcat( symbolTable[sum].name, str ) ;
					strcat( symbolTable[sum].location, Loc ) ;
				} // else
			} // else
		} // if
		
		else if ( cases == 6 ) {  // -------------------------------- integer
			if ( integerTable[sum].name[0] == NULL ) {                      // 找到空位
				strcat( integerTable[sum].name, str ) ;
			} // if
			else {
				bool same = true ;
				for ( int i = 0 ; i < strlen(str) ; i++ )      // 檢查是否相同 
					if( integerTable[sum].name[i] != str[i] ) same = false ;
			
				if ( !same ) {                                   // 重複定義(error)
					for ( ; integerTable[sum].name[0] != NULL ; sum++ ) ;       // 該格有data，往下找空位 
					strcat( integerTable[sum].name, str ) ;
				} // if
				
			} // else
		} // else if
	
		else if( cases == 7 ) {  // --------------------------------- string
			if ( stringTable[sum].name[0] == NULL )             // 找到空位
				strcat( stringTable[sum].name, str ) ;          // 存入string table 
				
			else {
				bool same = true ;
				for ( int i = 0 ; i < strlen(str) ; i++ )      // 檢查是否相同 
					if( stringTable[sum].name[i] != str[i] ) same = false ;
				
				if ( !same ) {                                   // 重複定義(error)
					for ( ; stringTable[sum].name[0] != NULL ; sum++ ) ;       // 該格有data，往下找空位 
				
					strcat( stringTable[sum].name, str ) ;    // 存入string table 
				} // if
			} // else
		} // else if
		
		// if ( ( ch[0] = getc( inFile ) ) != '\n' ) ungetc( ch[0], inFile ) ;
		
		if ( ch[0] == '\n' || ( checkDelimiter( ch[0] ) && ch[0] != '\'' ) || ch[0] == EOF ) 
			strcat( node.oprand, str ) ;
		else if ( ch[0] == '\t' && strlen( node.oprand ) == 0 )
			strcat( node.lebal, str ) ;
	
	} // hashing()

    bool checkTable( char key[60], string num ) {  // ========= 檢查是否為instructuin/pseudo/reg
    	char tmp[50] ;
    	
    	string fileName ;
		fileName = "Table" + num + ".table" ;            // 檢查table1(instruction)
    	FILE * inFile = fopen( fileName.c_str(), "rb" ) ;    	
    	while ( fgets( tmp, 50, inFile ) ) {
    		tmp[ strlen(tmp) ] = '\n' ;
    		for( int i = 0 ; i < strlen(tmp) ; i++ )  // 轉換成大寫 
    		    tmp[i] = toupper( tmp[i] ) ;
		
    		if ( strstr( tmp, key ) != NULL ) {       // tabal1裡是否包含
    			fclose(inFile) ;
    			return true ;
			} // if
		} // while
		
		fclose(inFile) ;
		return false ;
	} // check()
	
	void writeFile() {  // ==================================== 寫output檔
		DATA data;
    	int data_size;
    	data_size = sizeof(struct DATA);
	
    	FILE *fp_output ;
  
		string temp = "SIC_output.txt" ;
    	fp_output = fopen( temp.c_str() , "w+");
  
    	if (fp_output == NULL) {
        	cout << "Could not open " << "output file." << endl;
        	// return false;
   		} // if
  
  		char tempStr[] = "Line	Loc	Source statement		Object code" ;
  		fprintf( fp_output,"%s \n\n", tempStr ) ; 

 		for ( int i = 0 ; i < output.size() ; i++ ) {
		 	fprintf( fp_output,"%3d	%s	%s	%s	%s		%s\n",  
			 		 output[i].line, output[i].loc, output[i].lebal, output[i].opcode, output[i].oprand, output[i].objCode ); // write data as required

		} // for

    	fclose(fp_output) ;
	} // writeFile()
	
	void writeTableFile( string name, int cases ) {  // ======= 寫Table檔 
		DATA data;
    	int data_size;
    	data_size = sizeof(struct DATA);
	
    	FILE *fp_output ;
  
		string temp = name + ".table" ;
    	fp_output = fopen( temp.c_str() , "w+");
  
    	if (fp_output == NULL) {
        	cout << "Could not open " << "output file." << endl;
        } // if


		if ( cases == 5 )
 			for ( int i = 0 ; i < 100 ; i++ ) 
		 		fprintf( fp_output,"[%3d] %s	%s\n", i+1, symbolTable[i].name, symbolTable[i].location ) ;
		 		
		else if ( cases == 6 )
 			for ( int i = 0 ; i < 100 ; i++ ) 
		 		fprintf( fp_output,"[%3d] %s\n", i+1, integerTable[i].name ) ;

		else if ( cases == 7 )
 			for ( int i = 0 ; i < 100 ; i++ ) 
		 		fprintf( fp_output,"[%3d] %s\n", i+1, stringTable[i].name ) ; // write data as required


    	fclose(fp_output) ;

	} // writeFile()
	
	bool checkDelimiter( char c ) {  // ======================= 檢查是否為delimiter
		char tmp[50] ;
		
		string fileName = "Table4.table" ;            // 檢查table4(delimiter)
    	FILE * inFile = fopen( fileName.c_str(), "rb" ) ;    	
    	while ( fgets( tmp, 50, inFile ) ) {
    		tmp[ strlen(tmp) - 1 ] = '\0' ;
    		
    		if ( strchr( tmp, c ) != NULL ) {         // tabal4裡是否包含 
    			fclose(inFile) ;
    			return true ;
			} // if
		} // while
	
    	fclose(inFile) ;
		return false ;
	} // checkDelimiter()	
	
	void addHex( char num[5] ) {  // ======================================= Loc + 3 (16進制)
		char arr[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' } ;
		
		bool carry = false, first = true ; 
		int summand = 0, addend = 0, temp = 0 ;
		int count = strlen( num ) ;
		
		for ( int i = strlen(Loc)-1 ; i >= 0 ; i-- ) {
			for ( summand = 0 ; summand < strlen(arr) ; summand++ ) 
				if ( arr[summand] == Loc[i] ) break ;
				
			if ( count > 0 ) {
				for ( addend = 0 ; addend < strlen(arr) ; addend++ )  
					if ( arr[addend] == num[count-1] ) break ;
					
				if ( arr[addend] >= '0' && arr[addend] <= '9' ) 
					temp = num[count-1] - '0' ;
					
				else if ( arr[addend] >= 'A' && arr[addend] <= 'F' )
					temp = num[count-1] - 'A' + 10 ;
					
				count-- ;
			} // if
			else temp = 0 ;
				
			
			if ( carry ) summand = summand + temp + 1 ;
			else summand = summand + temp ;
			
			if ( summand > 15 ) {
				carry = true ;
				int idx = summand - 16 ;
				Loc[i] = arr[idx] ;
				
			} // if
			else {
				
				carry = false ;
				Loc[i] = arr[summand] ;
				
			} // else
		} // for
	} // addHex()
	
};

int main() {
	
	SIC sic ;
	sic.readFile_and_Pass1() ;
	sic.Pass2() ;
	sic.writeFile() ;
	/*
	SICXE sixce ; 
	sixce.XE_readFile_and_Pass1() ;
	sixce.Pass2() ;
	sixce.writeFile() ;
	*/
} // end main
