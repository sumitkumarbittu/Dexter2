FROM gcc:latest

# Install git to clone the httplib repository
RUN apt-get update && \
    apt-get install -y git

# Clone cpp-httplib
RUN git clone https://github.com/yhirose/cpp-httplib.git

# Copy your source code
COPY server.cpp .

# Compile your C++ server, specifying the path to httplib.h
RUN g++ -std=c++11 server.cpp -o server

# Expose the port for the Render platform
EXPOSE 8080

# Start the server
CMD ["./server"]
