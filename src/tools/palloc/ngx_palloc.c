
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#define NGX_EPERM                  ERROR_ACCESS_DENIED
#define NGX_ENOENT                 ERROR_FILE_NOT_FOUND
#define NGX_ENOPATH                ERROR_PATH_NOT_FOUND
#define NGX_ENOMEM                 ERROR_NOT_ENOUGH_MEMORY
#define NGX_EACCES                 ERROR_ACCESS_DENIED
#include <malloc.h>
#else
#include <stddef.h>
#endif // _WIN32
#include <stdint.h>
#include <assert.h>
#include "palloc.h"
#include "ngx_palloc.h"
 //#include "ngx_buf.h"
  //#include <ngx_config.h>
  //#include <ngx_core.h>



static ngx_inline void* ngx_palloc_small(ngx_pool_t* pool, size_t size,
	uint32_t align);
static void* ngx_palloc_block(ngx_pool_t* pool, size_t size);
static void* ngx_palloc_large(ngx_pool_t* pool, size_t size);

#define ngx_free(p)          dev->pfree(dev->ctx,p)
#define ngx_memalign(alignment, size)  dev->palloc(dev->ctx,size)
#define ngx_free1         free
#define ngx_memalign1(alignment, size) malloc(size)

#define pool2dev(pt) alloc_info_t* dev = &pt->ctx->_dev

#define NGX_FILE_ERROR              0


int ngx_pool_sizeof()
{
	return sizeof(ngx_pool_t);
}
ngx_pool_t* ngx_create_pool(palloc_t* ctx, size_t size)
{
	ngx_pool_t* p;
	alloc_info_t* dev = &ctx->_dev;
	int nse = sizeof(ngx_pool_t);
	assert(size > nse);
	p = ngx_memalign(NGX_POOL_ALIGNMENT, size);
	if (p == NULL) {
		return NULL;
	}
	p->ctx = ctx;
	p->d.last = (u_char*)p + nse;
	p->last = p->d.last;
	p->d.end = (u_char*)p + size;
	p->d.next = NULL;
	p->d.failed = 0;
	p->d.front = 0;
	p->d.count = 0;
	p->_count = 1;
	uint32_t pagesize = dev->mdi.pagesize - 1;
	p->max = (size < pagesize) ? size : pagesize;

	p->current = p;
	p->chain = NULL;
	p->large = NULL;
	p->cleanup = NULL;
	//p->log = log;

	return p;
}

void ngx_destroy_pool(ngx_pool_t* pool)
{
	ngx_pool_t* p, * n;
	ngx_pool_large_t* l;
	ngx_pool_cleanup_t* c;
	alloc_info_t* dev = 0;
	int64_t count = 0;
	if (!pool)
	{
		return;
	}
	dev = &pool->ctx->_dev;
	count = pool->_count;
	for (c = pool->cleanup; c; c = c->next) {
		if (c->handler) {
			//ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,				"run cleanup: %p", c);
			c->handler(c->data);
		}
	}

#if (NGX_DEBUG)

	/*
	 * we could allocate the pool->log from this pool
	 * so we cannot use this log while free()ing the pool
	 */

	for (l = pool->large; l; l = l->next) {
		ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0, "free: %p", l->alloc);
	}

	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
		ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
			"free: %p, unused: %uz", p, p->d.end - p->d.last);

		if (n == NULL) {
			break;
		}
	}

#endif

	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			ngx_free(l->alloc);
			count--;
		}
	}

	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
		ngx_free(p);
		count--;
		if (n == NULL) {
			break;
		}
	}
	if (count > 0)
	{
		printf("mem error!\n");
	}
}


void ngx_reset_pool(ngx_pool_t* pool)
{
	ngx_pool_t* p;
	ngx_pool_large_t* l;
	alloc_info_t* dev = 0;
	if (!pool)
	{
		return;
	}
	dev = &pool->ctx->_dev;
	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			ngx_free(l->alloc); pool->_count--;
		}
	}

	for (p = pool; p; p = p->d.next) {
		p->d.last = (u_char*)p + sizeof(ngx_pool_t);
		p->d.failed = 0;
		p->d.front = 0;
		p->d.count = 0;
	}

	pool->current = pool;
	pool->chain = NULL;
	pool->large = NULL;
}

/*
	todo ngx_palloc
*/
void* ngx_palloc(ngx_pool_t* pool, size_t size)
{
	if (!size)
	{
		return 0;
	}
#if !(NGX_DEBUG)
	if (size <= pool->max) {
		return ngx_palloc_small(pool, size, 1);
	}
#endif

	return ngx_palloc_large(pool, size);
}


