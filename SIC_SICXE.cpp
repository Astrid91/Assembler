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
        	string fileName = "SICXE_input" ;      // 打開SIC/XE測試檔 
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
			} // if
			else {                               // 存進table5(symbol) 
				hashing( temp, 5 ) ;
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


class SICXE {
	
public:	
	struct DATA {
		int line = 0 ; // 原本在哪個行數 
		char loc[10] = "" ;  // 指令位置
		char lebal[100] = "" ; 
		char opcode[20] = "" ;
		char oprand1[20] = "" ;
		char oprand2[20] = "" ;
		char objCode[20] = "" ;
		bool done = false ;
	};

	struct Instruction_Set {
		char name[60] ;
		char opcode[3] ;
		int format ;
	};
	
	const struct Instruction_Set Opcode_Table[ 58 ] = {  // SIC
		{ "ADD", "18", 3 },		{ "AND", "40", 3 },		{ "COMP", "28", 3 },	{ "DIV", "24", 3 },
		{ "J", "3C", 3 },		{ "JEQ", "30", 3 },		{ "JGT", "34", 3 },		{ "JLT", "38", 3 },
		{ "JSUB", "48", 3 },	{ "LDA", "00", 3 },		{ "LDCH", "50", 3 },	{ "LDL", "08", 3 },
		{ "LDX", "04", 3 },		{ "MUL", "20", 3 },		{ "OR", "44", 3 },		{ "RD", "D8", 3 },
		{ "RSUB", "4C", 3 },	{ "STA", "0C", 3 },		{ "STCH", "54", 3 },	{ "STL","14", 3 },
		{ "STSW", "E8", 3 },	{ "STX", "10", 3 },		{ "SUB", "1C", 3 },		{ "TD", "E0", 3 },
		{ "TIX", "2C", 3 },		{ "WD", "DC", 3 },		{ "ADDR", "90", 3 },	{ "CLEAR", "B4", 3 },
		{ "COMPR", "A0", 3 },	{ "DIVR", "9C", 3 },	{ "FIX", "C4", 3 },		{ "FLOAT", "C0", 3 },
		{ "HIO", "F4", 3 },		{ "MULR", "98", 3 },	{ "NORM", "C8", 3 },	{ "RMO", "AC", 3 },
		{ "SHIFTL", "A4", 3 },	{ "SHIFTR", "A8", 3 },	{ "SIO", "F0", 3 },		{ "SUBR", "94", 3 },
		{ "SVC", "B0", 3 },		{ "TIO", "F8", 3 },		{ "TIXR", "B8", 3 },	{ "COMPF", "88", 3 },
		{ "ADDF", "58", 3 }, 	{ "DIVF", "64", 3 },	{ "LDB", "68", 3 },		{ "LDF", "70", 3 },
		{ "LDS", "6C", 3 },		{ "LPS", "D0", 3 },		{ "LDT", "74", 3 },		{ "MULF", "60", 3 },
		{ "SSK", "EC", 3 },		{ "STB", "78", 3 },		{ "STF", "80", 3 },		{ "STS", "7C", 3 },
		{ "STT", "84", 3 },		{ "SUBF", "5C", 3 }
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
	
	bool pass1Error = false, pass2Error = false ;
	bool format1 = false, format2 = false ;
	bool format3 = false, format4 = false ;

	void XE_readFile_and_Pass1() {  // =========================== 讀檔 
		output.clear() ;
		
		string fileName = "SICXE_input.txt" ;            // 打開SIC測試檔 
        inFile = fopen( fileName.c_str(), "r" ) ;
        	
       	if ( inFile == nullptr ) {
        	cout << endl << "###" << fileName << " does not exist! ###" << endl; // 都打不開 
        } // end if
        
        char temp[60] = "" ;
        int count = 0 ;
        memset( temp, 0, 60 ) ;
        memset( Loc, 0, 10 ) ;
        memset( &node,0,sizeof(DATA) ) ;
        
        while ( ( ( ch[0] = getc( inFile ) ) != '\n') || strlen( temp ) == 0 ) { // 處理初始位置(START) 
        	if ( ch[0] == '\t' || ( ch[0] == ' ' && strlen( temp ) != 0 ) ) {
        		if ( strcmp( temp, "START" ) == 0 ) 
        			strcpy( node.opcode, temp ) ;
        		else 
        			strcpy( node.lebal, temp ) ;
				memset( temp, 0, 60 ) ;
				count = 0 ;
			} // if
			else if ( ch[0] != '\n' && ch[0] != ' ' ) {
				temp[count] = ch[0] ;
				count++ ;
			} // else
		} // while
		
		if ( temp[0] == '0' ) {
			strcpy( Loc, "0000" ) ;
			strcpy( node.loc, "0000" ) ;
			strcpy( node.oprand1, temp ) ;
		} // if
		else {
			strcpy( Loc, temp ) ;
			strcpy( node.loc, temp ) ;
			strcpy( node.oprand1, temp ) ;
		} // else
		
		node.line = Line ;							
		output.push_back( node ) ;
		
		memset( &node,0,sizeof(DATA) ) ;  // 清空node 
		memset( temp, 0, 60 ) ;		      // 清空temp 
		count = 0 ;
        
        while ( ( ch[0] = getc( inFile ) ) != EOF ) {  // 切token 
    		if ( ch[0] == '\n' || ch[0] == '\t' || ch[0] == ' ' || ( checkDelimiter( ch[0] ) ) ) {
    			if ( ch[0] != ' ' && checkDelimiter( ch[0] ) ) {       // 先判斷有delimiter的情況 
    				if ( ch[0] == '\'' ) {
    					char str[60] ;
        				memset( str, 0, 60 ) ;
					
        				if ( strlen(temp) != 0 ) strcat( node.oprand1, temp ) ;
        					strcat( node.oprand1, "\'" ) ; 
        				
        				for ( int i = 0 ; ( ch[0] = getc( inFile ) ) != '\'' ; i++ ) // 讀 '字串' 
        					str[i] = ch[0] ;
        					
						strcat( node.oprand1, str ) ;
						strcat( node.oprand1, "\'" ) ;
							
						if ( node.oprand1[0] == 'X' ) strcat( node.objCode, str ) ;
						
						else if ( node.oprand1[0] == 'C' ) {
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
					
					else if ( ch[0] == '+' ) {
						
						format4 = true ;
						strcat( node.opcode, "+" ) ;
						
					} // else if
					else if ( ch[0] == '#' ) {
						
						format4 = true ;
						strcat( node.oprand1, "#" ) ;
						
					} // else if
					
    				else {
    					if ( strlen(temp) != 0 ) {
							judgeTable( temp ) ;  
						} // if 
						
        				int len = strlen( node.oprand1 ) ;
        				if ( ch[0] != ',' ) {
        					node.oprand1[len] = ch[0] ;
        					node.oprand1[len+1] = '\0' ;
						} // if
					} // else
				} // if
								
    			else if ( temp[0] != '\0' ) {          // 表示裡面有東西 
    				judgeTable( temp ) ;               // 將內容判斷在哪個table，並且檢查格式 
				} // else if 
				if ( !pass1Error ) {	
					if ( ch[0] == '\n' ) {                 // 加上行數和指令位置 
    					Line += 5 ;
    					node.line = Line ; 
    					strcat( node.loc, Loc ) ;
					
						if( checkFormat() ) {
							convert_to_Mcode() ;
							output.push_back( node ) ;
						} // if
						else {
							strcpy( node.lebal, "ERROR!" ) ;
							strcpy( node.opcode, "" ) ;
							strcpy( node.oprand1, "" ) ;
							strcpy( node.oprand2, "" ) ;
							node.done = true ;
							output.push_back( node ) ;
						} // else
					
						if ( strcmp( node.opcode, "BYTE" ) == 0 && node.oprand1[0] == 'X' )
    						addHex( "1" ) ; // Loc + 1
						else if ( strcmp( node.opcode, "RESB" ) != 0 && strcmp( node.opcode, "EQU" ) != 0 )  {
							if ( format1 ) addHex( "1" ) ;
							else if ( format2 ) addHex( "2" ) ;
							else if ( format3 ) addHex( "3" ) ;
							else if ( format4 ) addHex( "4" ) ;
						} // else if
					
						memset( &node,0,sizeof(DATA) ) ;   // 清空node 
						format1 = false ;
						format2 = false ;
						format3 = false ;
						format4 = false ;
					} // if	
				} // if
				else {  //  Duplicate Definition!
					Line += 5 ;
    				node.line = Line ; 
    				strcat( node.loc, Loc ) ;
					pass1Error = false ;
					strcpy( node.lebal, "ERROR: Duplicate Definition!" ) ;
					strcpy( node.opcode, "" ) ;
					strcpy( node.oprand1, "" ) ;
					strcpy( node.oprand2, "" ) ;
					node.done = true ;
					output.push_back( node ) ;
					
					for( ; ch[0] != '\n' ; ch[0] = getc( inFile ) ) ; // 讀掉剩下的 
					
					memset( &node,0,sizeof(DATA) ) ;   // 清空node 
					format1 = false ;
					format2 = false ;
					format3 = false ;
					format4 = false ;
				} 
				
				
    			count = 0 ;
        		memset( temp, 0, 60 ) ;                // 清空temp 
			} // else if
			else {
				temp[count] = ch[0] ;
				count++ ;
			} // else
			
			if ( strcmp( node.opcode, "END" ) == 0 ) {
				Line += 5 ;
    			node.line = Line ;
				strcpy( node.oprand1, "FIRST" ) ;
				node.done = true ;
				output.push_back( node ) ;
				break ;
			} // if
			
        } // while
        
        fclose( inFile ) ;
	} // readFIle()

	bool checkFormat() {
		bool lower = false ;
		if ( node.oprand1[0] >= 'a' && node.oprand1[0] <= 'z' ) lower = true ;
			for( int i = 0 ; i < strlen(node.opcode) ; i++ )  // 轉換成大寫 
    		    node.opcode[i] = toupper( node.opcode[i] ) ;
    		    
		if ( strcmp( node.opcode, "ADDR" ) == 0 || strcmp( node.opcode, "COMPR" ) == 0 || strcmp( node.opcode, "DIVR" ) == 0 ||
		     strcmp( node.opcode, "MULR" ) == 0 || strcmp( node.opcode, "RMO" ) == 0 || strcmp( node.opcode, "SHIFTL" ) == 0 ||
			 strcmp( node.opcode, "SHIFTR" ) == 0 || strcmp( node.opcode, "SUBR" ) == 0 ) {
			 
			if ( strlen( node.oprand2 ) == 0 ) return false ;
			else if ( !checkTable( node.oprand1, "3" ) ) return false ;
			else if ( strcmp( node.opcode, "SHIFTL" ) != 0 && strcmp( node.opcode, "SHIFTR" ) != 0 && 
			 		   !checkTable( node.oprand1, "3" ) ) return false ;
			 	
		} // if
		else if ( strcmp( node.opcode, "FIX" ) == 0 || strcmp( node.opcode, "FLOAT" ) == 0 || strcmp( node.opcode, "HIO" ) == 0 ||
		     	  strcmp( node.opcode, "NORM" ) == 0 || strcmp( node.opcode, "RSUB" ) == 0 || strcmp( node.opcode, "SIO" ) == 0 ||
				  strcmp( node.opcode, "TIO" ) == 0 ) {
				  	
			if ( strlen( node.oprand1 ) != 0 || strlen( node.oprand2 ) != 0 ) return false ;	  
				   	
		} // else if
		else {
			
			if ( strlen( node.oprand2 ) != 0 ) return false ;	
			else if ( checkTable( node.oprand1, "3" ) ) return false ;
		} // else	
		
		
		if ( lower )
			for( int i = 0 ; i < strlen(node.opcode) ; i++ )  // 轉換成小寫 
    		   	node.opcode[i] = tolower( node.opcode[i] ) ;
    		   	
    	return true ;
		
	} // checkFormat()

	void Pass2() {  // ======================================== Pass2 
		for ( int i = 0 ; i < output.size() ; i++ ) {
			if ( output[i].done == false ) {  // pass1尚未翻譯完成 
				
				if ( checkTable( output[i].oprand1, "5" ) ) {   // 如果pass2在symbol table找到該lebal 
					for ( int j = 0 ; j < 100 ; j++ ) { 
		 				if ( strcmp( symbolTable[j].name, output[i].oprand1 ) == 0 ) {
		 					strcat( output[i].objCode, symbolTable[j].location ) ;
		 					output[i].done = true ;
		 					break ;
						 } // if
					} // for 
				} // if
				else {
					strcpy( output[i].lebal, "Error: Undefined Symbol!" ) ;
					strcpy( output[i].opcode, "" ) ;
					strcpy( output[i].oprand1, "" ) ;
					strcpy( output[i].oprand2, "" ) ;
					strcpy( output[i].objCode, "" ) ;
					
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
			for ( int j = 0 ; j < strlen( node.oprand1 ) ; j++ ) {
				num = (int)node.oprand1[j] - '0' ;
				sum = sum * 10 + num ;
			} // for
			
			itoa ( sum, hexNum, 16 ) ;                // dec to hex
			
			for ( i = 0 ; i < 6-strlen(hexNum) ; i++ ) node.objCode[i] = '0' ;
			strcat( node.objCode, hexNum ) ;
			node.done = true ;
		} // if	
		
		else if ( strcmp( node.opcode, "RESB" ) == 0 ) {
			char hexNum[5] = "" ;
			int num = 0 , sum = 0 ;
			for ( int j = 0 ; j < strlen( node.oprand1 ) ; j++ ) {
				num = (int)node.oprand1[j] - '0' ;
				sum = sum * 10 + num ;
			} // for
			
			itoa ( sum, hexNum, 16 ) ;                // dec to hex
			addHex( hexNum ) ;
			node.done = true ;
		} // else if
		else if ( strcmp( node.opcode, "RESW" ) == 0 ) {
			
			format3 = true ;
			node.done = true ;
		} // else if
		else if ( strcmp( node.opcode, "RSUB" ) == 0 ) {   // 處理 RSUB指令 
			strcat( node.objCode, "4F20000" ) ;
			format3 = true ;
			node.done = true ;
		} // else if
		else if ( strcmp( node.opcode, "EQU" ) == 0 ) {
			int i ;
			char hexNum[5] ;
			int num = 0 , sum = 0 ;
			for ( int j = 0 ; j < strlen( node.oprand1 ) ; j++ ) {
				num = (int)node.oprand1[j] - '0' ;
				sum = sum * 10 + num ;
			} // for
			
			itoa ( sum, hexNum, 16 ) ;                // dec to hex
			for ( int i = 0 ; i < strlen( hexNum ) ; i++ )
				hexNum[i] = toupper( hexNum[i] ) ;
				
			int count = strlen( hexNum ) - 1 ;
			for ( int i = 3 ; i >= 0 ; i-- ) {
				if ( count >= 0 ) {
					node.loc[i] = hexNum[count] ;
					count-- ;	
				} // if 
				else node.loc[i] = '0' ;
			} // for
			
			strcpy( Loc, node.loc ) ;
			
			for ( int i = 0 ; i < 100 ; i++ )   // 更改symbol table裡的位址 
		 		if ( strcmp( symbolTable[i].name, node.lebal ) == 0 )  
		 			strcpy( symbolTable[i].location, Loc ) ;
		 			
			addHex( "3" ) ;
			node.done = true ;
		} // else if
		else {
			bool addsymbol = false ; 
			char temp[5] = "" ;
			if ( node.opcode[0] == '+' ) {
			 	addsymbol = true ; 
				for ( int i = 1 ; i < strlen(node.opcode) ; i++ )
					temp[i-1] = node.opcode[i] ;
					
				strcpy( node.opcode, temp ) ;
			} // if 
			
			bool lower = false ;
			if ( node.oprand1[0] >= 'a' && node.oprand1[0] <= 'z' ) lower = true ;
			for( int i = 0 ; i < strlen(node.opcode) ; i++ )  // 轉換成大寫 
    		    node.opcode[i] = toupper( node.opcode[i] ) ;
			
			for ( i = 0 ; i < 58 ; i++ ) 
				if ( strcmp( Opcode_Table[i].name, node.opcode ) == 0 ) {
					break ;
				}
					
			if ( lower )
				for( int i = 0 ; i < strlen(node.opcode) ; i++ )  // 轉換成小寫 
    		   		node.opcode[i] = tolower( node.opcode[i] ) ;
    		    
    		if ( addsymbol ) {
    			strcpy( node.opcode, "+" ) ;
    			strcat( node.opcode, temp ) ;
			} // if
		
			strcat( node.objCode, Opcode_Table[i].opcode ) ;
		
			if ( strlen( node.oprand1 ) == 0 ) {      // format1
				format1 = true ;
				node.done = true ;
			} // if
			else if ( strlen( node.oprand2 ) != 0 || 
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "CLEAR" ) == 0 ||
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "SVC" ) == 0 ||
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "TIXR" ) == 0 ||
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "clear" ) == 0 ||
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "svc" ) == 0 ||
					  strlen( node.oprand2 ) == 0 && strcmp( node.opcode, "tixr" ) == 0 ) {
				format2 = true ;
				bool lower = false ;
				if ( node.oprand1[0] >= 'a' && node.oprand1[0] <= 'z' ) lower = true ;
				node.oprand1[0] = toupper( node.oprand1[0] ) ;
				node.oprand2[0] = toupper( node.oprand2[0] ) ;
				if ( checkTable( node.oprand1, "3" ) && checkTable( node.oprand2, "3" ) || strlen( node.oprand2 ) == 0 ) { 
					if ( node.oprand1[0] == 'A' ) strcat( node.objCode, "0" ) ;
					else if ( node.oprand1[0] == 'X' ) strcat( node.objCode, "1" ) ;
					else if ( node.oprand1[0] == 'L' ) strcat( node.objCode, "2" ) ;
					else if ( node.oprand1[0] == 'B' ) strcat( node.objCode, "3" ) ;
					else if ( node.oprand1[0] == 'S' ) strcat( node.objCode, "4" ) ;
					else if ( node.oprand1[0] == 'T' ) strcat( node.objCode, "5" ) ;
					else if ( node.oprand1[0] == 'F' ) strcat( node.objCode, "6" ) ;
					else if ( strcmp(node.oprand1, "PC") == 0 ) strcat( node.objCode, "7" ) ;
					else if ( strcmp(node.oprand1, "SW") == 0 ) strcat( node.objCode, "8" ) ;
				
					if ( strlen( node.oprand2 ) != 0 ) {
						if ( node.oprand2[0] == 'A' ) strcat( node.objCode, "0" ) ;
						else if ( node.oprand2[0] == 'X' ) strcat( node.objCode, "1" ) ;
						else if ( node.oprand2[0] == 'L' ) strcat( node.objCode, "2" ) ;
						else if ( node.oprand2[0] == 'B' ) strcat( node.objCode, "3" ) ;
						else if ( node.oprand2[0] == 'S' ) strcat( node.objCode, "4" ) ;
						else if ( node.oprand2[0] == 'T' ) strcat( node.objCode, "5" ) ;
						else if ( node.oprand2[0] == 'F' ) strcat( node.objCode, "6" ) ;
						else if ( strcmp(node.oprand2, "PC") == 0 ) strcat( node.objCode, "7" ) ;
						else if ( strcmp(node.oprand2, "SW") == 0 ) strcat( node.objCode, "8" ) ;
					} // if
					else strcat( node.objCode, "0" ) ;
				} // if
				
				if ( lower ) {
					node.oprand1[0] = tolower( node.oprand1[0] ) ;
					node.oprand2[0] = tolower( node.oprand2[0] ) ;
				} // if
				node.done = true ;
			} // else if
			
			else if ( format4 ) {                     // format4
				format4 = true ;
				char binaryNum[3] = "" ;
				itoa ( node.objCode[1] - '0', binaryNum, 2 ) ;
				if ( strcmp( binaryNum, "0" ) == 0 ) 
					strcpy( binaryNum, "0000" ) ;
				
				char saveLoc[5] = "" ;
				strcpy( saveLoc, Loc ) ;
				strcpy( Loc, binaryNum ) ;
				addHex( "0011" ) ;
				strcpy( binaryNum, Loc ) ;
				strcpy( Loc, saveLoc ) ;
				
				node.objCode[1] = binToHex( binaryNum ) ;
				
				bool findX = false ;
				for ( int j = 0 ; j < strlen( node.oprand1 ) ; j++ ) 
					if ( node.oprand1[j] == 'X' ) findX = true ;
				
				for ( int j = 0 ; j < strlen( node.oprand2 ) ; j++ ) 
					if ( node.oprand2[j] == 'X' ) findX = true ;
					
				if ( findX ) node.objCode[2] = binToHex( "1001" ) ;
				else node.objCode[2] = binToHex( "0001" ) ;
				
				if ( node.oprand1[0] == '#' ) {  // 要測 !!!!!!!!!!!!!!!!!!!!!!!! 
					int temp = strlen( node.oprand1 ) - 1 ;
					for ( int i = 3 ; i < 7-temp ; i++ ) {
						strcat( node.objCode, "0" ) ;
					} // for
					strcat( node.objCode, node.oprand1 ) ;
					node.done = true ;
				} // if
				else {
					if ( checkTable( node.oprand1, "5" ) ) {   // 如果pass1在symbol table找到該lebal 
						strcat( node.objCode, "0" ) ;
						for ( int i = 0 ; i < 100 ; i++ ) { 
		 					if ( strcmp( symbolTable[i].name, node.oprand1 ) == 0 ) {
		 						strcat( node.objCode, symbolTable[i].location ) ;
		 						node.done = true ;
		 						break ;
					 		} // if
						} // for 
					} // if
				} // else
			} // else if
			
			else {                     // format3
				format3 = true ;
				
				if ( checkTable( node.oprand1, "5" ) ) {   // 如果pass1在symbol table找到該lebal 
					for ( int i = 0 ; i < 100 ; i++ ) { 
		 				if ( strcmp( symbolTable[i].name, node.oprand1 ) == 0 ) {
		 					strcat( node.objCode, symbolTable[i].location ) ;
		 					node.done = true ;
		 					break ;
					 	} // if
					} // for 
				
					char binaryNum[3] = "" ;
					itoa ( node.objCode[1] - '0', binaryNum, 2 ) ;
					if ( strcmp( binaryNum, "0" ) == 0 ) 
						strcpy( binaryNum, "0000" ) ;
				
					if ( strlen(binaryNum) != 4 ) {
						int temp = strlen( binaryNum ) - 1 ;
						char tmp[5] = "" ;
						for ( int i = 0 ; i < 4-temp ; i++ ) {
							strcpy( tmp, "0" ) ;
						} // for
						strcat( tmp, binaryNum ) ;
						strcpy( binaryNum, tmp ) ;
					} // if
				
					char saveLoc[5] = "" ;
					strcpy( saveLoc, Loc ) ;
					strcpy( Loc, binaryNum ) ;
					addHex( "0011" ) ;
					strcpy( binaryNum, Loc ) ;
					strcpy( Loc, saveLoc ) ;
				
					node.objCode[1] = binToHex( binaryNum ) ;
			
					if ( strcmp( node.opcode, "comp") == 0 || strcmp( node.opcode, "COMP") == 0 ) {
						if ( strcmp( node.oprand1, "0") == 0 ) {
							strcat( node.objCode, "0000" ) ;
						} // if
					} // if
					else {
						bool findX = false ;
						for ( int j = 0 ; j < strlen( node.oprand1 ) ; j++ ) 
							if ( node.oprand1[j] == 'X' ) findX = true ;
				
						for ( int j = 0 ; j < strlen( node.oprand2 ) ; j++ ) 
							if ( node.oprand2[j] == 'X' ) findX = true ;
					
						if ( findX ) node.objCode[2] = binToHex( "1010" ) ; // 以PC計算 
						else node.objCode[2] = binToHex( "0010" ) ;
				
						for ( int i = 0 ; i < 4 ; i++ )
							cout << node.objCode[i] << endl ;
						dsip() ; 
						node.done = true ;
					} // else
					node.done = true ;
				} // if
			} // else if
		} // else 		
	} // convert_to_Mcode()
	
	char binToHex( char Num[3] ) {
		int num = 0, sum = 0 ;
		for ( int i = 0 ; i <= 3 ; i++ ) {
			num = (int)Num[i] - '0' ;
			sum = sum * 2 + num ;
		} // for
				
		char temp ;
		if ( sum >= 0 && sum <= 9 ) temp = sum + '0' ;
		else if ( sum == 10 ) temp = 'A' ;
		else if ( sum == 11 ) temp = 'B' ;
		else if ( sum == 12 ) temp = 'C' ;
		else if ( sum == 13 ) temp = 'D' ;
		else if ( sum == 14 ) temp = 'E' ;
		else if ( sum == 15 ) temp = 'F' ;
		
		return temp ;
	} // binToHex()

	void dsip() {
		char dest[12] = "", sour[12] = "" ;
		char tmp[3], temp[3] ;
		
		char saveLoc[5] = "" ;
		strcpy( saveLoc, Loc ) ;
		addHex( "0003" ) ;
		
		
		for ( int i = 1 ; i < strlen(Loc) ; i++ ) {
			if ( Loc[i] == '0' ) strcpy( temp, "0000" ) ; 
			else if ( Loc[i] == '1' ) strcpy( temp, "0001" ) ;
			else if ( Loc[i] == '2' ) strcpy( temp, "0010" ) ;
			else if ( Loc[i] == '3' ) strcpy( temp, "0011" ) ;
			else if ( Loc[i] == '4' ) strcpy( temp, "0100" ) ;
			else if ( Loc[i] == '5' ) strcpy( temp, "0101" ) ;
			else if ( Loc[i] == '6' ) strcpy( temp, "0110" ) ;
			else if ( Loc[i] == '7' ) strcpy( temp, "0111" ) ;
			else if ( Loc[i] == '8' ) strcpy( temp, "1000" ) ;
			else if ( Loc[i] == '9' ) strcpy( temp, "1001" ) ;
			else if ( Loc[i] == 'A' ) strcpy( temp, "1010" ) ;
			else if ( Loc[i] == 'B' ) strcpy( temp, "1011" ) ;
			else if ( Loc[i] == 'C' ) strcpy( temp, "1100" ) ;
			else if ( Loc[i] == 'D' ) strcpy( temp, "1101" ) ;
			else if ( Loc[i] == 'E' ) strcpy( temp, "1110" ) ;
			else if ( Loc[i] == 'F' ) strcpy( temp, "1111" ) ;
			strcat( sour, temp ) ;
		} // for
		strcpy( Loc, saveLoc ) ;
		
		for ( int i = 0 ; i < 100 ; i++ ) 
		 	if ( strcmp( symbolTable[i].name, node.oprand1 ) == 0 ) 
		 		strcpy( tmp, symbolTable[i].location ) ;

		for ( int i = 1 ; i < strlen(tmp) ; i++ ) {
			if ( tmp[i] == '0' ) strcpy( temp, "0000" ) ; 
			else if ( tmp[i] == '1' ) strcpy( temp, "0001" ) ;
			else if ( tmp[i] == '2' ) strcpy( temp, "0010" ) ;
			else if ( tmp[i] == '3' ) strcpy( temp, "0011" ) ;
			else if ( tmp[i] == '4' ) strcpy( temp, "0100" ) ;
			else if ( tmp[i] == '5' ) strcpy( temp, "0101" ) ;
			else if ( tmp[i] == '6' ) strcpy( temp, "0110" ) ;
			else if ( tmp[i] == '7' ) strcpy( temp, "0111" ) ;
			else if ( tmp[i] == '8' ) strcpy( temp, "1000" ) ;
			else if ( tmp[i] == '9' ) strcpy( temp, "1001" ) ;
			else if ( tmp[i] == 'A' ) strcpy( temp, "1010" ) ;
			else if ( tmp[i] == 'B' ) strcpy( temp, "1011" ) ;
			else if ( tmp[i] == 'C' ) strcpy( temp, "1100" ) ;
			else if ( tmp[i] == 'D' ) strcpy( temp, "1101" ) ;
			else if ( tmp[i] == 'E' ) strcpy( temp, "1110" ) ;
			else if ( tmp[i] == 'F' ) strcpy( temp, "1111" ) ;
			
			strcat( dest, temp ) ;
		} // for
		
		// disp = 目的 - 來源 
		for ( int i = 0 ; i < 12 ; i++ ) {
			if ( sour[i] == '0' ) sour[i] = '1' ;
			else if( sour[i] == '1' ) sour[i] = '0' ;
		} // for
		sour[12] = '\0' ;
		
		bool carry = false, first = true ;
		for ( int i = 11 ; i >= 0 ; i-- ) {
			if ( first ) {
				sour[i]++ ;
				first = false ;
			} // if
			if ( carry ) {
				sour[i]++ ;  
				carry = false ;
			} // if
			
			if ( sour[i] == '2' ) {
				carry = true ;
				sour[i] = '0' ;
			} // if
		} // for
		
		for ( int i = 11 ; i >= 0 ; i-- ) {
			sour[i] = sour[i] + dest[i] - '0' ;
			
			if ( carry ) {
				sour[i]++ ;  
				carry = false ;
			} // if
			
			if ( sour[i] == '2' ) {
				carry = true ;
				sour[i] = '0' ;
			} // if
			else if ( sour[i] == '3' ) {
				carry = true ;
				sour[i] = '1' ;
			} // else if
		} // for
		
		int i = 0, count = 3 ;
		memset( temp, 0, 3 ) ;
		for ( int k = 0 ; k < 3 ; k++ ) {
			for ( int j = 0 ; j < 4 ; j++ ) {
				temp[j] = sour[i] ;
				i++ ;
			} // for
			
			cout << "temp  " << temp << endl ;
			node.objCode[count] = binToHex( temp ) ;
			count++ ;
		} // for
		
		cout << count << endl ;
		node.objCode[count] = '\0' ;
	} // dsip()
	
	void judgeTable( char temp[60] ) {  // ==================== 判斷在哪個table
		if ( checkTable( temp, "3" ) ) {         // register
			cout << temp << "	register" << endl ;
			
			if ( strlen( node.oprand1 ) == 0 ) strcpy( node.oprand1, temp ) ;	
      		else strcpy( node.oprand2, temp ) ;	
      		
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
				cout << temp << "	integer" << endl ;
				hashing( temp, 6 ) ;
			} // if
			else {                               // 存進table5(symbol) 
				cout << temp << "	symbol" << endl ;
				hashing( temp, 5 ) ;
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
				if( ch[0] == '\t' || ch[0] == ' ' ) {        // 為定義label的位置 (第一行)
					strcat( symbolTable[sum].name, str ) ;
					strcat( symbolTable[sum].location, Loc ) ;
				} // if
			} // if
			else {
				bool same = false ;
				for ( ; symbolTable[sum].name[0] != NULL ; sum++ )
					if ( strcmp( symbolTable[sum].name, str ) == 0 ) same = true ;
				
				if ( (same && ch[0] == '\t') || (same && ch[0] == ' ') ) {       // 重複定義(error)
					pass1Error = true ;
					
				} // if
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
		
		if ( ch[0] != ' ' && ( ch[0] == '\n' || ( checkDelimiter( ch[0] ) && ch[0] != '\'' ) ) ) {
			strcat( node.oprand1, str ) ;
		} // if	
			
		else if ( ch[0] == '\t' /*&& strlen( node.oprand ) == 0) */|| ch[0] == ' ' ) {
			strcat( node.lebal, str ) ;
		
		}
	} // hashing()

    bool checkTable( char key[60], string num ) {  // ========= 檢查是否為instructuin/pseudo/reg
    	char tmp[50] , tempkey[60] = "" ;
    	strcpy( tempkey, key ) ;
    	
    	string fileName ;
		fileName = "Table" + num + ".table" ;            // 檢查table1(instruction)
    	FILE * inFile = fopen( fileName.c_str(), "rb" ) ;    	
    	while ( fgets( tmp, 50, inFile ) ) {
    		tmp[ strlen(tmp) ] = '\n' ;
    		
    		if ( num.compare( "1" ) == 0 ) 
    			for( int i = 0 ; i < strlen(tempkey) ; i++ )  // 轉換成小寫 
    		    	tempkey[i] = tolower( tempkey[i] ) ;
    		else if ( num.compare( "2" ) == 0 || num.compare( "3" ) == 0 )    	
    			for( int i = 0 ; i < strlen(tempkey) ; i++ )  // 轉換成小寫 
    		    	tempkey[i] = toupper( tempkey[i] ) ;
    
    		if ( strstr( tmp, tempkey ) != NULL ) {       // tabal1裡是否包含
    			fclose(inFile) ;
    			return true ;
			} // if
		} // while
		
		fclose(inFile) ;
		return false ;
	} // checkTable()
	
	void writeFile() {  // ==================================== 寫output檔
		DATA data;
    	int data_size;
    	data_size = sizeof(struct DATA);
	
    	FILE *fp_output ;
  
		string temp = "SICXE_output.txt" ;
    	fp_output = fopen( temp.c_str() , "w+");
  
    	if (fp_output == NULL) {
        	cout << "Could not open " << "output file." << endl;
        } // if
  
  		char tempStr[] = "Line	Location	Source code		Object code" ;
  		fprintf( fp_output,"%s \n\n", tempStr ) ; 

 		for ( int i = 0 ; i < output.size() ; i++ ) {
		 	fprintf( fp_output,"%3d	%s	%s		%s	%s",  
			output[i].line, output[i].loc, output[i].lebal, output[i].opcode, output[i].oprand1 ) ; // write data as required
			
			if ( strlen( output[i].oprand2 ) != 0 ) {
				fprintf( fp_output,",%s	%s\n",  output[i].oprand2, output[i].objCode ) ;
			}
			else fprintf( fp_output,"	%s\n",  output[i].objCode ) ;

		} // for

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
	
	SICXE sixce ; 
	sixce.XE_readFile_and_Pass1() ;
	sixce.Pass2() ;
	sixce.writeFile() ;
	
} // end main

