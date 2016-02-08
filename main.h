#define BUF_SIZE 256
#define DEF_SIZE 16


struct block_info 
{
	long * create;
	long * destroy;
	int c_count;
	int d_count;
	int size;
};

