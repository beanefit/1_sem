#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum Compilier_modes
{
	first,
	second
};
char *get_src( char *src_file_name, size_t *src_sz );
size_t srcSize( FILE *src );
void *compile( char* src, size_t src_sz, size_t *exe_sz, enum Compilier_modes mode );
int makecode( void *exe, size_t exe_sz );

int main( int argc, char *argv[] )
{
	size_t src_sz = 0;
	char *src = get_src( argv[ 1 ], &src_sz );
	size_t exe_sz = 0;
	enum Compilier_modes mode = first;
	//compile( src, src_sz, &exe_sz, mode );
	mode = second;
	void *exe = compile( src, src_sz, &exe_sz, mode );
	makecode( exe, exe_sz );
	return 0;
}

char *get_src( char *src_file_name, size_t *src_sz )
{
	FILE *src_file = fopen( src_file_name, "r" );
	*src_sz = srcSize( src_file );
	char *src = ( char * )calloc( sizeof( char ), *src_sz );
	fread( src, sizeof( char ), *src_sz, src_file );
	fclose( src_file );
	return src;
}

size_t srcSize( FILE *src )
{
	fseek( src, 0, SEEK_END );
	size_t src_sz = ftell( src );
	rewind( src );
	return src_sz;
}

void *compile( char *src, size_t src_sz, size_t *exe_sz, enum Compilier_modes mode )
{
	#define DEF_CMD( name, num ) const int CMD_##name = num;
	#include "commands.h"
	#undef DEF_CMD

	void *exe = calloc( sizeof( double ), src_sz );
	void *exe_cur = exe;	
	int src_cur = 0;
	int src_cur_delta = 0;

	#define label_amount 20
	void *labels[ label_amount ] = {0};
	#undef label_amount

	#define _Compile_
	#include "commands.h"

	if( mode == first )
	{
		printf( "began 1st Compile\n" );
		while( src_cur < src_sz )
		{
			char *str = ( char * )calloc( 5, sizeof( char ) );
			sscanf( src + src_cur, "%s%n", str, &src_cur_delta );
			src_cur += src_cur_delta;
			if( strcmp( str, "label" ) == 0 )
			{
				CMD_LABEL_code;
			}
			if( strcmp( str, "push" ) == 0 )
			{
				char *tmp = ( char * )calloc( 8, sizeof( char ) );
				sscanf( src + src_cur, "%s", tmp );
				if( strcmp( tmp, "ax" ) == 0 || strcmp( tmp, "bx" ) == 0 || strcmp( tmp, "cx" ) == 0 || strcmp( tmp, "dx" ) == 0 )
				{
					exe_cur += 2 * sizeof( int );
				}
				else
				{
					exe_cur += sizeof( int ) + sizeof( double );
				}
				free( tmp );
			}
			if( strcmp( str, "pop" ) == 0 )
			{
				exe_cur += 2 * sizeof( int );
			}
			if( strcmp( str, "add" ) == 0 )
			{
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "mul" ) == 0 )
			{
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "div" ) == 0 )
			{
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "sub" ) == 0 )
			{
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "out" ) == 0 )
			{
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "jmp" ) == 0 )
			{
				exe_cur += sizeof( int ) + sizeof( void * );
			}

			free( str );
		}

		free( exe );
		return NULL;
	}
	
	if( mode == second )
	{
printf( "\nbegan 2nd Compile\n" );
		while( src_cur < src_sz )							
		{
			char *str = ( char * )calloc( 5, sizeof( char ) );
			sscanf( src + src_cur, "%s%n", str, &src_cur_delta );
			src_cur += src_cur_delta;
	
			if( strcmp( str, "push" ) == 0 )
			{
				char *tmp = ( char * )calloc( 8, sizeof( char ) );
				sscanf( src + src_cur, "%s", tmp );
				if( strcmp( tmp, "ax" ) == 0 || strcmp( tmp, "bx" ) == 0 || strcmp( tmp, "cx" ) == 0 || strcmp( tmp, "dx" ) == 0 )
				{
					memcpy( exe_cur, &CMD_PUSHR, sizeof( int ) );
					exe_cur += sizeof( int );
					REG_READ_code;
				}
				else
				{
					memcpy( exe_cur, &CMD_PUSH, sizeof( int ) );
					exe_cur += sizeof( int );
					CMD_PUSH_code;
				}
				free( tmp );
			}
			if( strcmp( str, "pop" ) == 0 )
			{
				memcpy( exe_cur, &CMD_POPR, sizeof( int ) );
				exe_cur += sizeof( int );
				REG_READ_code;
			}
			if( strcmp( str, "add" ) == 0 )
			{
				memcpy( exe_cur, &CMD_ADD, sizeof( int ) );
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "mul" ) == 0 )
			{
				memcpy( exe_cur, &CMD_MUL, sizeof( int ) );
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "div" ) == 0 )
			{
				memcpy( exe_cur, &CMD_DIV, sizeof( int ) );
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "sub" ) == 0 )
			{
				memcpy( exe_cur, &CMD_SUB, sizeof( int ) );
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "out" ) == 0 )
			{
				printf( "out\n" );
				memcpy( exe_cur, &CMD_OUT, sizeof( int ) );
				exe_cur += sizeof( int );
			}
			if( strcmp( str, "jmp" ) == 0 )
			{
				printf( "jmp\n" );
				memcpy( exe_cur, &CMD_JMP, sizeof( int ) );
				exe_cur += sizeof( int );
				CMD_JMP_code;
			}
			free( str );
		}
		*exe_sz = ( size_t ) exe_cur - ( size_t ) exe;
		return exe;
	}
}

int makecode( void *exe, size_t exe_sz )
{
	FILE *exe_file = fopen( "./../CPU/exe", "w" );
	printf( "1 " );
	fwrite( exe, exe_sz, sizeof( char ), exe_file );
	printf( "2\n" );
	fclose( exe_file );
}
