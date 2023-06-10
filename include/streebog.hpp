#pragma once

/*
 * Almost stolen code :(
 * I really tried to write it by myself.
*/

#include <algorithm>
#include "constants.hpp" // Also contains BLOCK_SIZE macro

class Streebog {
   public:
	Streebog() : h(BLOCK_SIZE),
				 n(BLOCK_SIZE),
				 sigma(BLOCK_SIZE){};

	// This function sets the digest size of the cryptographic hash function object.
	void setMode(int __digestSize) {
		if (!(__digestSize == 256 | __digestSize == 512)) throw "Error: Incorrect digest size. Must be 256 or 512";
		this->_setNewDigestSize(__digestSize);
	}

    // Initializes h, n and sigma
	void init() {
		std::fill(h.begin(), h.end(), BLOCK_SIZE == 32 ? 0x01 : 0x00);
		std::fill(n.begin(), n.end(), 0x00);
		std::fill(sigma.begin(), sigma.end(), 0x00);
	}

    // A method for updating the current state of a hash function with data from a file
	void update(std::ifstream &file) {
		// Получаем размер файла
		file.seekg(0, std::ios::end);
		size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

        // Buffer for file reading
		std::vector<uint8_t> readBuffer(4096);

		for (int i = 0; i < (int)(fileSize / 4096); i++) {
            // Read a block of 4096 bytes from the file
			file.read((char *)readBuffer.data(), readBuffer.size());
            // Invert the bytes of the block
			std::reverse(readBuffer.begin(), readBuffer.end());
            // Update the state of the hash function with the block
			this->_updateState(readBuffer);
			readBuffer.resize(4096);
		}

		int lastBlockSize = (int)(fileSize % 4096);
		readBuffer.resize(lastBlockSize);
        // Reads last block
		file.read((char *)readBuffer.data(), readBuffer.size());
		std::reverse(readBuffer.begin(), readBuffer.end());
		this->_pad(readBuffer);
		this->_updateState(readBuffer);
		
        // Finalizing
		this->_finalize(readBuffer);
	}

    // A method to get the result of a hash function as an array of bytes
	std::vector<uint8_t> digest() const {
		return this->_digest;
	}


   private:
	std::vector<uint8_t> _digest;

	void _setNewDigestSize(int newSize) {
		newSize = (int)(newSize / 8);
		if (newSize > 0) {
			this->_digest.resize(newSize);
		}
	}


	std::vector<uint8_t> h;
	std::vector<uint8_t> n;
	std::vector<uint8_t> sigma;

	// Pads a given buffer with zeroes and a single 0x01 byte until its size is BLOCK_SIZE
	void _pad(std::vector<uint8_t> &buffer) {
		if (buffer.size() < BLOCK_SIZE) {
			std::vector<uint8_t> paddedData(BLOCK_SIZE, 0x00);
			paddedData[BLOCK_SIZE - buffer.size() - 1] = 0x01;
			std::copy(buffer.begin(), buffer.end(), paddedData.begin() + (BLOCK_SIZE - buffer.size()));
			buffer = paddedData;
		}
	}

	void _updateState(std::vector<uint8_t> &buffer) {
		std::vector<uint8_t> v512(BLOCK_SIZE, 0x00);
		v512[62] = 0x02;

		while (buffer.size() >= BLOCK_SIZE) {
			std::vector<uint8_t> block(buffer.end() - BLOCK_SIZE, buffer.end());
			buffer.erase(buffer.end() - BLOCK_SIZE, buffer.end());

			h = _compress(n, h, block);
			n = _add512(n, v512);
			sigma = _add512(sigma, block);
		}
	}

	void _finalize(std::vector<uint8_t> &buffer) {
		std::vector<uint8_t> v512_0(BLOCK_SIZE, 0x00);
		std::vector<uint8_t> v512(BLOCK_SIZE, 0x00);

		uint32_t msgBitSize = buffer.size() * 8;
		v512[60] = (uint8_t)(msgBitSize >> 24);
		v512[61] = (uint8_t)(msgBitSize >> 16);
		v512[62] = (uint8_t)(msgBitSize >> 8);
		v512[63] = (uint8_t)(msgBitSize);

		this->_pad(buffer);

		h = _compress(n, h, buffer);

		n = _add512(n, v512);
		sigma = _add512(sigma, buffer);

		h = _compress(v512_0, h, n);
		h = _compress(v512_0, h, sigma);

		std::reverse(h.begin(), h.end());

		if (BLOCK_SIZE == 32) {
			std::copy(h.begin() + 32, h.end(), this->_digest.begin());
		} else {
			this->_digest = h;
		}
	}

