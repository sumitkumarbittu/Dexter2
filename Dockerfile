FROM gcc:latest

RUN apt-get update && \
    apt-get install -y git

RUN git clone https://github.com/yhirose/cpp-httplib.git

COPY server.cpp .

RUN g++ -std=c++11 server.cpp -o server

EXPOSE 8080

CMD ["./server"]