/*
	todo ngx_pnalloc
*/
void* ngx_pnalloc(ngx_pool_t* pool, size_t size)
{
#if !(NGX_DEBUG)
	if (size <= pool->max) {
		return ngx_palloc_small(pool, size, 0);
	}
#endif

	return ngx_palloc_large(pool, size);
}


static ngx_inline void* ngx_palloc_small(ngx_pool_t* pool, size_t size, uint32_t align)
{
	u_char* m;
	ngx_pool_t* p;
	int64_t ns = size, l, e;
	if (!pool || ns < 0)
	{
		return 0;
	}
	p = pool->current;
	l = p->d.last, e = p->d.end;
	if (l > e)
	{
		l = l;
	}
	//assert(p->d.last < p->d.end);
	do {
		m = p->d.last;

		if (align) {
			m = ngx_align_ptr(m, NGX_ALIGNMENT);
		}

		if ((size_t)(p->d.end - m) >= size) {
			p->d.last = m + size;
			p->d.front = m;
			p->d.count++;
			return m;
		}

		p = p->d.next;

	} while (p);

	return ngx_palloc_block(pool, size);
}


static void* ngx_palloc_block(ngx_pool_t* pool, size_t size)
{
	u_char* m;
	size_t       psize;
	ngx_pool_t* p, * new;
	alloc_info_t* dev = 0;
	int64_t ns = size;
	if (!pool || ns < 0)
	{
		return 0;
	}
	dev = &pool->ctx->_dev;
	psize = (size_t)(pool->d.end - (u_char*)pool);

	m = ngx_memalign(NGX_POOL_ALIGNMENT, psize);
	if (m == NULL) {
		return NULL;
	}
	pool->_count++;
	new = (ngx_pool_t*)m;

	new->d.end = m + psize;
	new->d.next = NULL;
	new->d.failed = 0;
	new->d.count = 1;

	m += sizeof(ngx_pool_data_t);
	m = ngx_align_ptr(m, NGX_ALIGNMENT);
	new->d.front = m;
	new->d.last = m + size;

	for (p = pool->current; p->d.next; p = p->d.next) {
		if (p->d.failed++ > 4) {
			pool->current = p->d.next;
		}
	}

	p->d.next = new;

	return m;
}


static void* ngx_palloc_large(ngx_pool_t* pool, size_t size)
{
	void* p;
	uint32_t         n;
	ngx_pool_large_t* large;
	alloc_info_t* dev = 0;
	int64_t ns = size;
	if (!pool || ns < 0)
	{
		return 0;
	}
	dev = &pool->ctx->_dev;
	p = ngx_memalign(16, size);
	if (p == NULL) {
		return NULL;
	}
	pool->_count++;
	n = 0;

	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = p;
			return p;
		}

		if (n++ > 3) {
			break;
		}
	}

	large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
	if (large == NULL) {
		ngx_free(p); pool->_count--;
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}


void* ngx_pmemalign(ngx_pool_t* pool, size_t size, size_t alignment)
{
	void* p;
	ngx_pool_large_t* large;
	alloc_info_t* dev = 0;
	if (!pool)
	{
		return -1;
	}
	dev = &pool->ctx->_dev;
	p = ngx_memalign(alignment, size);
	if (p == NULL) {
		return NULL;
	}
	pool->_count++;
	large = ngx_palloc_small(pool, sizeof(ngx_pool_large_t), 1);
	if (large == NULL) {
		ngx_free(p); pool->_count--;
		return NULL;
	}

	large->alloc = p;
	large->size = size;
	large->next = pool->large;
	pool->large = large;

	return p;
}


/*
	todo ngx_pfree
*/
int ngx_pfree(ngx_pool_t* pool, void* p)
{
	ngx_pool_large_t* l;
	ngx_pool_t* po;
	alloc_info_t* dev = 0;
	if (!pool || !p)
	{
		return -1;
	}
	dev = &pool->ctx->_dev;
	for (l = pool->large; l; l = l->next) {
		if (p == l->alloc) {
			ngx_free(l->alloc); pool->_count--;
			l->alloc = NULL;
			l->size = 0;
			return NGX_OK;
		}
	}
	po = pool;
	// 计数d.count归0则这块内存可以重新分配
	if (po->d.count == 1)
	{
		po = po;
	}
	do {
		u_char* b = (u_char*)po;// +sizeof(ngx_pool_t);
		u_char* up = (u_char*)p;
		uint64_t b6 = 0, u6 = up - b, po6 = po->d.end - b;
		int64_t d1 = u6 - b6;
		int64_t d2 = po6 - u6;
		if (up > b && up < po->d.end) {
			po->d.count--;
			if (po->d.count <= 0)
			{
				po->d.last = (u_char*)po + sizeof(ngx_pool_t);
				po->d.failed = 0;
				po->d.count = 0;
			}
			return 0;
		}
		po = po->d.next;
	} while (po);
	return NGX_DECLINED;
}

