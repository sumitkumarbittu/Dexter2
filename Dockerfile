FROM gcc:latest

RUN apt-get update && \
    apt-get install -y git

RUN git clone https://github.com/yhirose/cpp-httplib.git

COPY server.cpp .

RUN g++ server.cpp cpp-httplib/httplib.h -o server

CMD ["./server"]
