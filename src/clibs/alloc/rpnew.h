
#ifdef __cplusplus

#include <new>
#include <rpmalloc.h>

#ifdef _WIN32

extern void __CRTDECL
operator delete(void* p) noexcept {
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p) noexcept {
	rpfree(p);
}

extern void* __CRTDECL
operator new(std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

extern void* __CRTDECL
operator new[](std::size_t size) noexcept(false) {
	return rpmalloc(size);
}

extern void* __CRTDECL
operator new(std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

extern void* __CRTDECL
operator new[](std::size_t size, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpmalloc(size);
}

#if (__cplusplus >= 201402L || _MSC_VER >= 1916)

extern void __CRTDECL
operator delete(void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::size_t size) noexcept {
	(void)sizeof(size);
	rpfree(p);
}

#endif

#if (__cplusplus > 201402L || defined(__cpp_aligned_new))

extern void __CRTDECL
operator delete(void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::align_val_t align) noexcept {
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete(void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

extern void __CRTDECL
operator delete[](void* p, std::size_t size, std::align_val_t align) noexcept {
	(void)sizeof(size);
	(void)sizeof(align);
	rpfree(p);
}

extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc(align, size);
}

extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align) noexcept(false) {
	return rpaligned_alloc(align, size);
}

extern void* __CRTDECL
operator new(std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc(align, size);
}

extern void* __CRTDECL
operator new[](std::size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept {
	(void)sizeof(tag);
	return rpaligned_alloc(align, size);
}

#endif

#endif

#endif
