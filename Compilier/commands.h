#ifdef DEF_CMD
DEF_CMD( PUSH, push, 1, {
				int push_type = 0;
				void *arg = calloc( 1, sizeof( int ) );
				push_type();
				int cmd = CMD_PUSH + push_type;
				to_exe( &cmd, int );
				if( push_type != 0 )
				{
					to_exe( arg, int );
				}
				else
				{
					to_exe( arg, double );
				}
				free( arg );
			},

      2 * sizeof( int );{
				int push_type = 0;
				void *arg = calloc( 1, sizeof( int ) );
				push_type();
				free( arg );
				if( push_type == 0 )
					exe_cur += sizeof( int );
      			},

			{

			});
DEF_CMD( PUSHR, push, 2, , 0,  );
DEF_CMD( RAMPUSH, push, 3, , 0,  );
DEF_CMD( RAMPUSHR, push, 4, , 0,  );
DEF_CMD( POPR, pop, 5, {
				int push_type = 0;
				void *arg = calloc( 1, sizeof( int ) );
				push_type();
				int cmd = CMD_POPR + push_type - 1;
				to_exe( &cmd, int );
				to_exe( arg, int );
				free( arg );
			},

			2 * sizeof( int ),
			
			{

			});
DEF_CMD( RAMPOP, pop, 6, , 0, );
DEF_CMD( RAMPOPR, pop, 7, , 0, );
DEF_CMD( ADD, add, 8,  { to_exe( &CMD_ADD, int ); }, sizeof( int ),  );
DEF_CMD( MUL, mul, 9,  { to_exe( &CMD_MUL, int ); }, sizeof( int ),  );
DEF_CMD( DIV, div, 10, { to_exe( &CMD_DIV, int ); }, sizeof( int ),  );
DEF_CMD( SUB, sub, 11, { to_exe( &CMD_SUB, int ); }, sizeof( int ),  );
DEF_CMD( OUT, out, 12, { to_exe( &CMD_OUT, int ); }, sizeof( int ),  );
DEF_CMD( LABEL, label, 13, ,  0;{
					int label = -1;
					from_src( %d, &label );
					labels[ label ] = ( size_t )exe_cur - ( size_t )exe;
				},

				{

				});
DEF_CMD( JMP, jmp, 14,  {
				to_exe( &CMD_JMP, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JE, je, 15,    {
				to_exe( &CMD_JE, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JNE, jne, 16,  {
				to_exe( &CMD_JNE, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JA, ja, 17,    {
				to_exe( CMD_JA, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JAE, jae, 18,  {
				to_exe( &CMD_JAE, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JB, jb, 19,    {
				to_exe( &CMD_JB, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( JBE, jbe, 20,  {
				to_exe( &CMD_JBE, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( CALL, call, 21,{
				to_exe( &CMD_CALL, int );
				CMD_JMP_code();
			},

			sizeof( int ) + sizeof( size_t ),
			
			{
				
			});
DEF_CMD( RET, ret, 22, { to_exe( &CMD_RET, int ); }, sizeof( int ),  );
DEF_CMD( SQRT, sqrt, 23, { to_exe( &CMD_SQRT, int ); }, sizeof( int ),  );
#endif /*DEF_CMD*/

#define push_type()							\
do									\
{									\
	char *tmp = ( char * )calloc( 8, sizeof( char ) );		\
	char tmpc = 0;							\
	check_src( %c, &tmpc );						\
	while( tmpc == ' ' )						\
	{								\
		src_cur += 1;						\
		check_src( %c, &tmpc );					\
	}								\
	if( tmpc == '[' )						\
	{								\
		src_cur += 1;						\
		check_src( [%[^]]], tmp );				\
		if( ifreg( tmp ) )					\
		{							\
			push_type = 3;					\
			REG_READ_code();				\
		}							\
		else							\
		{							\
			push_type = 2;					\
			int index;					\
			from_src( %d, &index );				\
			memcpy( arg, &index, sizeof( int ) );		\
		}							\
	}								\
	else								\
	{								\
		check_src( %s, tmp );					\
		if( ifreg( tmp ) )					\
		{							\
			push_type = 1;					\
			REG_READ_code();				\
		}							\
		else							\
		{							\
			arg = realloc( arg, sizeof( double ) );		\
			double value = 0;				\
			from_src( %lg, &value );			\
			memcpy( arg, &value, sizeof( double ) );	\
		}							\
	}								\
	free( tmp );							\
} while( 0 )

#define REG_READ_code()							\
do									\
{									\
	src_cur += src_cur_delta;					\
	int reg_num = 0;						\
	if( strcmp( tmp, "ax" ) == 0 )					\
		reg_num = 1;						\
	if( strcmp( tmp, "bx" ) == 0 )					\
		reg_num = 2;						\
	if( strcmp( tmp, "cx" ) == 0 )					\
		reg_num = 3;						\
	if( strcmp( tmp, "dx" ) == 0 )					\
		reg_num = 4;						\
	memcpy( arg, &reg_num, sizeof( int ) );				\
} while( 0 )

#define CMD_JMP_code()							\
do									\
{									\
	int label = -1;							\
	from_src( %d, &label );						\
	to_exe( labels[ label ], size_t );				\
} while( 0 )

#define ifreg( tmp ) strcmp( tmp, "ax" ) == 0 || strcmp( tmp, "bx" ) == 0 || strcmp( tmp, "cx" ) == 0 || strcmp( tmp, "dx" ) == 0

#define	to_exe( ptr, size )						\
do									\
{									\
	memcpy( exe_cur, ptr, sizeof( size ) );			\
	exe_cur += sizeof( size );					\
} while( 0 )

#define from_src( type, dest )						\
do									\
{									\
	sscanf( src + src_cur, "type%n", dest, &src_cur_delta );	\
	src_cur += src_cur_delta;					\
} while( 0 )

#define check_src( type, dest )						\
do									\
{									\
	sscanf( src + src_cur, "type%n", dest, &src_cur_delta );	\
} while( 0 )