SHELL :=	/bin/sh
PWD :=		$(shell pwd)

container: build-image
	docker container run \
	-it --rm \
	-v $(PWD):/home \
	regex_benchmark sh

build-image: Dockerfile
	docker image build \
		-t regex_benchmark .

run: compile
	@./bin/main

run-pcre2-demo: compile-pcre2-demo
	@./bin/pcre2demo $(PATTERN) $(SUBJECT)

compile:
	@g++ -Wall -g main.cpp \
	src/Log_parser.cpp \
	src/Results.cpp \
	src/pcre2_mz.cpp \
	-o bin/main -lyaml-cpp -lre2 -lpcre2-8 -fopenmp

compile-pcre2-demo:
	@g++ -Wall -g pcre2_demo.cpp -o bin/pcre2demo -lpcre2-8

clean:
	rm bin/*