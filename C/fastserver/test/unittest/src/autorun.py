import os

if __name__ == '__main__':
	os.system('make clean && make');
	testList = os.popen('make list').read().strip().split(' ')

	for testCase in testList:
		os.system('./%s' %(testCase));