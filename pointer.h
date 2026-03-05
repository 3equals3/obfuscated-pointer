#include <iostream>
#include <cstdint>
#include <atomic>

namespace eq3 {
	namespace detail {
		inline std::atomic<uint64_t>& key_counter() {
			static std::atomic<uint64_t> counter{0xDEADBEEFCAFEBABE};
			return counter;
		}
	}

	template<typename T>
	class encrypted_pointer {
	private:
		volatile uintptr_t _encryptedAddress;
		volatile uintptr_t _key;

		static inline uint64_t generate_unique_key() {
			return detail::key_counter().fetch_add(0x1337BEEFDEADC0DE, std::memory_order_relaxed);
		}

	public:
		encrypted_pointer() : _encryptedAddress(0), _key(0) {}

		encrypted_pointer(T* ptr) : _key(generate_unique_key()) {
			_encryptedAddress = reinterpret_cast<uintptr_t>(ptr);
			_encryptedAddress ^= _key;
			_encryptedAddress = (_encryptedAddress << 8) | (_encryptedAddress >> (sizeof(uintptr_t) * 8 - 8));
		}

		inline T* get() {
			uintptr_t decryptedAddress = _encryptedAddress;
			decryptedAddress =
				(decryptedAddress << (sizeof(uintptr_t) * 8 - 8)) | (decryptedAddress >> 8);
			decryptedAddress ^= _key;
			return reinterpret_cast<T*>(decryptedAddress);
		}

		inline T* set(T* ptr) {
			_key = generate_unique_key();
			_encryptedAddress = reinterpret_cast<uintptr_t>(ptr);
			_encryptedAddress ^= _key;
			_encryptedAddress =
				(_encryptedAddress << 8) | (_encryptedAddress >> (sizeof(uintptr_t) * 8 - 8));
			return ptr;
		}

	public:
		inline T* operator->() {
			return get();
		}
		inline T* operator=(T* ptr) {
			return set(ptr);
		}
		inline bool operator==(T* ptr) {
			return get() == ptr;
		}
	};
}
