router: router.out

router.out: main.cpp ip.conf IpAddress.cpp RouterTable.cpp
	g++ $^ -o $@ -std=c++17
	./$@

clean:
	rm *.out
