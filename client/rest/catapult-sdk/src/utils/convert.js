import charMapping from './charMapping';

const Char_To_Nibble_Map = (function () {
	const builder = charMapping.createBuilder();
	builder.addRange('0', '9', 0);
	builder.addRange('a', 'f', 10);
	builder.addRange('A', 'F', 10);
	return builder.map;
})();

const Char_To_Digit_Map = (function () {
	const builder = charMapping.createBuilder();
	builder.addRange('0', '9', 0);
	return builder.map;
})();

const Nibble_To_Char_Map = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'];

function tryParseByte(char1, char2) {
	const nibble1 = Char_To_Nibble_Map[char1];
	const nibble2 = Char_To_Nibble_Map[char2];
	return undefined === nibble1 || undefined === nibble2
		? undefined
		: (nibble1 << 4) | nibble2;
}

/** @exports utils/convert */
const convert = {
	/**
	 * Decodes two hex characters into a byte.
	 * @param {string} char1 The first hex digit.
	 * @param {string} char2 The second hex digit.
	 * @returns {numeric} The decoded byte.
	 */
	toByte: (char1, char2) => {
		const byte = tryParseByte(char1, char2);
		if (undefined === byte)
			throw Error(`unrecognized hex char '${char1}${char2}'`);

		return byte;
	},

	/**
	 * Determines whether or not a string is a hex string.
	 * @param {string} input The string to test.
	 * @returns {boolean} true if the input is a hex string, false otherwise.
	 */
	isHexString: input => {
		if (0 !== input.length % 2)
			return false;

		for (let i = 0; i < input.length; i += 2) {
			if (undefined === tryParseByte(input[i], input[i + 1]))
				return false;
		}

		return true;
	},

	/**
	 * Converts a hex string to a uint8 array.
	 * @param {string} input A hex encoded string.
	 * @returns {Uint8Array} A uint8 array corresponding to the input.
	 */
	hexToUint8: input => {
		if (0 !== input.length % 2)
			throw Error(`hex string has unexpected size '${input.length}'`);

		const output = new Uint8Array(input.length / 2);
		for (let i = 0; i < input.length; i += 2)
			output[i / 2] = convert.toByte(input[i], input[i + 1]);

		return output;
	},

	/**
	 * Converts a uint8 array to a hex string.
	 * @param {Uint8Array} input A uint8 array.
	 * @returns {string} A hex encoded string corresponding to the input.
	 */
	uint8ToHex: input => {
		let s = '';
		for (const byte of input) {
			s += Nibble_To_Char_Map[byte >> 4];
			s += Nibble_To_Char_Map[byte & 0x0F];
		}

		return s;
	},

	/**
	 * Tries to parse a string representing an unsigned integer.
	 * @param {string} str The string to parse.
	 * @returns {numeric} The number represented by the input or undefined.
	 */
	tryParseUint: str => {
		if ('0' === str)
			return 0;

		let value = 0;
		for (const char of str) {
			const digit = Char_To_Digit_Map[char];
			if (undefined === digit || (0 === value && 0 === digit))
				return undefined;

			value *= 10;
			value += digit;

			if (value > Number.MAX_SAFE_INTEGER)
				return undefined;
		}

		return value;
	},

	/**
	 * Converts a uint8 array to a uint32 array.
	 * @param {Uint8Array} input A uint8 array.
	 * @returns {Uint32Array} A uint32 array created from the input.
	 */
	uint8ToUint32: input => new Uint32Array(input.buffer),

	/**
	 * Converts a uint32 array to a uint8 array.
	 * @param {Uint32Array} input A uint32 array.
	 * @returns {Uint8Array} A uint8 array created from the input.
	 */
	uint32ToUint8: input => new Uint8Array(input.buffer)
};

export default convert;
