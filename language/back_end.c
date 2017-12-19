#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex_analyser.h"
#include "./../tree_t/tree.h"
#include "./../log/log.h"
#include "back_end.h"

int	RAM_COUNTER = 0;
int	LABEL_COUNTER = 0;

int analyser_make_asm( struct analyser_t *analyser )
{
	FILE *dest = fopen( "asm", "w" );

	fprintf( dest, 	"call main\n"
			"end\n" );

	int RAM_end = 0;
	int RAM_beg = 0;

	back_node( analyser, tree_get_root( analyser -> tree ), dest, recurs, &RAM_beg, &RAM_end );
}

int back_node( struct analyser_t *analyser, struct tree_node_t *node, FILE *dest, enum back_type mode, int *RAM_b, int *RAM_e )
{
	struct lex_t *lexem = ( struct lex_t * )tree_get_elem( node );

	int RAM_beg = *RAM_b;
	int RAM_end = *RAM_e;

	switch( lexem -> key )
	{
		case 1:
		{
			fprintf( dest, "push %lg\n", lexem -> koeff );

			break;
		}

		case 2:
		{
			#define case_oper( operatr, cmd )							\
				case operatr:									\
				{										\
					back_node( analyser, L( node ), dest, normal, &RAM_beg, &RAM_end );	\
														\
					back_node( analyser, R( L( node ) ), dest, normal, &RAM_beg, &RAM_end );\
														\
					fprintf( dest, #cmd"\n" );						\
														\
					break;									\
				}

			switch( *lexem -> value )
			{
				case_oper( '+', add )
				case_oper( '-', sub )
				case_oper( '*', mul )
				case_oper( '/', div )
				
				case '=':
				{
					back_node( analyser, R( L( node ) ), dest, normal, &RAM_beg, &RAM_end );

					char *variable = ( (struct lex_t * )tree_get_elem( L( node ) ) ) -> value;

					for( int i = RAM_beg; i < RAM_end; i++ )
						if( !strcmp( analyser -> vars[ i ] -> name, variable ) )
							fprintf( dest, "pop [%d]\n", i );

					break;
				}
			}

			#undef case_oper

			break;
		}

		case 3:
		{
			if( *lexem -> value == '1' )
			{
				struct lex_t *fst_cond = ( struct lex_t * )tree_get_elem( L( L( node ) ) );
				check_pointer( fst_cond, 1 );

				#define if_oper( symb, cmd )							\
					if( *fst_cond -> value == symb )					\
					{									\
						back_node( analyser, L( L( L( node ) ) ), dest, normal, &RAM_beg, &RAM_end );	\
														\
						back_node( analyser, R( L( L( L( node ) ) ) ), dest, normal, &RAM_beg, &RAM_end );	\
														\
						fprintf( dest, #cmd" %d\n", LABEL_COUNTER );			\
					}

				if_oper( '<', jae )
				if_oper( '>', jbe )
				if_oper( '?', jne )

				#undef if_oper
			
				#define ne( symb )	\
					*fst_cond -> value != symb

				if( ne( '<' ) && ne( '>' ) && ne( '?' ) )
				{
					back_node( analyser, L( L( node ) ), dest, normal, &RAM_beg, &RAM_end );
				
					fprintf( dest, 	"push 0\n"
							"je %d\n", LABEL_COUNTER );
				}

				#undef ne

				back_node( analyser, R( L( node ) ), dest, recurs, &RAM_beg, &RAM_end );

				fprintf( dest, "label %d\n", LABEL_COUNTER++ );

				break;
			}

			if( *lexem -> value == '3' )
			{
				fprintf( dest, "label %d\n", LABEL_COUNTER );

				back_node( analyser, R( L( node ) ), dest, recurs, &RAM_beg, &RAM_end );

				struct lex_t *fst_cond = ( struct lex_t * )tree_get_elem( L( L( node ) ) );
				check_pointer( fst_cond, 1 );

				#define if_oper( symb, cmd )							\
					if( *fst_cond -> value == symb )					\
					{									\
						back_node( analyser, L( L( L( node ) ) ), dest, normal, &RAM_beg, &RAM_end );	\
														\
						back_node( analyser, R( L( L( L( node ) ) ) ), dest, normal, &RAM_beg, &RAM_end );	\
														\
						fprintf( dest, #cmd" %d\n", LABEL_COUNTER++ );			\
					}

				if_oper( '<', jb )
				if_oper( '>', ja )
				if_oper( '?', je )

				#undef if_oper

				#define ne( symb )	\
					*fst_cond -> value != symb

				if( ne( '<' ) && ne( '>' ) && ne( '?' ) )
				{
					back_node( analyser, L( L( node ) ), dest, normal, &RAM_beg, &RAM_end );
				
					fprintf( dest, 	"push 0\n"
							"jne %d\n", LABEL_COUNTER++ );
				}

				#undef ne
			}

			if( *lexem -> value == '4' )
			{
				back_node( analyser, L( L( node ) ), dest, normal, &RAM_beg, &RAM_end );

				fprintf( dest, "label %d\n", LABEL_COUNTER );

				back_node( analyser, R( L( node ) ), dest, recurs, &RAM_beg, &RAM_end );

				back_node( analyser, R( R( L( L( node ) ) ) ), dest, normal, &RAM_beg, &RAM_end );

				struct lex_t *fst_cond = ( struct lex_t * )tree_get_elem( R( L( L( node ) ) ) );
				check_pointer( fst_cond, 1 );

					#define if_oper( symb, cmd )							\
						if( *fst_cond -> value == symb )					\
					{										\
						back_node( analyser, L( R( L( L( node ) ) ) ), dest, normal, &RAM_beg, &RAM_end );		\
															\
						back_node( analyser, R( L( R( L( L( node ) ) ) ) ), dest, normal, &RAM_beg, &RAM_end );	\
															\
						fprintf( dest, #cmd" %d\n", LABEL_COUNTER++ );				\
					}

				if_oper( '<', jb )
				if_oper( '>', ja )
				if_oper( '?', je )

				#undef if_oper

				#define ne( symb )	\
					*fst_cond -> value != symb

				if( ne( '<' ) && ne( '>' ) && ne( '?' ) )
				{
					back_node( analyser, R( L( L( node ) ) ), dest, normal, &RAM_beg, &RAM_end );
				
					fprintf( dest, 	"push 0\n"
							"jne %d\n", LABEL_COUNTER++ );
				}

				#undef ne
			}
		}

		case 4:
		{
			for( int i = RAM_beg; i < RAM_end; i++ )
				if( !strcmp( analyser -> vars[ i ] -> name, lexem -> value ) )
					fprintf( dest, "push [%d]\n", i );

			break;
		}

		case 6:
		{
			struct func_t *func = lexem -> func;

			if( func -> mode == call )
			{
				for( int i = 0; i < func -> param_count; i++ )
				{
					struct lex_t *param = func -> params_arr[ i ];

					if( param -> key == 1 )
						fprintf( dest, "push %lg\n", param -> value );

					if( param -> key == 4 )
						for( int j = RAM_beg; j < RAM_end; j++ )
							if( !strcmp( analyser -> vars[ j ] -> name, lexem -> value ) )
								fprintf( dest, "push [%d]\n", j );
				}

				fprintf( dest, "call %s\n", lexem -> value );

				break;
			}

			RAM_beg = RAM_COUNTER;
			RAM_end = RAM_COUNTER;

			for( int i = 0; i < func -> param_count; i++ )
			{
				analyser -> vars[ RAM_end ] = ( struct var_t * )calloc( 1, sizeof( struct var_t ) );
				analyser -> vars[ RAM_end ] -> name = func -> params_arr[ i ] -> value;

				analyser -> vars[ RAM_end ] -> adrs = RAM_end++;
				
				RAM_COUNTER++;
			}

			fprintf( dest, "label %s\n", func -> name );

			back_node( analyser, L( node ), dest, recurs, &RAM_beg, &RAM_end );

			fprintf( dest, "ret\n" );

			break;
		}

		case 8:
		{
			analyser -> vars[ RAM_end ] = ( struct var_t * )calloc( 1, sizeof( struct var_t ) );
			analyser -> vars[ RAM_end ] -> name = ( ( struct lex_t * )tree_get_elem( L( node ) ) ) -> value;

			analyser -> vars[ RAM_end++ ] -> adrs = (*RAM_e)++;

			RAM_COUNTER++;

			break;
		}
	}

	if( mode == recurs )
		if( tree_get_next( node, right ) )
			back_node( analyser, R( node ), dest, recurs, &RAM_beg, &RAM_end );
}
