FROM gcc:latest
WORKDIR /app
COPY . .
RUN g++ -o server server.cpp
EXPOSE 8080
CMD ["./server"]
