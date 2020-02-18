FROM ubuntu:latest

RUN apt-get update && \
	apt-get install -y build-essential git cmake libboost-all-dev libcurl4-gnutls-dev libomp-dev

WORKDIR /tcn3r

COPY CMakeLists.txt /tcn3r/
COPY src /tcn3r/src

RUN cmake CMakeLists.txt
RUN make

ENTRYPOINT ["./src/tcn3r"]