/*
	todo ngx_pcalloc
*/
void* ngx_pcalloc(ngx_pool_t* pool, size_t count, size_t size)
{
	void* p;
	size_t as = count * size;
	p = ngx_palloc(pool, as);
	if (p)
	{
		size_t* c = (size_t*)p;
		size_t ls = as % sizeof(size_t);
		size_t ct = as / sizeof(size_t);
		for (size_t i = 0; i < ct; i++, c++)
		{
			if (*c)
			{
				*c = 0;
			}
		}
		if (ls)
			memset(c, 0, ls);
	}
	return p;
}

/*
	todo ngx_prealloc
*/
void* ngx_prealloc(ngx_pool_t* pool, void* ptr, size_t size)
{
	u_char* m = (u_char*)ptr;
	u_char* m1 = (u_char*)ptr;
	ngx_pool_t* p = pool;
	alloc_info_t* dev = 0;
	ngx_pool_large_t* l;
	int64_t olds = size;
	if (!pool || !p || !size)
	{
		return 0;
	}
	if (!ptr)
	{
		return ngx_palloc(pool, size);
	}
	if (size > pool->max)
	{
		dev = &pool->ctx->_dev;
		for (l = pool->large; l; l = l->next) {
			if (ptr == l->alloc) {
				if (dev->prealloc)
				{
					m1 = dev->prealloc(dev->ctx, ptr, size);
				}
				else
				{
					m1 = dev->palloc(dev->ctx, size);
					if (m1)
					{
						memcpy(m1, ptr, l->size);
						dev->pfree(dev->ctx, ptr);
					}
				}
				if (m1)
				{
					l->alloc = m1;
					l->size = size;
				}
				return m1;
			}
		}
	}

	// 从小块管理找
	do {
		u_char* b = (u_char*)p + sizeof(ngx_pool_t) - 1;
		u_char* up = (u_char*)ptr;
		if (m == p->d.front && ((size_t)(p->d.end - m) >= size)) {
			p->d.last = m + size;
			return m;
		}
		if (up > b && up < p->d.end)
		{
			olds = p->d.end - up;
			if (olds > size)
			{
				olds = size;
			}
			break;
		}
		p = p->d.next;
	} while (p);
	m = ngx_palloc(pool, size);
	// 不保存大小所以复制整个大小
	memcpy(m, ptr, olds);
	ngx_pfree(pool, ptr);
	return m;
}


ngx_pool_cleanup_t* ngx_pool_cleanup_add(ngx_pool_t* p, size_t size)
{
	ngx_pool_cleanup_t* c;

	c = ngx_palloc(p, sizeof(ngx_pool_cleanup_t));
	if (c == NULL) {
		return NULL;
	}

	if (size) {
		c->data = ngx_palloc(p, size);
		if (c->data == NULL) {
			return NULL;
		}

	}
	else {
		c->data = NULL;
	}

	c->handler = NULL;
	c->next = p->cleanup;

	p->cleanup = c;

	//ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, p->log, 0, "add cleanup: %p", c);

	return c;
}
#ifdef ngx_close_file 

void ngx_pool_run_cleanup_file(ngx_pool_t* p, ngx_fd_t fd)
{
	ngx_pool_cleanup_t* c;
	ngx_pool_cleanup_file_t* cf;

	for (c = p->cleanup; c; c = c->next) {
		if (c->handler == ngx_pool_cleanup_file) {

			cf = c->data;

			if (cf->fd == fd) {
				c->handler(cf);
				c->handler = NULL;
				return;
			}
		}
	}
}


void ngx_pool_cleanup_file(void* data)
{
	ngx_pool_cleanup_file_t* c = data;

	//ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d",c->fd);

	if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
		//ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,		ngx_close_file_n " \"%s\" failed", c->name);
	}
}


