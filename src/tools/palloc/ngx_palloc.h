
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


 //#include <ngx_config.h>
 //#include <ngx_core.h>
#ifndef u_char
typedef unsigned char u_char;
#endif // !u_char

#ifndef NGX_ALIGNMENT
#define NGX_ALIGNMENT   sizeof(unsigned long)    /* platform word */
#endif

#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#ifdef _WIN32
typedef HANDLE                      ngx_fd_t;
#else
typedef int                      ngx_fd_t;
#endif // _WIN32
typedef int ngx_err_t;
#define ngx_abort       abort


/* TODO: platform specific: array[NGX_INVALID_ARRAY_INDEX] must cause SIGSEGV */
#define NGX_INVALID_ARRAY_INDEX 0x80000000


/* TODO: auto_conf: ngx_inline   inline __inline __inline__ */
#ifndef ngx_inline
#define ngx_inline      inline
#endif


#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_AGAIN      -2
#define  NGX_BUSY       -3
#define  NGX_DONE       -4
#define  NGX_DECLINED   -5
#define  NGX_ABORT      -6


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
 //#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void* data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {
	ngx_pool_cleanup_pt   handler;
	void* data;
	ngx_pool_cleanup_t* next;
};

typedef struct ngx_pool_s            ngx_pool_t;
typedef struct ngx_chain_s            ngx_chain_t;
typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {
	ngx_pool_large_t* next;
	void* alloc;
	size_t size;
};


typedef struct {
	u_char* last;
	u_char* end;
	u_char* front;
	ngx_pool_t* next;
	uint32_t failed;
	int64_t count;
} ngx_pool_data_t;


struct ngx_pool_s {
	ngx_pool_data_t       d;
	u_char* last;
	size_t                max;
	ngx_pool_t* current;
	ngx_chain_t* chain;
	ngx_pool_large_t* large;
	ngx_pool_cleanup_t* cleanup;
	//ngx_log_t* log;
	palloc_t* ctx;
	int64_t _count;
};


typedef struct {
	ngx_fd_t              fd;
	u_char* name;
	//ngx_log_t* log;
} ngx_pool_cleanup_file_t;


ngx_pool_t* ngx_create_pool(palloc_t* ctx, size_t size);
void ngx_destroy_pool(ngx_pool_t* pool);
void ngx_reset_pool(ngx_pool_t* pool);

void* ngx_palloc(ngx_pool_t* pool, size_t size);
void* ngx_pnalloc(ngx_pool_t* pool, size_t size);
void* ngx_pcalloc(ngx_pool_t* pool, size_t count, size_t size);
void* ngx_prealloc(ngx_pool_t* pool, void* ptr, size_t size);
void* ngx_pmemalign(ngx_pool_t* pool, size_t size, size_t alignment);
int ngx_pfree(ngx_pool_t* pool, void* p);


ngx_pool_cleanup_t* ngx_pool_cleanup_add(ngx_pool_t* p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t* p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void* data);
void ngx_pool_delete_file(void* data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */
