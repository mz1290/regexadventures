FROM alpine

RUN apk update
RUN apk add build-base git gdb vim wget curl make automake autoconf cmake

# re2 library
WORKDIR /opt
RUN git clone https://github.com/google/re2.git && \
	cd re2 && \
	make && make install

# pcre2 library
WORKDIR /opt
RUN curl -L -o pcre2.zip https://ftp.pcre.org/pub/pcre/pcre2-10.34.zip && \
	unzip pcre2.zip && \
	rm pcre2.zip && \
	cd pcre2-10.34 && \
	./configure --enable-jit --with-pic && \
	make && make install

# C++ yaml library
WORKDIR /opt
RUN git clone https://github.com/jbeder/yaml-cpp.git && \
	cd yaml-cpp && \
	mkdir -p build && cd build && \
	cmake .. -DYAML_BUILD_SHARED_LIBS=ON && \
	make && make install

# Symlinks to /usr/local/lib
RUN ln -s /opt/yaml-cpp/build/libyaml-cpp.so /usr/local/lib/libyaml-cpp.so && \
	ln -s /opt/yaml-cpp/build/libyaml-cpp.so.0.6 /usr/local/lib/libyaml-cpp.so.0.6 && \
	ln -s /opt/yaml-cpp/build/libyaml-cpp.so.0.6.3 /usr/local/lib/libyaml-cpp.so.0.6.3

WORKDIR /home