void ngx_pool_delete_file(void* data)
{
	ngx_pool_cleanup_file_t* c = data;

	ngx_err_t  err;

	//ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, c->log, 0, "file cleanup: fd:%d %s",		c->fd, c->name);

	if (ngx_delete_file(c->name) == NGX_FILE_ERROR) {
		err = -1;

		if (err != NGX_ENOENT) {
			//ngx_log_error(NGX_LOG_CRIT, c->log, err,				ngx_delete_file_n " \"%s\" failed", c->name);
		}
	}

	if (ngx_close_file(c->fd) == NGX_FILE_ERROR) {
		//ngx_log_error(NGX_LOG_ALERT, c->log, ngx_errno,			ngx_close_file_n " \"%s\" failed", c->name);
	}
}
#endif

#if 0

static void* ngx_get_cached_block(size_t size)
{
	void* p;
	ngx_cached_block_slot_t* slot;

	if (ngx_cycle->cache == NULL) {
		return NULL;
	}

	slot = &ngx_cycle->cache[(size + ngx_pagesize - 1) / ngx_pagesize];

	slot->tries++;

	if (slot->number) {
		p = slot->block;
		slot->block = slot->block->next;
		slot->number--;
		return p;
	}

	return NULL;
}
#endif
typedef struct palloc_data_s palloc_data;
void* def_malloc(void* ctx, size_t s)
{
	alloc_info_t* ct = ((alloc_info_t*)ctx);
	void* ptr = malloc(s);
	if (ptr && ct)
	{
		ct->count++;
		ct->alloc_size += msize(ptr);
	}
	return ptr;
}
void* def_realloc(void* ctx, size_t c, size_t s)
{
	alloc_info_t* ct = ((alloc_info_t*)ctx);
	void* ptr = realloc(c, s);
	if (ptr && ct)
	{
		ct->count++;
		ct->alloc_size += msize(ptr);
	}
	return ptr;
}
void def_free(void* ctx, void* p)
{
	if (ctx && p)
	{
		alloc_info_t* ct = ((alloc_info_t*)ctx);
		ct->count--;
		size_t ms = msize(p);
		ct->alloc_size -= ms;
		free(p);

	}
}


int palloc_init(palloc_t* p, alloc_info_t* a)
//palloc_t* create_palloc(alloc_info_t* a)
{
	//palloc_t* p = (palloc_t*)malloc(sizeof(palloc_t));
	//memset(p, 0, sizeof(palloc_t));
	if (a && a->palloc && a->pfree)
	{
		memcpy(&p->_dev, a, sizeof(alloc_info_t));
	}
	else
	{
		p->_dev.count = 0;
		p->_dev.ctx = &p->_dev;
		p->_dev.palloc = def_malloc;
		p->_dev.prealloc = def_realloc;
		p->_dev.pfree = def_free;
	}
	if (!p->_dev.mdi.pagesize)
	{
		get_mem_info(&p->_dev.mdi);
	}
	//p->_pool_sizeof = ngx_pool_sizeof();
	p->create_pool = ngx_create_pool;
	p->destroy_pool = ngx_destroy_pool;
	p->reset_pool = ngx_reset_pool;

	p->palloc = ngx_palloc;
	p->pnalloc = ngx_pnalloc;
	p->pcalloc = ngx_pcalloc;
	p->prealloc = ngx_prealloc;
	p->pmemalign = ngx_pmemalign;
	p->pfree = ngx_pfree;
	//p->pool_cleanup_add = ngx_pool_cleanup_add;
	//p->pool_run_cleanup_file = ngx_pool_run_cleanup_file;
	//p->pool_cleanup_file = ngx_pool_cleanup_file;
	//p->pool_delete_file = ngx_pool_delete_file;
	return 0;
}

void palloc_close(palloc_t* p)
{

}
void destroy_palloc(palloc_t* p)
{
	if (p)
	{
		free(p);
	}
}


void get_mem_info(mem_dev_info_t* p)
{
	if (p)
	{
#ifdef _WIN32
		SYSTEM_INFO  si;
		GetSystemInfo(&si);
		p->pagesize = si.dwPageSize;
		p->allocation_granularity = si.dwAllocationGranularity;
		p->ncpu = si.dwNumberOfProcessors;

#else
		p->pagesize = getpagesize();

#endif // _WIN32
		p->cacheline_size = sizeof(void*) * 8;
		for (uint32_t n = p->pagesize; n >>= 1; p->pagesize_shift++) { /* void */ }

	}
}