	// XOR two 512-bit vectors a and b and return the result.
	std::vector<uint8_t> _xor512(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
		std::vector<uint8_t> result(BLOCK_SIZE, 0x00);
		for (int i = 0; i < BLOCK_SIZE; i++) {
			result[i] = (uint8_t)(a[i] ^ b[i]);
		}

		return result;
	}

	std::vector<uint8_t> _add512(const std::vector<uint8_t> &a, const std::vector<uint8_t> &b) {
		std::vector<uint8_t> result(BLOCK_SIZE, 0x00);
		uint32_t tmp = 0;

		for (int i = 63; i >= 0; i--) {
			tmp = a[i] + b[i] + (tmp >> 8);
			result[i] = (uint8_t)(tmp & 0xff);
		}

		return result;
	}

	std::vector<uint8_t> _getIterKey(const std::vector<uint8_t> &k, int i) {
		std::vector<uint8_t> result(BLOCK_SIZE, 0x00);
		result = this->_xor512(k, C[i]);
		result = this->_lpsTransform(result);
		return result;
	}

	// LPS (Linear Permutation Shift) 
	std::vector<uint8_t> _lpsTransform(const std::vector<uint8_t> &state) {
		// S
		uint8_t sTransformed[BLOCK_SIZE] = {0x00};
		for (int i = 0; i < BLOCK_SIZE; i++) {
			sTransformed[i] = P[state[i]];
		}

		// P
		uint8_t pTransformed[BLOCK_SIZE] = {0x00};
		for (int i = 0; i < BLOCK_SIZE; i++) {
			pTransformed[i] = sTransformed[T[i]];
		}

		// L
		std::vector<uint8_t> result(BLOCK_SIZE, 0x00);
		for (int i = 0; i < 8; i++) {
			uint64_t tmp64Result = 0;

			uint64_t tmp64 = ((uint64_t)pTransformed[i * 8] << 56) |
							 ((uint64_t)pTransformed[(i * 8) + 1] << 48) |
							 ((uint64_t)pTransformed[(i * 8) + 2] << 40) |
							 ((uint64_t)pTransformed[(i * 8) + 3] << 32) |
							 ((uint64_t)pTransformed[(i * 8) + 4] << 24) |
							 ((uint64_t)pTransformed[(i * 8) + 5] << 16) |
							 ((uint64_t)pTransformed[(i * 8) + 6] << 8) |
							 ((uint64_t)pTransformed[(i * 8) + 7]);

			for (int j = 0; j < BLOCK_SIZE; j++) {
				if ((tmp64 >> (63 - j)) & 1) {
					tmp64Result ^= A[j];
				}
			}

			result[i * 8] = (uint8_t)(tmp64Result >> 56);
			result[(i * 8) + 1] = (uint8_t)(tmp64Result >> 48);
			result[(i * 8) + 2] = (uint8_t)(tmp64Result >> 40);
			result[(i * 8) + 3] = (uint8_t)(tmp64Result >> 32);
			result[(i * 8) + 4] = (uint8_t)(tmp64Result >> 24);
			result[(i * 8) + 5] = (uint8_t)(tmp64Result >> 16);
			result[(i * 8) + 6] = (uint8_t)(tmp64Result >> 8);
			result[(i * 8) + 7] = (uint8_t)(tmp64Result);
		}

		return result;
	}

	std::vector<uint8_t> _eTranform(const std::vector<uint8_t> &k, const std::vector<uint8_t> &m) {
		std::vector<uint8_t> result = this->_xor512(m, k);
		std::vector<uint8_t> _K = k;

		for (int i = 0; i < 12; i++) {
			result = this->_lpsTransform(result);
			_K = this->_getIterKey(_K, i);
			result = this->_xor512(result, _K);
		}

		return result;
	}

	std::vector<uint8_t> _compress(const std::vector<uint8_t> &n, const std::vector<uint8_t> &h, const std::vector<uint8_t> &m) {
		std::vector<uint8_t> k = this->_xor512(n, h);
		k = this->_lpsTransform(k);

		std::vector<uint8_t> tmp = this->_eTranform(k, m);
		tmp = this->_xor512(tmp, h);
		tmp = this->_xor512(tmp, m);

		return tmp;
	}
};