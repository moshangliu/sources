import os

if __name__ == '__main__':
	os.system('sh ./build.sh');
	testList = os.popen('make list').read().strip().split(' ')

	for testCase in testList:
		os.system('./%s' %(testCase));

	os.system('make clean')
