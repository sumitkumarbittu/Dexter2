FROM gcc:latest

# Install git and wget
RUN apt-get update && apt-get install -y git wget

# Clone httplib and download nlohmann/json single-header
RUN git clone https://github.com/yhirose/cpp-httplib.git && \
    mkdir -p nlohmann && \
    wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp -O nlohmann/json.hpp

# Copy project files
COPY server.cpp .
COPY index.html .

# Compile with includes
RUN g++ -std=c++17 server.cpp -o server -Icpp-httplib -Inlohmann

EXPOSE 8080

CMD ["./server"]
