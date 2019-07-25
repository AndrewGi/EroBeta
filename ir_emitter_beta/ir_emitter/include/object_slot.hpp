#pragma once
#include <new>
namespace ero {
	template<uint32_t _SIZE> struct static_object_slot_t {
		using destructor_t = void(*)(void*);
		static constexpr uint32_t size() {
			return _SIZE;
		}
		bool contains() const {
			return _destructor!=nullptr;
		}
		void destroy() {
			if (contains()) {
				_destructor(ptr());
				_destructor = nullptr;
			}
		}
		void* ptr() {
			return _mem;
		}
		const void* ptr() const {
			return _mem;
		}
		template<class T, class... Args>
		T& construct(Args&&... args) {
			static_assert(sizeof(T)<=size(), "size bigger than object slot");
			destroy();
			void* p = ptr();
			T* out = new (p) T(static_cast<Args&&>(args)...);
			_destructor = [](void* ptr) {
				static_cast<T*>(ptr)->~T();
			};
			return *out;
		}
	private:
		uint8_t _mem[size()];
		destructor_t _destructor = nullptr;
	};
}