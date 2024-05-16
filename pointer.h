#include <iostream>
#include <cstdint>
#include <chrono>

namespace eq3 {
	extern bool random_initted = false;
	//constexpr long long ENCRYPTION_KEY = 0x22801337FEDCBA98; // if we want to use static this is required
	template<typename T>
	class encrypted_pointer {
	private:
		volatile uintptr_t _encryptedAddress;
		volatile uintptr_t _key = 0;
	public:
		inline unsigned long long getKey() {
			auto k = 0ll;
			for (int v6 = 8; v6 > 0; --v6) { // minecraft CID generation got reversed btw
				int v7 = rand();
				k = (k << 8) | (v7 & 0xFF);
			}
			return (_key = k);
		}
		encrypted_pointer() : _encryptedAddress(0) {}
		encrypted_pointer(T* ptr) {
			if (!random_initted) {
				srand(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				random_initted = !random_initted; // 我为这种胡言乱语感到羞耻
			} // 
			_encryptedAddress = reinterpret_cast<uintptr_t>(ptr);
			_key = getKey();
			_encryptedAddress ^= _key; // TODO: If it doesn't screw up optimization, then it’s advisable to use a chaotic algorithm
			_encryptedAddress = (_encryptedAddress << 8) | (_encryptedAddress >> (sizeof(uintptr_t) * 8 - 8));
		}

		inline T* get() {
			uintptr_t decryptedAddress = _encryptedAddress;
			decryptedAddress =
				(decryptedAddress << (sizeof(uintptr_t) * 8 - 8)) | (decryptedAddress >> 8); //TODO: IDA showing as __ROR8__(qword_1337, 8);
			decryptedAddress ^= _key;

			return reinterpret_cast<T*>(decryptedAddress);
		}

		inline T* set(T* ptr) {
			if (!random_initted) {
				srand(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
				random_initted = !random_initted; // yep code duping
			}
			_key = getKey();
			_encryptedAddress = reinterpret_cast<uintptr_t>(ptr);
			_encryptedAddress ^= _key;
			_encryptedAddress =
				(_encryptedAddress << 8) | (_encryptedAddress >> (sizeof(uintptr_t) * 8 - 8));
			return ptr;
		}
	public:
		inline T* operator->() { // estúpido
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
