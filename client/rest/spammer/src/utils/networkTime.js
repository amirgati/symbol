function getNetworkTime() {
	// 1459468800000 is the number of milliseconds from 1970-01-01 till epoch time (2016-04-01)
	return new Date().getTime() - 1459468800000;
}

export default { getNetworkTime };
