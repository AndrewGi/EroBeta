#pragma once
namespace ero {
	struct BitQueue {
		static constexpr uint32_t bit_len_to_byte(uint32_t bit_len) {
			return bit_len / 8 + (bit_len % 8) ? 1 : 0;
		}

		uint32_t byte_len() const {
			return bit_len_to_byte(bit_len);
		}

		const uint8_t* bytes;
		const uint32_t bit_len;
		uint32_t bit_index;
		bool is_done() const {
			return bit_index >= bit_len;
		}

		bool get_bit(uint32_t index) const {
			if(index >= bit_len)
				return false; //out of range
			uint32_t byte_index = index / 8;
			uint32_t b_i = 8 - (index % 8);
			return (bytes[byte_index] >> b_i) & 1u;
		}

		bool next_bit() {
			return get_bit(bit_index++);
		}

		struct cursor_t {
			uint32_t bit_pos;
			const BitQueue &parent;

			bool is_done() const {
				return parent.bit_len <= bit_pos;
			}

			bool next() {
				if (is_done())
					return false;
				bool result = parent.get_bit(bit_pos);
				bit_pos++;
				return result;
			}
			uint32_t bits_left() const {
				return parent.bit_len - bit_pos;
			}
			uint8_t current_alignment() const {
				return bit_pos%8;
			}
			uint8_t aligned_byte() {
				if (current_alignment()!=0)
					return 0;
				if (bits_left()<8)
					return 0;
				uint8_t b = parent.bytes[bit_pos/8];
				bit_pos+=8;
				return b;
			}
		};

		cursor_t get_cursor() const {
			return cursor_t {bit_index, *this};
		}
	};
	template<uint32_t _bit_queue_size>
	struct StaticBitQueue : BitQueue {
		static constexpr uint32_t max_bit_len() {
			return _bit_queue_size;
		}
		static constexpr uint32_t max_byte_len() {
			return bit_len_to_byte(max_bit_len());
		}
		uint8_t _data[max_byte_len()];
		StaticBitQueue() : BitQueue(_data, max_bit_len(), 0) {}
	};
	template<class T>
	BitQueue make_bit_queue(const T &val) {
		return BitQueue {reinterpret_cast<const uint8_t *>(&val), sizeof(T) * 8, 0};
	}
}