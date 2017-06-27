import { expect } from 'chai';
import SchemaType from '../../src/utils/SchemaType';

describe('schema type enumeration', () => {
	it('exposes expected types', () => {
		// Assert:
		expect(SchemaType).to.deep.equal({
			none: 0,
			object: 1,
			array: 2,
			dictionary: 3,
			max: 3
		});
	});
});
