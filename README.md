CalcLex
=======

Lexical Analyzer for a Calculator language.
Contains 2 parts:
A Lexical Analyzer and a recursive descent parser for the grammer specified:
//rules								              //predict sets
program -> stmt_list $$			    	  {ID, read, write, $$} 
 
stmt_list -> stmt stmt_list	    		{ID, read, write } 
stmt_list -> <epsilon>			      	{ $$ } 
 
stmt -> ID := expr				    	    { ID } 
stmt -> read ID						          { read } 
stmt -> write expr				    	    { write } 
  
expr -> term term_tail			      	{ (, ID, numconst } 
 
term_tail -> + term term_tail	    	{ + } 
term_tail -> - term term_tail	     	{ - } 
term_tail -> <epsilon>				      { ),ID,read,write,$$} 
  
term -> factor factor_tail			     { (, ID, numconst } 
 
factor_tail -> * factor factor_tail { * } 
factor_tail -> / factor factor_tail { / } 
factor_tail -> <epsilon>			      { +, -, ), ID, read, write, $$} 
 
factor -> ( expr )					        { ( } 
factor -> ID						            { ID } 
factor -> numconst					        { numconst }
